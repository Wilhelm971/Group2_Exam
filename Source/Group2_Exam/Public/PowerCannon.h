/**
 * @file PowerCannon.h
 * @brief Header file for the APowerCannon class, which represents a cannon node in the power network that attacks enemies when powered.
 */

#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "PowerCannon.generated.h"

class UStaticMesh;
class UMaterialInterface;
class APowerLine;

/**
 * @class APowerCannon
 * @brief Cannon node that attacks enemies when powered.
 * 
 * Extends APowerNode with firing logic and building preview features.
 * This class manages the cannon's power reception, firing mechanism, visual materials for placement previews,
 * and interactions with enemies within its attack range.
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
     * @brief Default constructor for APowerCannon.
     * 
     * Initializes the cannon with default properties.
     */
    APowerCannon();

protected:
    /**
     * @brief Called when the game starts or when spawned.
     * 
     * Sets up initial state for the cannon.
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief Called after all components have been initialized.
     * 
     * Handles post-initialization setup, such as component attachments or material assignments.
     */
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER OVERRIDES
    // =============================================================
    /** 
     * @brief Receives power from another node and starts the firing timer.
     * 
     * Overrides the base class to activate the cannon's attack behavior when powered.
     * @param FromNode The node providing the power.
     */
    virtual void ReceivePower(APowerNode* FromNode) override;

    /** 
     * @brief Loses power and stops the firing timer.
     * 
     * Overrides the base class to deactivate the cannon's attack behavior.
     */
    virtual void LosePower() override;

    // =============================================================
    // CANNON PROPERTIES
    // =============================================================
    /** 
     * @brief Interval between shots when the cannon is powered.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float FireInterval = 2.0f;

    /** 
     * @brief Range for detecting and attacking enemies.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float AttackRange = 500.0f;

    /** 
     * @brief Damage dealt per shot to enemies.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float Damage = 30.0f;

    /**
     * @brief Class of the power line used for shots.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TSubclassOf<APowerLine> ShotLineClass;

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    /** 
     * @brief Static mesh component for the cannon's visual representation.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CannonMesh;

    /** 
     * @brief Material applied when the cannon is in its normal (placed) state.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* NormalMaterial;

    /** 
     * @brief Material applied during valid preview placement.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* PreviewValidMaterial;

    /** 
     * @brief Material applied during invalid preview placement.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* PreviewInvalidMaterial;

    // =============================================================
    // BUILDING PROPERTIES
    // =============================================================
    /** 
     * @brief Flag indicating if the cannon is in preview mode during placement.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bIsPreviewMode = false;

    /** 
     * @brief Flag indicating if the current placement position is valid.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bPlacementValid = true;

    /** 
     * @brief Minimum distance to other nodes required for valid placement.
     * 
     * Editable in the editor and blueprints, clamped to a minimum of 50.0f.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building", meta = (ClampMin = "50.0"))
    float MinPlacementDistance = 200.0f;

    // =============================================================
    // BUILDING FUNCTIONS
    // =============================================================
    /** 
     * @brief Sets the cannon to preview mode for placement.
     * 
     * @param bPreview True to enable preview mode, false to disable.
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetPreviewMode(bool bPreview);

    /** 
     * @brief Checks if the current position is valid for placement.
     * 
     * Updates the bPlacementValid flag based on distance and other criteria.
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void CheckPlacementValidity();

    /** 
     * @brief Updates the visual appearance based on placement validity.
     * 
     * Applies the appropriate preview material.
     */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void UpdatePreviewVisuals();

   
    
protected:


    /**
     * @brief Widget component for displaying the health bar UI.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UWidgetComponent* HealthBarComponent;
private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    /** 
     * @brief Attempts to shoot at the nearest enemy within range.
     * 
     * Called periodically when the cannon is powered.
     */
    void TryShoot();

    /** 
     * @brief Fires a shot at the specified enemy target.
     * 
     * Handles damage application and visual effects.
     * @param Target The enemy actor to fire at.
     */
    void FireAtEnemy(AActor* Target);

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    /** 
     * @brief Timer handle for managing the firing interval.
     */
    FTimerHandle TimerHandle_Fire;
};