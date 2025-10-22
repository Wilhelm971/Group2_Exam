#include "PlacementController.h"
#include "Building.h"
#include "BuildingPreview.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

APlacementController::APlacementController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void APlacementController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			// Optionally store references (e.g., to text blocks) in the widget blueprint to update resources.
		}
	}
}

void APlacementController::SetupInputComponent()
{
	Super::SetupInputComponent();

	Super::SetupInputComponent();

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &APlacementController::OnLeftClick);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &APlacementController::OnRightClick);
	InputComponent->BindAction("Rotate", IE_Pressed, this, &APlacementController::OnRotate);
	// You must create these input actions inside Project Settings -> Input (LeftClick, RightClick, Rotate)

}

void APlacementController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bPlacing && CurrentPreview)
	{
		UpdatePreviewLocation();
	}
}

void APlacementController::OnLeftClick()
{
	if (!bPlacing)
	{
		// Start placing if we have a building selected
		if (PreviewClass)
		{
			CurrentPreview = GetWorld()->SpawnActor<ABuildingPreview>(PreviewClass, FVector::ZeroVector, CurrentRotation);
			bPlacing = true;
		}
	}
	else
	{
		// Try to place
		FHitResult Hit;
		if (!CurrentPreview) return;

		FVector PlaceLoc = CurrentPreview->GetActorLocation();
		if (ABuilding* BuildingCDO = BuildingClass ? BuildingClass->GetDefaultObject<ABuilding>() : nullptr)
		{
			int32 Cost = BuildingCDO->Cost;
			if (Resources >= Cost)
			{
				// Additional collision test to ensure no overlaps: perform sweep using the mesh bounds or a box
				FCollisionShape Shape = FCollisionShape::MakeBox(FVector(100.f)); // tune to your mesh size
				bool bHit = GetWorld()->OverlapAnyTestByChannel(PlaceLoc, FQuat::Identity, ECC_WorldStatic, Shape);
				if (!bHit)
				{
					// Spawn real building
					FActorSpawnParameters Params;
					Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					GetWorld()->SpawnActor<ABuilding>(BuildingClass, PlaceLoc, CurrentRotation, Params);

					Resources -= Cost;
					// Update UI: call a function on the widget or broadcast an event
					// e.g., cast widget to your UHUDWidget subclass and call UpdateResources(Resources)

					// End placing or keep placing (for repeated placements you might keep placing)
					bPlacing = false;
					CurrentPreview->Destroy();
					CurrentPreview = nullptr;
				}
				else
				{
					// invalid placement - maybe play sound / flash UI
					// Optionally inform preview it's invalid
					CurrentPreview->SetIsValidPlacement(false);
				}
			}
			else
			{
				// Not enough resources - UI feedback
			}
		}
	}
}

void APlacementController::OnRightClick()
{
	// Cancel placing
	if (bPlacing)
	{
		bPlacing = false;
		if (CurrentPreview) CurrentPreview->Destroy();
		CurrentPreview = nullptr;
	}
}

void APlacementController::OnRotate()
{
	if (bPlacing && CurrentPreview)
	{
		CurrentRotation.Yaw += 90.f;
		CurrentPreview->SetActorRotation(CurrentRotation);
	}
}

bool APlacementController::UpdatePreviewLocation()
{
	FHitResult Hit;
	if (GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1 /* visibility */, true, Hit))
	{
		FVector Snapped = SnapLocationToGrid(Hit.Location);
		CurrentPreview->SetActorLocation(Snapped);
		// Simple overlap test to decide valid placement
		FCollisionShape Shape = FCollisionShape::MakeBox(FVector(100.f));
		bool bOverlap = GetWorld()->OverlapAnyTestByChannel(Snapped, FQuat::Identity, ECC_WorldStatic, Shape);
		CurrentPreview->SetIsValidPlacement(!bOverlap);
		return true;
	}
	return false;
}

FVector APlacementController::SnapLocationToGrid(const FVector& Location) const
{
	FVector Result = Location;
	Result.X = FMath::RoundToFloat(Location.X / GridSize) * GridSize;
	Result.Y = FMath::RoundToFloat(Location.Y / GridSize) * GridSize;
	// Z depends on terrain; usually keep hit Z
	return Result;
}


