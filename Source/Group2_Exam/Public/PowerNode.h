#pragma once

#include "CoreMinimal.h"
#include "W_HealthBar.h"
#include "GameFramework/Actor.h"
#include "PowerNode.generated.h"

class APowerNode;  // Forward declaration for ReceivePower parameter

/**
 * APowerNode
 * 
 * Base class for nodes in the power network.
 * Manages power state, health, and damage.
 */
UCLASS()
class GROUP2_EXAM_API APowerNode : public AActor
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    APowerNode();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // =============================================================
    // POWER PROPERTIES
    // =============================================================
    /** Range within which this node can transmit power to others. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float PowerRange = 500.0f;

    /** Flag indicating if this node is currently powered. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    bool bIsPowered = false;

    /** Current power level in the node. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float CurrentPower = 0.0f;

    // =============================================================
    // POWER FUNCTIONS
    // =============================================================
    /** Called when this node receives power from another. */
    virtual void ReceivePower(APowerNode* FromNode);

    /** Receives a power amount from the network. */
    UFUNCTION(BlueprintCallable, Category = "Power")
    virtual void ReceivePowerAmount(float Amount);

    /** Called when this node loses power. */
    virtual void LosePower();

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    /** Maximum health of the node. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.0f;

    /** Current health of the node. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    // Enemy Character reference
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    bool bIsDestroyed = false;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    /** Applies custom damage to the node. */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    virtual void TakeDamageCustom(float DamageAmount);

    // Health Bar
    UPROPERTY()
    UW_HealthBar* HealthBar;

    UFUNCTION(BlueprintCallable, Category = "Cannon")
    virtual void UpdateHealthBar();
};