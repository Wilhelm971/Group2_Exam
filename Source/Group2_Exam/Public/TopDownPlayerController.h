

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PowerCannon.h"
#include "TopDownPlayerController.generated.h"

/**
 * A Player Controller for a top-down tower defence game
 * Handles camera movement, mouse cursor visibility and later the buying new towers
 */


class UInputMappingContext;
class UInputAction;

UCLASS()
class GROUP2_EXAM_API ATopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATopDownPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSubclassOf<APowerCannon> CannonToPlaceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TEnumAsByte<ECollisionChannel> PlacementTraceChannel = ECC_Visibility;

	UPROPERTY()
	APowerCannon* PreviewCannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	bool bBuildingModeActive = false;

private:
	// --- CAMERA ---
	UPROPERTY(EditAnywhere, Category="Camera")
	float PanSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float MinZoom = 50.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float MaxZoom = 5000.0f;

	// Current zoom target length
	float TargetArmLength = 2500.0f;

	// Cached reference to the controlled pawn (the top-down camera pawn)
	APawn* ControlledPawn;



public:
	// --- INPUT SYSTEM ---
	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	UInputAction* BuildAction;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	UInputAction* PlaceAction;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	UInputAction* CancelAction;

	// --- INPUT HANDLERS ---
	void HandleMove(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);

	void ToggleBuildingMode(const FInputActionValue& Value);
	void PlaceBuilding(const FInputActionValue& Value);
	void CancelBuilding(const FInputActionValue& Value);


private:
	void StartBuildingMode();
	void CancelBuildingMode();
	void UpdatePreviewPosition();
	
};
