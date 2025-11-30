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
 * Central power source node that periodically emits power pulses to the network.
 * Extends APowerNode with pulsing behavior and higher health.
 * 
 * @note Pulses are handled globally via PowerNetworkSubsystem.
 * @see UPowerNetworkSubsystem for distribution logic.
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
     * Default constructor. Overrides base health for durability.
     */
    APowerCore();

protected:
    /**
     * Called when the game starts or when spawned. Starts pulse timer (commented; handled globally).
     */
    virtual void BeginPlay() override;

public:
    /**
     * Applies default mesh and material after components are initialized.
     */
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER PROPERTIES
    // =============================================================
    
    /** Interval for emitting power pulses (in seconds). Configurable in editor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float PulseInterval = 5.0f;

    // =============================================================
    // POWER FUNCTIONS
    // =============================================================
    
    // /** Emits a power pulse to the network. (Commented; use subsystem instead.) */
    //void EmitPowerPulse();

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    
    /** Static mesh asset for the core. Set in editor or constructor helpers. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* CoreStaticMeshAsset;

    /** Material for the core. Applied for visual representation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* CoreMaterial;

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    
    /** Mesh component for the core. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CoreMesh;

private:
    // =============================================================
    // PRIVATE DATA
    // =============================================================
    
    // /** Timer handle for pulse interval. (Commented; global in subsystem.) */
    //FTimerHandle TimerHandle_Pulse;
};