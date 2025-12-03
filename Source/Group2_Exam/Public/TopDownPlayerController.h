/**
 * @file TopDownPlayerController.h
 * @brief Header file for the ATopDownPlayerController class, managing player input and camera in a top-down game.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MyUserWidget.h"
#include "TopDownPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class APawn;  // Forward declaration for ControlledPawn
class APowerCannon;  // Forward declaration (replaces include for CannonToPlaceClass and PreviewCannon)

/**
 * @class ATopDownPlayerController
 * @brief Player controller for a top-down tower defense game.
 * 
 * Manages camera movement, input handling, and building/placement of towers.
 * Also handles economy (coins) and UI updates.
 */
UCLASS()
class GROUP2_EXAM_API ATopDownPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    /**
     * @brief Default constructor for ATopDownPlayerController.
     * 
     * Sets up default properties.
     */
    ATopDownPlayerController();

    /**
     * @brief Called when the game starts.
     * 
     * Initializes UI, timers, and other runtime elements.
     */
    virtual void BeginPlay() override;

    /** 
     * @brief Instance of the custom user widget.
     */
    UPROPERTY()
    UMyUserWidget* MyWidgetInstance;

    /** 
     * @brief Class of the widget to instantiate.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> WidgetClass;

    /**
     * @brief Sets up the input component for enhanced input.
     */
    virtual void SetupInputComponent() override;
    
    /**
     * @brief Called every frame.
     * 
     * Handles runtime updates like preview position in building mode.
     * @param DeltaSeconds Time since last frame.
     */
    virtual void Tick(float DeltaSeconds) override;

    // =============================================================
    // BUILDING PROPERTIES
    // =============================================================
    /** 
     * @brief Class of the cannon to spawn during building mode.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TSubclassOf<APowerCannon> CannonToPlaceClass;

    /** 
     * @brief Collision channel used for placement traces (e.g., under cursor).
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TEnumAsByte<ECollisionChannel> PlacementTraceChannel = ECC_Visibility;

    /** 
     * @brief Preview instance of the cannon during building mode.
     */
    UPROPERTY()
    APowerCannon* PreviewCannon;

    /** 
     * @brief Flag indicating if building mode is active.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bBuildingModeActive = false;


    // =============================================================
    // ECONOMY PROPERTIES (NEW)
    // =============================================================
    /** 
     * @brief Current number of coins the player has.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
    float CurrentCoins = 0.0f;

    /** 
     * @brief Cost to place a cannon.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CannonCost = 30.0f;

    /** 
     * @brief Coins earned per second.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CoinsPerSecond = 5.0f;

    // =============================================================
    // UI
    // =============================================================

    /** 
     * @brief Class of the HUD widget.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    /** 
     * @brief Instance of the HUD widget.
     */
    UPROPERTY()
    UUserWidget* HUDWidget;
    

    /**
     * @brief Gets the count of active power cores.
     * 
     * @return The number of active cores.
     */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    int32 GetActiveCoreCount() const;

    /**
     * @brief Gets the health percentage of the core.
     * 
     * @return The health as a percentage (0.0 to 1.0).
     */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    float GetCoreHealthPercent() const;


    
    // =============================================================
    // CAMERA PROPERTIES
    // =============================================================
    /** 
     * @brief Speed of camera panning movement.
     * 
     * Editable in the editor.
     */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float PanSpeed = 2000.0f;

    /** 
     * @brief Speed of camera zooming.
     * 
     * Editable in the editor.
     */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float ZoomSpeed = 5000.0f;

    /** 
     * @brief Minimum zoom distance (arm length).
     * 
     * Editable in the editor.
     */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinZoom = 50.0f;

    /** 
     * @brief Maximum zoom distance (arm length).
     * 
     * Editable in the editor.
     */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxZoom = 5000.0f;

    /** 
     * @brief Target arm length for smooth zooming interpolation.
     */
    float TargetArmLength = 2500.0f;

    /** 
     * @brief Cached reference to the controlled pawn (camera pawn).
     */
    APawn* ControlledPawn;

    // =============================================================
    // INPUT PROPERTIES
    // =============================================================
    /** 
     * @brief Input mapping context for enhanced input system.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputMappingContext* InputMapping;

    /** 
     * @brief Input action for camera movement.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* MoveAction;

    /** 
     * @brief Input action for camera zooming.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* ZoomAction;

    /** 
     * @brief Input action to toggle building mode.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* BuildAction;

    /** 
     * @brief Input action to place the building.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* PlaceAction;

    /** 
     * @brief Input action to cancel building mode.
     * 
     * Editable in defaults only.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputAction* CancelAction;

    // =============================================================
    // PUBLIC FUNCTIONS (INPUT HANDLERS)
    // =============================================================
    /** 
     * @brief Handles camera panning input.
     * 
     * @param Value The input action value (vector for direction).
     */
    void HandleMove(const FInputActionValue& Value);

    /** 
     * @brief Handles camera zoom input.
     * 
     * @param Value The input action value (scalar for zoom amount).
     */
    void HandleZoom(const FInputActionValue& Value);

    /** 
     * @brief Toggles building mode on/off.
     */
	UFUNCTION(BlueprintCallable)
    void ToggleBuildingMode();

    /** 
     * @brief Places the preview cannon if valid.
     * 
     * @param Value The input action value.
     */
    void PlaceBuilding(const FInputActionValue& Value);

    /** 
     * @brief Cancels building mode.
     * 
     * @param Value The input action value.
     */
    void CancelBuilding(const FInputActionValue& Value);

private:
    // =============================================================
    // PRIVATE FUNCTIONS (BUILDING HELPERS)
    // =============================================================
    /** 
     * @brief Initializes building mode and spawns preview.
     */
    void StartBuildingMode();

    /** 
     * @brief Cleans up building mode and destroys preview.
     */
    void CancelBuildingMode();

    /** 
     * @brief Updates the preview cannon's position based on cursor.
     */
    void UpdatePreviewPosition();

    // =============================================================
    // PRIVATE FUNCTIONS (ECONOMY HELPERS - NEW)
    // =============================================================
    /** 
     * @brief Timer-based function to earn coins over time.
     */
    void EarnCoins();

    /** 
     * @brief Timer handle for coin earning.
     */
    FTimerHandle CoinTimerHandle;
};