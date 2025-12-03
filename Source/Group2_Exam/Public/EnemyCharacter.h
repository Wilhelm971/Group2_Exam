/**
 * @file EnemyCharacter.h
 * @brief Header file for the AEnemyCharacter class, representing enemy AI in the game.
 */

#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "Components/WidgetComponent.h"
#include "W_HealthBar.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"


class AGridManager;  // Forward declaration (replaces include)


/**
 * @class AEnemyCharacter
 * @brief Base class for enemy AI characters in the tower defense game.
 * 
 * Handles movement, targeting, attacking, and damage. Uses grid-based pathfinding to reach targets.
 */
UCLASS()
class GROUP2_EXAM_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    /**
     * @brief Default constructor for AEnemyCharacter.
     * 
     * Sets up default properties.
     */
    AEnemyCharacter();

protected:
    /**
     * @brief Called when the game starts or when spawned.
     * 
     * Initializes health, finds grid manager, and starts targeting.
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief Called every frame.
     * 
     * Handles movement along path, target searching, and attacking.
     * @param DeltaTime Time since last frame.
     */
    virtual void Tick(float DeltaTime) override;

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    /** 
     * @brief Maximum health of the enemy.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth;

    /** 
     * @brief Current health of the enemy.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** 
     * @brief Movement speed of the enemy.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 300.f;

    // Bounty for killing enemies
    /** 
     * @brief Bounty rewarded for killing this enemy.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Bounty = 20.0f;

    /** 
     * @brief Reference to GameManager for wave tracking.
     */
    UPROPERTY()
    class AGameManager* GameManager;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    /** 
     * @brief Applies custom damage to the enemy.
     * 
     * Reduces health and handles death if health reaches zero.
     * @param DamageAmount The amount of damage.
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void TakeDamageCustom(float DamageAmount);



    // =============================================================
    // AI PROPERTIES
    // =============================================================
    /** 
     * @brief Current target power node to attack.
     * 
     * Visible in the editor but read-only in blueprints.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APowerNode* TargetTower;

    /** 
     * @brief Range at which the enemy can attack the target.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange = 200.f;

    /** 
     * @brief Interval for recalculating the path to the target.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PathRecalcInterval = 1.f;

    /**
     * @brief Finds the closest power node as the new target.
     */
    void FindClosestTarget();

    /**
     * @brief Calculates A* path to the target using GridManager.
     */
    void CalculateGridPath();

    /** 
     * @brief Check bools for other actors
     */
    UPROPERTY()
    bool bDoPathfinding = true;

    /** 
     * @brief Flag if the target tower is destroyed.
     * 
     * Visible in the editor.
     */
    UPROPERTY(VisibleAnywhere)
    bool bIsTowerDestroyed = false;

    /**HealthBar */
    /** 
     * @brief Widget component for the health bar.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UWidgetComponent* HealthBarComponent;

    /** 
     * @brief Instance of the health bar widget.
     */
    UPROPERTY()
    UW_HealthBar* HealthBar;

    /**
     * @brief Updates the health bar based on current health.
     */
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void UpdateHealthBar();
private:
    // =============================================================
    // PRIVATE HELPERS
    // =============================================================

    /**
     * @brief Gets the next waypoint in the current path.
     * 
     * @return The world position of the next path point.
     */
    FVector GetNextPathPoint();

    /**
     * @brief Attacks the current target if in range.
     */
    void AttackTarget();

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    /** 
     * @brief Time since last target search.
     */
    float TimeSinceLastSearch = 0.f;

    /** 
     * @brief Time since last path recalculation.
     */
    float TimeSincePathRecalc = 0.f;

    /** 
     * @brief Interval for searching new targets.
     */
    static constexpr float SearchInterval = 0.5f;

    /** 
     * @brief Array of world-space path points from A*.
     */
    TArray<FVector> PathPoints;

    /** 
     * @brief Current index in the path points array.
     */
    int32 CurrentPathIndex = 0;

    /** 
     * @brief Reference to the grid manager for pathfinding.
     */
    UPROPERTY()
    AGridManager* GridMgr;
};