


#include "Cannon.h"

ACannon::ACannon()
{

	PrimaryActorTick.bCanEverTick = true;
	
	PowerNeededPerShot = 10.0f;
	FireCooldown = 1.2f;
	TimeSinceLastFire = 0.0f;

	bNeedsPower = true;
}

void ACannon::BeginPlay()
{
	Super::BeginPlay();
	TimeSinceLastFire = FireCooldown;
}

void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceLastFire += DeltaTime;
	TryFire();
}

void ACannon::OnReceivePower(float amount)
{
	ApplyPower(amount);
}

void ACannon::TryFire()
{
	if ( StoredPower >= PowerNeededPerShot && TimeSinceLastFire >= FireCooldown )
	{
		StoredPower -= PowerNeededPerShot;
		TimeSinceLastFire = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("ShotFired"));
	}
}

