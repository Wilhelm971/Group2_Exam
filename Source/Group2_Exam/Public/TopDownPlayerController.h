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
 * Manages camera movement, input handling via Enhanced Input, building/placement of towers,
 * and economy (coins over time). Possesses a camera pawn for view control.
 * 
 * @note Building mode allows preview and placement of cannons if affordable.
 * @see ATopDownCameraPawn for camera setup.
 * @see APowerCannon for placeable towers.
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
     * Default constructor. Sets initial mouse and input properties.
     */
    ATopDownPlayerController();

    /**
     * Called when the game starts. Sets up input mappings, caches pawn, and starts coin timer.
     */
    virtual void BeginPlay() override;
    
    /**
     * Sets up input bindings using Enhanced Input system.
     */
    virtual void SetupInputComponent() override;
    
    /**
     * Called every frame to update building preview if active.
     * @param DeltaSeconds Time since last frame (in seconds).
     */
    virtual void Tick(float DeltaSeconds) override;

    // =============================================================
    // BUILDING PROPERTIES
    // =============================================================
    
    /** Class of the cannon to spawn during building mode. Set in editor or Blueprints. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TSubclassOf<APowerCannon> CannonToPlaceClass;

    /** Collision channel used for placement traces (e.g., under cursor). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TEnumAsByte<ECollisionChannel> PlacementTraceChannel = ECC_Visibility;

    /** Preview instance of the cannon during building mode (transient). */
    UPROPERTY()
    APowerCannon* PreviewCannon;

    /** Flag indicating if building mode is active. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bBuildingModeActive = false;

    // =============================================================
    // ECONOMY PROPERTIES
    // =============================================================
    
    /** Current number of coins the player has. Read-only in Blueprints for UI. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Economy")
    float CurrentCoins = 0.0f;

    /** Cost to place a cannon. Configurable in editor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CannonCost = 50.0f;

    /** Coins earned per second. Upgradable via gameplay. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float CoinsPerSecond = 7.0f;

    // =============================================================
    // UI (truncated in provided doc; assuming HUD setup)
    // =============================================================
    // ... (truncated 1206 characters)... Reference to the controlled pawn (camera pawn).


    // =============================================================
    // UI
    // =============================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY()
    UUserWidget* HUDWidget;
    

    UFUNCTION(BlueprintCallable, Category = "HUD")
    int32 GetActiveCoreCount() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    float GetCoreHealthPercent() const;


    

    /** Reference to the controlled pawn for camera operations. */
    APawn* ControlledPawn;


        
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


    

    // =============================================================
    // INPUT PROPERTIES
    // =============================================================
    
    /** Input mapping context for enhanced input system. Set in defaults. */
    UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
    UInputMappingContext* InputMapping;

    /** Input action for camera movement (panning). */
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
    
    /**
     * Handles camera panning input from MoveAction.
     * @param Value Input value (vector for direction).
     */
    void HandleMove(const FInputActionValue& Value);

    /**
     * Handles camera zoom input from ZoomAction.
     * @param Value Input value (scalar for zoom amount).
     */
    void HandleZoom(const FInputActionValue& Value);

    /**
     * Toggles building mode on/off.
     * @param Value Trigger value (not used).
     */
    void ToggleBuildingMode(const FInputActionValue& Value);

    /**
     * Places the preview cannon if valid and affordable.
     * @param Value Trigger value (not used).
     */
    void PlaceBuilding(const FInputActionValue& Value);

    /**
     * Cancels building mode and destroys preview.
     * @param Value Trigger value (not used).
     */
    void CancelBuilding(const FInputActionValue& Value);

private:
    // =============================================================
    // PRIVATE FUNCTIONS (BUILDING HELPERS)
    // =============================================================
    
    /**
     * Initializes building mode and spawns preview cannon.
     */
    void StartBuildingMode();

    /**
     * Cleans up building mode and destroys preview cannon.
     */
    void CancelBuildingMode();

    /**
     * Updates the preview cannon's position based on cursor hit result.
     * Checks validity and updates visuals.
     */
    void UpdatePreviewPosition();

    // =============================================================
    // PRIVATE FUNCTIONS (ECONOMY HELPERS)
    // =============================================================
    
    /**
     * Timer-based function to earn coins over time.
     * Called every second; adds CoinsPerSecond to CurrentCoins.
     */
    void EarnCoins();

    /** Timer handle for coin earning loop. */
    FTimerHandle CoinTimerHandle;
};