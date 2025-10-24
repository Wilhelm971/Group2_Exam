/*
#include "PlacementController.h"
#include "BuildingPreview.h"
#include "Building.h"
#include "Kismet/GameplayStatics.h"
#include "RealGameMode.h"
#include "Engine/World.h"

APlacementController::APlacementController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlacementController::BeginPlay()
{
	Super::BeginPlay();

	if (!GameModeRef)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARealGameMode::StaticClass(), Found);
		if (Found.Num() > 0) GameModeRef = Cast<ARealGameMode>(Found[0]);
	}
}

void APlacementController::StartPlacing(TSubclassOf<ABuilding> BuildClass)
{
	if (!PreviewClass || !BuildClass) return;

	// spawn preview
	if (CurrentPreview) CurrentPreview->Destroy();
	FActorSpawnParameters P;
	CurrentPreview = GetWorld()->SpawnActor<ABuildingPreview>(PreviewClass, FVector::ZeroVector, FRotator::ZeroRotator, P);
	// TODO: set preview mesh and other visuals based on BuildClass (BP friendly)
}

bool APlacementController::ConfirmPlacement(const FVector& WorldLocation, const FRotator& WorldRotation)
{
	if (!CurrentPreview) return false;

	// determine chosen build class from preview -> for simplicity assume preview implies class set externally
	// For example, set a Tag or variable; here we will assume user sets the proper class in Preview actor or controller.

	// TODO: in your game, you should pass the chosen BuildClass; here we'll just assume CannonClass for demo
	TSubclassOf<ABuilding> Chosen = CannonClass;
	if (!Chosen) return false;

	// check funds
	if (GameModeRef && GameModeRef->Funds < Chosen.GetDefaultObject()->BuildCost)
	{
		// not enough money
		return false;
	}

	// deduct funds
	if (GameModeRef) GameModeRef->Funds -= Chosen.GetDefaultObject()->BuildCost;

	// spawn actual building
	FActorSpawnParameters P;
	P.Owner = this;
	ABuilding* NewB = GetWorld()->SpawnActor<ABuilding>(Chosen, WorldLocation, WorldRotation, P);
	if (NewB)
	{
		// register to power network manager automatically if manager exists will be in building BeginPlay
	}

	CurrentPreview->Destroy();
	CurrentPreview = nullptr;
	return true;
}

void APlacementController::CancelPlacement()
{
	if (CurrentPreview) { CurrentPreview->Destroy(); CurrentPreview = nullptr; }
}
*/