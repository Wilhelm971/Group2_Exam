/**
 * @file DormantPowerCores.h
 * @brief Header file for the ADormantPowerCores class, representing inactive power cores that activate upon power accumulation.
 */

#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "DormantPowerCores.generated.h"

class UStaticMesh;
class UMaterialInterface;

/**
 * @class ADormantPowerCores
 * @brief Dormant power core that accumulates power and activates into a PowerCore when threshold is reached.
 * 
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
    /**
     * @brief Default constructor for ADormantPowerCores.
     * 
     * Initializes default properties.
     */
    ADormantPowerCores();

protected:
    /**
     * @brief Called when the game starts or when spawned.
     * 
     * Sets up initial state.
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief Called after all components have been initialized.
     * 
     * Handles mesh and material setup.
     */
    virtual void PostInitializeComponents() override;

    // =============================================================
    // POWER OVERRIDES
    // =============================================================
    /** 
     * @brief Receives and accumulates power amount. Activates if threshold reached.
     * 
     * Overrides base to handle accumulation.
     * @param Amount The power amount received.
     */
    virtual void ReceivePowerAmount(float Amount) override;

    // =============================================================
    // PROPERTIES
    // =============================================================
    /** 
     * @brief Power required to activate into a PowerCore.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float ActivationThreshold = 750.0f;

    /** 
     * @brief Current accumulated power.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float AccumulatedPower = 0.0f;

    // =============================================================
    // MESH AND MATERIALS
    // =============================================================
    /** 
     * @brief Static mesh asset for the dormant core.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* DormantStaticMeshAsset;

    /** 
     * @brief Material for the dormant core.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* DormantMaterial;

protected:
    // =============================================================
    // COMPONENTS
    // =============================================================
    /** 
     * @brief Mesh component for the dormant core.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DormantMesh;

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    /**
     * @brief Activates this dormant core into a PowerCore and checks win condition.
     */
    void Activate();

    /**
     * @brief Checks if win condition is met (5 active PowerCores).
     */
    void CheckWinCondition();
};