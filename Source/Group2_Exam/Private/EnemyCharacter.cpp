#include "EnemyCharacter.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentHealth = MaxHealth;

	// ---- movement ------------------------------------------------
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	MoveComp->MaxWalkSpeed = MoveSpeed;
	MoveComp->GroundFriction = 4.f;
	MoveComp->BrakingDecelerationWalking = 0.f;
	MoveComp->bRequestedMoveUseAcceleration = false;

	// ---- collision -----------------------------------------------
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic,  ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn,        ECR_Ignore);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	FindClosestTarget();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ---- target retry ------------------------------------------------
	TimeSinceLastSearch += DeltaTime;
	if (!TargetTower && TimeSinceLastSearch >= SearchInterval)
	{
		FindClosestTarget();
		TimeSinceLastSearch = 0.f;
	}

	// ---- path recalc ------------------------------------------------
	TimeSincePathRecalc += DeltaTime;
	if (TargetTower && TimeSincePathRecalc >= PathRecalcInterval)
	{
		CalculatePathToTarget();
		TimeSincePathRecalc = 0.f;
	}

	// ---- move / attack -----------------------------------------------
	if (TargetTower && IsValid(TargetTower))
	{
		const float Dist = FVector::Dist(GetActorLocation(), TargetTower->GetActorLocation());

		if (Dist <= AttackRange)
		{
			AttackTarget();
		}
		else
		{
			const FVector TargetPos = bUsePathfinding ? GetNextPathPoint()
			                                          : TargetTower->GetActorLocation();
			const FVector Dir = (TargetPos - GetActorLocation()).GetSafeNormal();
			GetCharacterMovement()->Velocity = Dir * MoveSpeed;
		}
	}
}

/* --------------------------------------------------------------
   FIND CLOSEST TOWER (PowerNode = Cannon OR Core)
   -------------------------------------------------------------- */
void AEnemyCharacter::FindClosestTarget()
{
	TargetTower = nullptr;
	float BestDist = FLT_MAX;

	TArray<AActor*> Nodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), Nodes);

	for (AActor* A : Nodes)
	{
		if (APowerNode* N = Cast<APowerNode>(A))
		{
			if (IsValid(N))
			{
				const float D = FVector::Dist(GetActorLocation(), N->GetActorLocation());
				if (D < BestDist)
				{
					BestDist = D;
					TargetTower = N;
				}
			}
		}
	}

	if (TargetTower)
	{
		UE_LOG(LogTemp, Log, TEXT("%s → %s (%.0f)"), *GetName(),
		       *TargetTower->GetName(), BestDist);
	}
}

/* --------------------------------------------------------------
   A* PATHFINDING (synchronous)
   -------------------------------------------------------------- */
void AEnemyCharacter::CalculatePathToTarget()
{
	if (!TargetTower || !bUsePathfinding) return;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	FNavPathSharedPtr NavPath;
	const FVector Goal = TargetTower->GetActorLocation();

	const bool bPathFound = NavSys->FindPathToLocationSynchronously(
		GetWorld(),
		GetActorLocation(),
		Goal,
		FNavAgentProperties(),
		EPathFindingMode::Regular,  // ← FIXED: A* works!
		&NavPath);

	PathPoints.Empty();

	if (bPathFound && NavPath.IsValid())
	{
		const TArray<FNavPathPoint>& Points = NavPath->GetPathPoints();
		for (const FNavPathPoint& P : Points)
		{
			PathPoints.Add(P.Location);
		}
		UE_LOG(LogTemp, Log, TEXT("Path: %d points"), PathPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No path to %s"), *TargetTower->GetName());
	}
}

/* --------------------------------------------------------------
   NEXT WAYPOINT
   -------------------------------------------------------------- */
FVector AEnemyCharacter::GetNextPathPoint()
{
	if (PathPoints.Num() > 0)
	{
		// remove point when we are close enough
		const FVector First = PathPoints[0];
		if (FVector::Dist(GetActorLocation(), First) < 80.f)
		{
			PathPoints.RemoveAt(0);
		}
		return First;
	}
	return TargetTower ? TargetTower->GetActorLocation() : GetActorLocation();
}

/* --------------------------------------------------------------
   ATTACK
   -------------------------------------------------------------- */
void AEnemyCharacter::AttackTarget()
{
	if (!TargetTower || !IsValid(TargetTower)) return;

	const float DPS = 30.f;
	const float Damage = DPS * GetWorld()->DeltaTimeSeconds;

	if (APowerCannon* Cannon = Cast<APowerCannon>(TargetTower))
	{
		Cannon->TakeDamageCustom(Damage);
	}
	else if (APowerCore* Core = Cast<APowerCore>(TargetTower))
	{
		Core->Health -= Damage;
		if (Core->Health <= 0.f)
		{
			UE_LOG(LogTemp, Error, TEXT("GAME OVER – Core destroyed"));
			Core->Destroy();
		}
	}
}

/* --------------------------------------------------------------
   DAMAGE
   -------------------------------------------------------------- */
void AEnemyCharacter::TakeDamageCustom(float DamageAmount)
{
	if (DamageAmount <= 0.f) return;

	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.f)
	{
		UE_LOG(LogTemp, Log, TEXT("%s DIED"), *GetName());
		Destroy();
	}
}

void AEnemyCharacter::TakeDamageFromCannon(float Damage)
{
	TakeDamageCustom(Damage);
}

float AEnemyCharacter::TakeDamage(float DamageAmount,
                                  struct FDamageEvent const& DamageEvent,
                                  AController* EventInstigator,
                                  AActor* DamageCauser)
{
	return 0.f;   // we ignore the built-in UE damage system
}