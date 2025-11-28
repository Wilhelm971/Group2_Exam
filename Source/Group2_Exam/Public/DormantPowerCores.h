#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "DormantPowerCores.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * ADormantPowerCores
 * 
 * Dormant power core that accumulates power and activates into a PowerCore when threshold is reached.
 * Extends APowerNode to participate in the power network.
 */
UCLASS()
class GROUP2_EXAM_API ADormantPowerCores : public APowerNode
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    ADormantPowerCores();

protected:
    virtual void BeginPlay() override;

public:
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER OVERRIDES
    // =============================================================
    /** Receives and accumulates power amount. Activates if threshold reached. */
    virtual void ReceivePowerAmount(float Amount) override;

    // =============================================================
    // PROPERTIES
    // =============================================================
    /** Power required to activate into a PowerCore. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float ActivationThreshold = 500.0f;

    /** Current accumulated power. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float AccumulatedPower = 0.0f;

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    /** Static mesh asset for the dormant core. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* DormantStaticMeshAsset;

    /** Material for the dormant core. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* DormantMaterial;

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    /** Mesh component for the dormant core. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DormantMesh;

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    /** Activates this dormant core into a PowerCore and checks win condition. */
    void Activate();

    /** Checks if win condition is met (5 active PowerCores). */
    void CheckWinCondition();
};