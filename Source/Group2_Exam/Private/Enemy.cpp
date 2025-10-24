#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "BaseHQ.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	// find HQ
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseHQ::StaticClass(), Found);
	if (Found.Num() > 0) HQTarget = Cast<ABaseHQ>(Found[0]);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HQTarget) return;

	float Dist = FVector::Dist(GetActorLocation(), HQTarget->GetActorLocation());
	if (Dist > AttackRange)
		MoveTowardsHQ(DeltaTime);
	else
		AttackHQ(DeltaTime);
}

void AEnemy::MoveTowardsHQ(float DeltaTime)
{
	if (!HQTarget) return;
	FVector Dir = (HQTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	AddMovementInput(Dir, 1.0f);
}

void AEnemy::AttackHQ(float DeltaTime)
{
	if (!HQTarget) return;
	float Damage = DamagePerSecond * DeltaTime;
	HQTarget->ReceiveDamage(Damage);
}

float AEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Applied = FMath::Min(CurrentHealth, DamageAmount);
	CurrentHealth -= Applied;
	if (CurrentHealth <= 0.f) Destroy();
	return Applied;
}
