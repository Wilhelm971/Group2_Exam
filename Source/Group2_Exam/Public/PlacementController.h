

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlacementController.generated.h"

class ABuilding;
class ABuildingPreview;
class UUserWidget;


UCLASS()
class GROUP2_EXAM_API APlacementController : public APlayerController
{
	GENERATED_BODY()

public:
	APlacementController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	// Building types (set in editor)
	UPROPERTY(EditAnywhere, Category="Buildings")
	TSubclassOf<ABuilding> BuildingClass;
	
	// Preview class (usually a small actor with same mesh)
	UPROPERTY(EditAnywhere, Category="Buildings")
	TSubclassOf<ABuildingPreview> PreviewClass;
	
	// HUD widget class
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// Grid size for snapping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placement")
	float GridSize = 200.f;

	// Resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 Resources = 200;

protected:
	// Current preview instance
	UPROPERTY()
	ABuildingPreview* CurrentPreview;


	// Which building is selected (for this example we only have one)
	bool bPlacing = false;

	// Input handlers
	void OnLeftClick();
	void OnRightClick();
	void OnRotate();

	// Helpers
	bool UpdatePreviewLocation();
	FVector SnapLocationToGrid(const FVector& Location) const;
	bool CanPlaceAtLocation(const FVector& Location, FHitResult& OutHit);

	// The rotation for placement
	FRotator CurrentRotation = FRotator::ZeroRotator;

	// UI instance
	UPROPERTY()
	UUserWidget* HUDWidget;
	
};
