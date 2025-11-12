


#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PowerCore.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;

	// Configure movement for direct velocity control (no AIController needed)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);  // Smooth turning to face movement direction
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;  // Sync max speed

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Bases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Bases);
	if (Bases.Num() > 0)
	{
		TargetBase = Cast<APowerCore>(Bases[0]);
	}
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetBase)
	{
		FVector Direction = (TargetBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		GetCharacterMovement()->Velocity = Direction * MoveSpeed;
	}

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::TakeDamageFromCannon(float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.f)
	{
		Destroy();
	}
}

void AEnemyCharacter::FindTargetBase()
{
	TArray<AActor*> Bases;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Bases);
    
	UE_LOG(LogTemp, Log, TEXT("Enemy %s found %d PowerCores"), *GetName(), Bases.Num());
    
	if (Bases.Num() == 0)
	{
		TargetBase = nullptr;
		return;
	}
    
	// Pick CLOSEST (robust for multi-core)
	TargetBase = nullptr;
	float ClosestDist = FLT_MAX;
	for (AActor* BaseActor : Bases)
	{
		if (APowerCore* Base = Cast<APowerCore>(BaseActor))
		{

				float Dist = FVector::Dist(GetActorLocation(), Base->GetActorLocation());
				UE_LOG(LogTemp, Log, TEXT("  - Core %s at dist %.0f"), *Base->GetName(), Dist);
				if (Dist < ClosestDist)
				{
					ClosestDist = Dist;
					TargetBase = Base;
				}
			
		}
	}
}


