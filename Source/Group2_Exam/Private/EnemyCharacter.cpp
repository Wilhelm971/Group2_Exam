


#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PowerCore.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentHealth = MaxHealth;

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
		AddMovementInput(Direction, 1.0f);
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

