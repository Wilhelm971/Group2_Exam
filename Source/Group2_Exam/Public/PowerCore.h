/**
 * @file PowerCore.h
 * @brief Header file for the APowerCore class, which represents the central power source in the network.
 */

#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "PowerCore.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * @class APowerCore
 * @brief Central power source node that periodically emits power pulses.
 * 
 * Extends APowerNode with pulsing behavior to distribute power throughout the network.
 */
UCLASS()
class GROUP2_EXAM_API APowerCore : public APowerNode
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR AND OVERRIDES
	// =============================================================
	/**
	 * @brief Default constructor for APowerCore.
	 * 
	 * Initializes the core with default properties.
	 */
	APowerCore();

protected:
	/**
	 * @brief Called when the game starts or when spawned.
	 * 
	 * Sets up the pulsing timer or initial state.
	 */
	virtual void BeginPlay() override;

public:
    /**
     * @brief Called after all components have been initialized.
     * 
     * Handles post-initialization setup for the core.
     */
    virtual void PostInitializeComponents() override;

	// =============================================================
	// POWER PROPERTIES
	// =============================================================
	/** 
	 * @brief Interval for emitting power pulses.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float PulseInterval = 5.0f;

	// =============================================================
	// POWER FUNCTIONS
	// =============================================================
	/** 
	 * @brief Emits a power pulse to the network. (Deprecated)
	 * 
	 * This function is marked as deprecated and may be removed in future versions.
	 */
	//void EmitPowerPulse();

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    /** 
     * @brief Static mesh asset for the core's visual representation.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* CoreStaticMeshAsset;

    /** 
     * @brief Material applied to the core.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* CoreMaterial;

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    /** 
     * @brief Mesh component for rendering the core.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CoreMesh;

private:
	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** 
	 * @brief Timer handle for managing the pulse interval. (Commented out)
	 */
	//FTimerHandle TimerHandle_Pulse;
};