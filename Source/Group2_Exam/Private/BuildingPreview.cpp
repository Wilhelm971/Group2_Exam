/*#include "BuildingPreview.h"
#include "Components/StaticMeshComponent.h"

ABuildingPreview::ABuildingPreview()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetMobility(EComponentMobility::Movable);
}

void ABuildingPreview::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildingPreview::SetValidPlacement(bool bValid)
{
	// TODO: change material or color to indicate valid placement
}
*/