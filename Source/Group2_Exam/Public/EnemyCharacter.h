#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class APowerNode;  // Forward declaration (replaces include)
class AGridManager;  // Forward declaration (replaces include)
class APowerNode;

/**
 * AEnemyCharacter
 * 
 * Base class for enemy AI characters in the tower defense game.
 * Handles movement, targeting, attacking, and damage.
 */
UCLASS()
class GROUP2_EXAM_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    /** Maximum health of the enemy. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    /** Current health of the enemy. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** Movement speed of the enemy. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 300.f;

    // Bounty for killing enemies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Bounty = 20.0f;

    /** Reference to GameManager for wave tracking. */
    UPROPERTY()
    class AGameManager* GameManager;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    /** Applies custom damage to the enemy. */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void TakeDamageCustom(float DamageAmount);



    // =============================================================
    // AI PROPERTIES
    // =============================================================
    /** Current target power node to attack. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APowerNode* TargetTower;

    /** Range at which the enemy can attack the target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange = 150.f;

    /** Interval for recalculating the path to the target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PathRecalcInterval = 1.f;

    /** Finds the closest power node as the new target. */
    void FindClosestTarget();

    /** Calculates A* path to the target using GridManager. */
    void CalculateGridPath();

    UPROPERTY()
    bool bDoPathfinding = true;

private:
    // =============================================================
    // PRIVATE HELPERS
    // =============================================================

    /** Gets the next waypoint in the current path. */
    FVector GetNextPathPoint();

    /** Attacks the current target if in range. */
    void AttackTarget();

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    /** Time since last target search. */
    float TimeSinceLastSearch = 0.f;

    /** Time since last path recalculation. */
    float TimeSincePathRecalc = 0.f;

    /** Interval for searching new targets. */
    static constexpr float SearchInterval = 0.5f;

    /** Array of world-space path points from A*. */
    TArray<FVector> PathPoints;

    /** Current index in the path points array. */
    int32 CurrentPathIndex = 0;

    /** Reference to the grid manager for pathfinding. */
    UPROPERTY()
    AGridManager* GridMgr;
};