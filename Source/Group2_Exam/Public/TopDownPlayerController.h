// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TopDownPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class APawn;  // Forward declaration for ControlledPawn
class APowerCannon;  // Forward declaration (replaces include for CannonToPlaceClass and PreviewCannon)

/**
 * ATopDownPlayerController
 * 
 * Player controller for a top-down tower defense game.
 * Manages camera movement, input handling, and building/placement of towers.
 */
UCLASS()
class GROUP2_EXAM_API ATopDownPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    ATopDownPlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;

    // =============================================================
    // BUILDING PROPERTIES
    // =============================================================
    /** Class of the cannon to spawn during building mode. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TSubclassOf<APowerCannon> CannonToPlaceClass;

    /** Collision channel used for placement traces (e.g., under cursor). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TEnumAsByte<ECollisionChannel> PlacementTraceChannel = ECC_Visibility;

    /** Preview instance of the cannon during building mode. */
    UPROPERTY()
    APowerCannon* PreviewCannon;

    /** Flag indicating if building mode is active. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bBuildingModeActive = false;


    // =============================================================
    // ECONOMY PROPERTIES (NEW)
    // =============================================================
    /** Current number of coins the player has. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
    float CurrentCoins = 0.0f;

    /** Cost to place a cannon. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CannonCost = 50.0f;

    /** Coins earned per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CoinsPerSecond = 7.0f;

    // =============================================================
    // CAMERA PROPERTIES
    // =============================================================
    /** Speed of camera panning movement. */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float PanSpeed = 2000.0f;

    /** Speed of camera zooming. */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float ZoomSpeed = 5000.0f;

    /** Minimum zoom distance (arm length). */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinZoom = 50.0f;

    /** Maximum zoom distance (arm length). */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxZoom = 5000.0f;

    /** Target arm length for smooth zooming interpolation. */
    float TargetArmLength = 2500.0f;

    /** Cached reference to the controlled pawn (camera pawn). */
    APawn* ControlledPawn;

    // =============================================================
    // INPUT PROPERTIES
    // =============================================================
    /** Input mapping context for enhanced input system. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputMappingContext* InputMapping;

    /** Input action for camera movement. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* MoveAction;

    /** Input action for camera zooming. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* ZoomAction;

    /** Input action to toggle building mode. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* BuildAction;

    /** Input action to place the building. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* PlaceAction;

    /** Input action to cancel building mode. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* CancelAction;

    // =============================================================
    // PUBLIC FUNCTIONS (INPUT HANDLERS)
    // =============================================================
    /** Handles camera panning input. */
    void HandleMove(const FInputActionValue& Value);

    /** Handles camera zoom input. */
    void HandleZoom(const FInputActionValue& Value);

    /** Toggles building mode on/off. */
    void ToggleBuildingMode(const FInputActionValue& Value);

    /** Places the preview cannon if valid. */
    void PlaceBuilding(const FInputActionValue& Value);

    /** Cancels building mode. */
    void CancelBuilding(const FInputActionValue& Value);

private:
    // =============================================================
    // PRIVATE FUNCTIONS (BUILDING HELPERS)
    // =============================================================
    /** Initializes building mode and spawns preview. */
    void StartBuildingMode();

    /** Cleans up building mode and destroys preview. */
    void CancelBuildingMode();

    /** Updates the preview cannon's position based on cursor. */
    void UpdatePreviewPosition();

    // =============================================================
    // PRIVATE FUNCTIONS (ECONOMY HELPERS - NEW)
    // =============================================================
    /** Timer-based function to earn coins over time. */
    void EarnCoins();

    /** Timer handle for coin earning. */
    FTimerHandle CoinTimerHandle;
};