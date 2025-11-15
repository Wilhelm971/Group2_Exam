// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "PowerCannon.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * APowerCannon
 * 
 * Cannon node that attacks enemies when powered.
 * Extends APowerNode with firing logic and building preview features.
 */
UCLASS()
class GROUP2_EXAM_API APowerCannon : public APowerNode
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    APowerCannon();

protected:
    virtual void BeginPlay() override;

public:
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER OVERRIDES
    // =============================================================
    /** Receives power and starts firing timer. */
    virtual void ReceivePower(APowerNode* FromNode) override;

    /** Loses power and stops firing. */
    virtual void LosePower() override;

    // =============================================================
    // CANNON PROPERTIES
    // =============================================================
    /** Interval between shots when powered. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float FireInterval = 2.0f;

    /** Range for detecting and attacking enemies. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float AttackRange = 350.0f;

    /** Damage dealt per shot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
    float Damage = 10.0f;

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
    /** Flag for preview mode during placement. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bIsPreviewMode = false;

    /** Flag indicating if current placement is valid. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
    bool bPlacementValid = true;

    /** Minimum distance to other nodes for valid placement. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building", meta = (ClampMin = "50.0"))
    float MinPlacementDistance = 250.0f;

    // =============================================================
    // BUILDING FUNCTIONS
    // =============================================================
    /** Sets the cannon to preview mode. */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetPreviewMode(bool bPreview);

    /** Checks if current position is valid for placement. */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void CheckPlacementValidity();

    /** Updates visuals based on placement validity. */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void UpdatePreviewVisuals();

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    /** Mesh component for the cannon. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CannonMesh;

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    /** Attempts to shoot at the nearest enemy. */
    void TryShoot();

    /** Fires a shot at the specified target. */
    void FireAtEnemy(AActor* Target);

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    /** Timer handle for firing interval. */
    FTimerHandle TimerHandle_Fire;
};