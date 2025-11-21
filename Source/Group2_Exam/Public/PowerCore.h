// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "PowerCore.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * APowerCore
 * 
 * Central power source node that periodically emits power pulses.
 * Extends APowerNode with pulsing behavior.
 */
UCLASS()
class GROUP2_EXAM_API APowerCore : public APowerNode
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR AND OVERRIDES
	// =============================================================
	APowerCore();

protected:
	virtual void BeginPlay() override;

public:
    virtual void PostInitializeComponents() override;

	// =============================================================
	// POWER PROPERTIES
	// =============================================================
	/** Interval for emitting power pulses. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float PulseInterval = 5.0f;

	// =============================================================
	// POWER FUNCTIONS
	// =============================================================
	/** Emits a power pulse to the network. */
	void EmitPowerPulse();

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    /** Static mesh asset for the core. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* CoreStaticMeshAsset;

    /** Material for the core. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* CoreMaterial;

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    /** Mesh component for the core. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CoreMesh;

private:
	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** Timer handle for pulse interval. */
	FTimerHandle TimerHandle_Pulse;
};