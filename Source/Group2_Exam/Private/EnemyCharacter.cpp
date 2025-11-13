


#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "PowerCore.h"
#include "Engine/World.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;

	// Movement: Direct velocity (works WITHOUT AIController for spawned enemies)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;  // Top-down friendly
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Enemy %s spawned - searching for PowerCore..."), *GetName());

	FindTargetBase();

	if (TargetBase)
	{
		UE_LOG(LogTemp, Log, TEXT("✓ Enemy %s locked onto: %s (dist: %.0f)"), *GetName(), *TargetBase->GetName(),
			   FVector::Dist(GetActorLocation(), TargetBase->GetActorLocation()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ Enemy %s NO TARGET - will retry in Tick"));
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// RETRY search if no target (fixes spawn timing)
	TimeSinceLastSearch += DeltaTime;
	if (!TargetBase && TimeSinceLastSearch >= SearchInterval)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy %s retrying target search..."), *GetName());
		FindTargetBase();
		TimeSinceLastSearch = 0.0f;
	}

	// CHASE TARGET
	if (TargetBase && IsValid(TargetBase))
	{
		FVector Direction = (TargetBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		GetCharacterMovement()->Velocity = Direction * MoveSpeed;

		// ATTACK if close (<150uu)
		float Distance = FVector::Dist(GetActorLocation(), TargetBase->GetActorLocation());
		if (Distance < 150.0f)
		{
			// Damage core over time
			TargetBase->Health -= 100.0f * DeltaTime;  // 20 DPS
			UE_LOG(LogTemp, Log, TEXT("Enemy %s attacking base! Health: %.0f"), *GetName(), TargetBase->Health);

			if (TargetBase->Health <= 0.0f)
			{
				UE_LOG(LogTemp, Error, TEXT("GAME OVER! Base destroyed by %s"), *GetName());
				TargetBase->Destroy();
			}
		}
	}
	else if (TargetBase)  // Invalid/destroyed
	{
		TargetBase = nullptr;
	}
}


float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
								  AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		CurrentHealth -= ActualDamage;
		UE_LOG(LogTemp, Warning, TEXT("Enemy %s hit! Health: %.0f/%.0f"), *GetName(), CurrentHealth, MaxHealth);

		if (CurrentHealth <= 0.0f)
		{
			UE_LOG(LogTemp, Log, TEXT("Enemy %s DIED!"), *GetName());
			Destroy();
		}
	}
	
	return ActualDamage;
}

void AEnemyCharacter::TakeDamageFromCannon(float Damage)
{
	TakeDamage(Damage, FDamageEvent{}, nullptr, nullptr);
}

void AEnemyCharacter::FindTargetBase()
{
	TArray<AActor*> Bases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Bases);

	UE_LOG(LogTemp, Log, TEXT("Enemy %s scanned %d cores"), *GetName(), Bases.Num());

	if (Bases.Num() == 0)
	{
		TargetBase = nullptr;
		return;
	}

	// Pick CLOSEST (multi-core safe)
	TargetBase = nullptr;
	float ClosestDist = FLT_MAX;
	for (AActor* BaseActor : Bases)
	{
		if (APowerCore* Base = Cast<APowerCore>(BaseActor))
		{
			if (IsValid(Base))
			{
				float Dist = FVector::Dist(GetActorLocation(), Base->GetActorLocation());
				if (Dist < ClosestDist)
				{
					ClosestDist = Dist;
					TargetBase = Base;
				}
			}
		}
	}
}


