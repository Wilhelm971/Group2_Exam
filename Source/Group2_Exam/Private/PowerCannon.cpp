#include "PowerCannon.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"


APowerCannon::APowerCannon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APowerCannon::BeginPlay()
{
	Super::BeginPlay();
}

void APowerCannon::ReceivePower(APowerNode* FromNode)
{
	Super::ReceivePower(FromNode);
	// Call parent to mark as powered
	Super::ReceivePower(FromNode);

	// Start firing loop if not already active
	if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_Fire, this, &APowerCannon::TryShoot, FireInterval, true);

		UE_LOG(LogTemp, Log, TEXT("%s is now powered and ready to fire!"), *GetName());
	}
}

void APowerCannon::LosePower()
{
	Super::LosePower();
	// Stop firing
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	UE_LOG(LogTemp, Warning, TEXT("%s lost power and stopped firing."), *GetName());
}

void APowerCannon::TryShoot()
{
	if (!bIsPowered) return;

	// Find nearest enemy within AttackRange
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Enemies);

	AActor* ClosestEnemy = nullptr;
	float ClosestDist = FLT_MAX;

	for (AActor* Enemy : Enemies)
	{
		if (!Enemy) continue;

		float Dist = FVector::Dist(Enemy->GetActorLocation(), GetActorLocation());
		if (Dist < AttackRange && Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestEnemy = Enemy;
		}
	}

	if (ClosestEnemy)
	{
		FireAtEnemy(ClosestEnemy);
	}
}

void APowerCannon::FireAtEnemy(AActor* Target)
{
	if (!Target) return;

	// You could spawn a projectile or apply damage directly
	UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, nullptr);

	DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(),
		FColor::Red, false, 0.2f, 0, 2.0f);

	UE_LOG(LogTemp, Log, TEXT("%s fired at %s"), *GetName(), *Target->GetName());
}
