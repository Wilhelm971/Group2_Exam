


#include "Building.h"
#include "Engine/Engine.h"


ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = false;
	bNeedsPower = true;
	StoredPower = 0.0f;
	BuildCost = 50;
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABuilding::ReceivePower(float Amount)
{
	if (!bNeedsPower || Amount <= 0.f) return;
	StoredPower += Amount;
	OnReceivePower(Amount);

	// Debug
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Cyan,
			FString::Printf(TEXT("%s received %.1f power (stored %.1f)"), *GetName(), Amount, StoredPower));
	}
}

void ABuilding::OnReceivePower(float Amount)
{
	// base: nothing extra
}

void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}