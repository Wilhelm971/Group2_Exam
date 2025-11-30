#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerNode.generated.h"

class APowerNode;  // Forward declaration for ReceivePower parameter

/**
 * APowerNode
 * 
 * Base class for nodes in the power network.
 * Manages power state, health, damage, and registration with PowerNetworkSubsystem.
 * 
 * @note All derived nodes (e.g., cores, cannons) register on BeginPlay and unregister on EndPlay.
 * @see UPowerNetworkSubsystem for graph management.
 */
UCLASS()
class GROUP2_EXAM_API APowerNode : public AActor
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    
    /**
     * Default constructor. Initializes health.
     */
    APowerNode();

protected:
    /**
     * Called when the game starts: Registers with network subsystem.
     */
    virtual void BeginPlay() override;
    
    /**
     * Called on destruction or game end: Unregisters from subsystem.
     * @param EndPlayReason Reason for ending (e.g., destruction).
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // =============================================================
    // POWER PROPERTIES
    // =============================================================
    
    /** Range within which this node can transmit power to others (in units). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float PowerRange = 500.0f;

    /** Flag indicating if this node is currently powered. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    bool bIsPowered = false;

    /** Current power level in the node. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float CurrentPower = 0.0f;

    // =============================================================
    // POWER FUNCTIONS
    // =============================================================
    
    /**
     * Called when this node receives power from another.
     * Sets powered state and logs change.
     * @param FromNode The source node transmitting power.
     */
    virtual void ReceivePower(APowerNode* FromNode);

    /**
     * Receives a power amount from the network.
     * Accumulates CurrentPower; overridable for custom behavior.
     * @param Amount Power amount to add (positive).
     */
    UFUNCTION(BlueprintCallable, Category = "Power")
    virtual void ReceivePowerAmount(float Amount);

    /**
     * Called when this node loses power. Resets powered state.
     */
    virtual void LosePower();

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    
    /** Maximum health of the node. Configurable in editor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

    /** Current health of the node. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** Flag if the node is destroyed (health <=0). Read-only in Blueprints. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    bool bIsDestroyed = false;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    
    /**
     * Applies damage to the node; handles destruction or conversion (e.g., core to dormant).
     * @param DamageAmount Amount of damage to apply (positive).
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    virtual void TakeDamageCustom(float DamageAmount);
};