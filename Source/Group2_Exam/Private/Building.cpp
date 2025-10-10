


#include "Building.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bNeedsPower = true;

}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABuilding::OnReceivePower(float amount)
{
	ApplyPower(amount);
}

void ABuilding::ApplyPower(float amount)
{
	StoredPower += amount;
}



