/**
 * @file PowerNode.h
 * @brief Header file for the APowerNode class, base for nodes in the power network.
 */

#pragma once

#include "CoreMinimal.h"
#include "W_HealthBar.h"
#include "GameFramework/Actor.h"
//#include "EnemyCharacter.h"
#include "PowerNode.generated.h"

class AEnemyCharacter;

/**
 * @class APowerNode
 * @brief Base class for nodes in the power network.
 * 
 * Manages power state, health, and damage. Serves as a foundation for powered buildings.
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
     * @brief Default constructor for APowerNode.
     * 
     * Initializes default properties.
     */
    APowerNode();

protected:
    /**
     * @brief Called when the game starts or when spawned.
     * 
     * Sets up health and other runtime elements.
     */
    virtual void BeginPlay() override;
    
    /**
     * @brief Called when the actor ends play.
     * 
     * Cleans up resources.
     * @param EndPlayReason The reason for ending play.
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // =============================================================
    // POWER PROPERTIES
    // =============================================================
    /** 
     * @brief Range within which this node can transmit power to others.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
    float PowerRange = 250.0f;

    /** 
     * @brief Flag indicating if this node is currently powered.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    bool bIsPowered = false;

    /** 
     * @brief Current power level in the node.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
    float CurrentPower = 0.0f;

    // =============================================================
    // POWER FUNCTIONS
    // =============================================================
    /** 
     * @brief Called when this node receives power from another.
     * 
     * Virtual for subclass overriding.
     * @param FromNode The source node.
     */
    virtual void ReceivePower(APowerNode* FromNode);

    /** 
     * @brief Receives a power amount from the network.
     * 
     * @param Amount The amount of power received.
     */
    UFUNCTION(BlueprintCallable, Category = "Power")
    virtual void ReceivePowerAmount(float Amount);

    /** 
     * @brief Called when this node loses power.
     * 
     * Virtual for subclass overriding.
     */
    virtual void LosePower();

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    /** 
     * @brief Maximum health of the node.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 120.0f;

    /** 
     * @brief Current health of the node.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** 
     * @brief Flag indicating if the node has been placed.
     * 
     * Editable in the editor.
     */
    UPROPERTY(EditAnywhere)
    bool bIsPlaced = false;




    // Enemy Character reference
    /** 
     * @brief Reference to an enemy character (purpose unclear, possibly for targeting).
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    AEnemyCharacter* EnemyCharacter;

    /** 
     * @brief Flag indicating if the node is destroyed.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
    bool bIsDestroyed = false;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    /** 
     * @brief Applies custom damage to the node.
     * 
     * Reduces health and handles destruction if health reaches zero.
     * @param DamageAmount The amount of damage to apply.
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    virtual void TakeDamageCustom(float DamageAmount);

    // Health Bar
    /** 
     * @brief Instance of the health bar widget.
     */
    UPROPERTY()
    UW_HealthBar* HealthBar;

    /**
     * @brief Updates the health bar display based on current health.
     */
    UFUNCTION(BlueprintCallable, Category = "Cannon")
    virtual void UpdateHealthBar();
};