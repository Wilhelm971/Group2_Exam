#include "Cannon.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = true;
	FireInterval = 1.2f;
	FireRange = 1200.f;
	DamagePerShot = 15.f;
	PowerPerShot = 10.f;
}

void ACannon::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FireTimer, this, &ACannon::TryFire, FireInterval, true, FireInterval);
}

void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Passive drain / clamping
	StoredPower = FMath::Max(0.f, StoredPower);
}

void ACannon::TryFire()
{
	if (StoredPower < PowerPerShot) return;

	AEnemy* Target = FindNearestEnemy();
	if (!Target) return;

	// Consume power
	StoredPower -= PowerPerShot;

	if (ProjectileClass)
	{
		FVector SpawnLoc = GetActorLocation() + FVector(0,0,50);
		FVector Dir = (Target->GetActorLocation() - SpawnLoc).GetSafeNormal();
		FRotator Rot = Dir.Rotation();

		FActorSpawnParameters P;
		P.Owner = this;
		AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLoc, Rot, P);
		if (Proj)
		{
			Proj->InitVelocity(Dir * 1400.f);
			Proj->Damage = DamagePerShot;
		}
	}
	else
	{
		UGameplayStatics::ApplyDamage(Target, DamagePerShot, nullptr, this, nullptr);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange,
			FString::Printf(TEXT("%s fired at %s"), *GetName(), *Target->GetName()));
	}
}

AEnemy* ACannon::FindNearestEnemy() const
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	AEnemy* Best = nullptr;
	float BestDist = FireRange;
	for (AActor* A : Enemies)
	{
		float D = FVector::Dist(A->GetActorLocation(), GetActorLocation());
		if (D < BestDist)
		{
			BestDist = D;
			Best = Cast<AEnemy>(A);
		}
	}
	return Best;
}