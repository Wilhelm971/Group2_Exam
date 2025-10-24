
/*
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementController.generated.h"

class ABuilding;
class ABuildingPreview;
class UUserWidget;


UCLASS()
class GROUP2_EXAM_API APlacementController : public AActor
{
	GENERATED_BODY()

public:
	APlacementController();

protected:
	virtual void BeginPlay() override;

public:
	// Preview actor class (ghost)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placement")
	TSubclassOf<ABuildingPreview> PreviewClass;

	// Building options
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placement")
	TSubclassOf<ABuilding> CannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placement")
	TSubclassOf<ABuilding> MineClass;

	// Current preview instance
	UPROPERTY(Transient)
	ABuildingPreview* CurrentPreview = nullptr;

	// Reference to GameMode for funds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placement")
	ARealGameMode* GameModeRef = nullptr;

	// Begin placing one of the building types
	UFUNCTION(BlueprintCallable)
	void StartPlacing(TSubclassOf<ABuilding> BuildClass);

	// Confirm placement at given world location and rotation
	UFUNCTION(BlueprintCallable)
	bool ConfirmPlacement(const FVector& WorldLocation, const FRotator& WorldRotation);

	// Cancel placement
	UFUNCTION(BlueprintCallable)
	void CancelPlacement();
};
*/