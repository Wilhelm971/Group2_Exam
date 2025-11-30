#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "PowerCannon.generated.h"

class UStaticMesh;
class UMaterialInterface;
class APowerLine;

/**
 * APowerCannon
 * 
 * Cannon node that attacks enemies when powered.
 * Extends APowerNode with firing logic and building preview features.
 * 
 * @note Fires at intervals if powered and enemies in range.
 * @see APowerLine for shot visuals.
 */
UCLASS()
class GROUP2_EXAM_API APowerCannon : public APowerNode
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    
    /**
     * Default constructor. Sets up mesh.
     */
    APowerCannon();

protected:
    /**
     * Called when the game starts: Starts firing timer.
     */
    virtual void BeginPlay() override;

public:
    /**
     * Applies default mesh after initialization.
     */
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER OVERRIDES
    // =============================================================
    
    /**
     * Receives power and starts firing timer.
     * @param FromNode Source node.
     */
    virtual void ReceivePower(APowerNode* FromNode) override;

    /**
     * Loses power and stops firing.
     */
    virtual void LosePower() override;

    // =============================================================
    // CANNON PROPERTIES
    // =============================================================
    
    /** Interval between shots when powered (in seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float FireInterval = 2.0f;

    /** Range for detecting and attacking enemies (in units). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float AttackRange = 350.0f;

    /** Damage dealt per shot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float Damage = 10.0f;

    /** Class for spawning shot visualization lines. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSubclassOf<APowerLine> ShotLineClass;

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    
    /** Static mesh asset for the cannon. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* CannonStaticMeshAsset;

    /** Material for normal (placed) state. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* NormalMaterial;

    /** Material for valid preview placement. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* PreviewValidMaterial;

    /** Material for invalid preview placement. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* PreviewInvalidMaterial;

    // =============================================================
    // BUILDING PROPERTIES
    // =============================================================
    
    /** Flag for preview mode during placement. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bIsPreviewMode = false;

    /** Flag indicating if current placement is valid. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bPlacementValid = true;

    /** Minimum distance to other nodes for valid placement (in units). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building", meta = (ClampMin = "50.0"))
    float MinPlacementDistance = 250.0f;

    // =============================================================
    // BUILDING FUNCTIONS
    // =============================================================
    
    /**
     * Sets the cannon to preview mode (disables collision, applies materials).
     * @param bPreview True for preview.
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetPreviewMode(bool bPreview);

    /**
     * Checks if current position is valid for placement (distance to nodes).
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void CheckPlacementValidity();

    /**
     * Updates visuals based on placement validity (materials).
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void UpdatePreviewVisuals();

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    
    /** Mesh component for the cannon. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CannonMesh;

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    
    /**
     * Attempts to shoot at the nearest enemy if powered.
     */
    void TryShoot();

    /**
     * Fires a shot at the specified target.
     * @param Target Enemy to attack.
     */
    void FireAtEnemy(AActor* Target);

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    
    /** Timer handle for firing interval. */
    FTimerHandle TimerHandle_Fire;
};