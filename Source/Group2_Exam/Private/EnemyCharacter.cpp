#include "EnemyCharacter.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GridManager.h"                 // <-- NEW

// =============================================================
// CONSTRUCTOR
// =============================================================
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

// =============================================================
// BEGIN PLAY
// =============================================================
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Grab the singleton GridManager (must be placed in the level)
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		GridMgr = Cast<AGridManager>(Found[0]);
		UE_LOG(LogTemp, Warning, TEXT("Enemy %s found GridManager"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("!!! NO GRIDMANAGER IN LEVEL !!!"));
	}

	FindClosestTarget();
}

// =============================================================
// TICK
// =============================================================
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
		CalculateGridPath();
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
			const FVector Next = GetNextPathPoint();
			const FVector Dir = (Next - GetActorLocation()).GetSafeNormal();
			GetCharacterMovement()->Velocity = Dir * MoveSpeed;
		}
	}
}

// =============================================================
// FIND CLOSEST TOWER
// =============================================================
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

// =============================================================
// GRID-BASED A* (uses your GridManager)
// =============================================================
void AEnemyCharacter::CalculateGridPath()
{
	if (!GridMgr || !TargetTower) return;

	// Convert world positions → grid indices
	FVector StartWorld = GetActorLocation();
	FVector EndWorld   = TargetTower->GetActorLocation();

	FIntPoint StartIdx = GridMgr->WorldToGridIndex(StartWorld);
	FIntPoint EndIdx   = GridMgr->WorldToGridIndex(EndWorld);

	// Run A* on the grid (exactly like NodeActor does)
	TArray<FIntPoint> GridPath = GridMgr->FindPath(StartIdx, EndIdx);

	PathPoints.Empty();
	CurrentPathIndex = 0;

	if (GridPath.Num() > 0)
	{
		// Convert grid indices back to world locations (center of cell)
		for (const FIntPoint& Idx : GridPath)
		{
			FVector World = GridMgr->GridToWorldCenter(Idx);
			PathPoints.Add(World);
		}

		UE_LOG(LogTemp, Log, TEXT("A* path: %d points"), PathPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No grid path to %s"), *TargetTower->GetName());
	}
}

// =============================================================
// GET NEXT WAYPOINT
// =============================================================
FVector AEnemyCharacter::GetNextPathPoint()
{
	if (PathPoints.IsValidIndex(CurrentPathIndex))
	{
		// If we are close enough to the current point, advance
		const FVector Cur = PathPoints[CurrentPathIndex];
		if (FVector::Dist(GetActorLocation(), Cur) < 60.f)
		{
			++CurrentPathIndex;
		}
		return Cur;
	}

	// Fallback – go straight to target
	return TargetTower ? TargetTower->GetActorLocation() : GetActorLocation();
}

// =============================================================
// ATTACK
// =============================================================
void AEnemyCharacter::AttackTarget()
{
	if (!TargetTower || !IsValid(TargetTower)) return;

	const float Damage = 30.f * GetWorld()->DeltaTimeSeconds;
	TargetTower->TakeDamageCustom(Damage);
}

// =============================================================
// DAMAGE (enemy itself)
// =============================================================
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