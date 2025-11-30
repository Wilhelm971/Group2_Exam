#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class APowerNode;  // Forward declaration (replaces include)
class AGridManager;  // Forward declaration (replaces include)

/**
 * AEnemyCharacter
 * 
 * Base class for enemy AI characters in the tower defense game.
 * Handles movement along grid-based paths, targeting the closest power node,
 * attacking when in range, and taking damage. Uses A* pathfinding via GridManager.
 * 
 * @note Enemies reward bounty on death and notify GameManager for wave tracking.
 * @see AGridManager for pathfinding details.
 * @see APowerNode for target interactions.
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
     * Default constructor. Sets up initial stats and movement properties.
     */
    AEnemyCharacter();

protected:
    /**
     * Called when the game starts or when the enemy is spawned.
     * Finds the GridManager and initial target power node.
     */
    virtual void BeginPlay() override;

public:
    /**
     * Called every frame to update movement, path recalculation, and attacks.
     * @param DeltaTime Time since last frame (in seconds).
     */
    virtual void Tick(float DeltaTime) override;

    // =============================================================
    // STATS PROPERTIES
    // =============================================================
    
    /** Maximum health of the enemy. Configurable in editor or Blueprints. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    /** Current health of the enemy. Read-only in Blueprints for monitoring. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    /** Movement speed of the enemy in units per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 300.f;

    /** Bounty rewarded to the player when this enemy is killed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Bounty = 20.0f;

    /** Reference to GameManager for wave tracking and enemy death notifications. */
    UPROPERTY()
    class AGameManager* GameManager;

    // =============================================================
    // DAMAGE FUNCTIONS
    // =============================================================
    
    /**
     * Applies damage to the enemy and handles death if health reaches zero.
     * @param DamageAmount Amount of damage to apply (must be positive).
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void TakeDamageCustom(float DamageAmount);

    /**
     * Applies damage specifically from a cannon attack.
     * Wrapper for TakeDamageCustom; use for cannon-specific effects if needed.
     * @param Damage Amount of damage from the cannon.
     */
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void TakeDamageFromCannon(float Damage);

    // =============================================================
    // AI PROPERTIES
    // =============================================================
    
    /** Current target power node to move toward and attack. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    APowerNode* TargetTower;

    /** Range at which the enemy can attack the target (in units). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange = 150.f;

    /** Interval (in seconds) for recalculating the path to the target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float PathRecalcInterval = 1.f;

    /**
     * Searches for and sets the closest power node as the target.
     * Called periodically to handle target destruction or repositioning.
     */
    void FindClosestTarget();

    /**
     * Calculates an A* path to the current target using the GridManager.
     * Updates PathPoints array with world-space waypoints.
     */
    void CalculateGridPath();

    /** Flag to enable/disable pathfinding behavior. */
    UPROPERTY()
    bool bDoPathfinding = true;

private:
    // =============================================================
    // PRIVATE HELPERS
    // =============================================================

    /**
     * Retrieves the next waypoint from the current path.
     * Advances index if close to current point; falls back to direct target if path ends.
     * @return Next world-space position to move toward.
     */
    FVector GetNextPathPoint();

    /**
     * Attacks the current target if within range.
     * Applies continuous damage based on DeltaTime.
     */
    void AttackTarget();

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    
    /** Time since last target search (for periodic checks). */
    float TimeSinceLastSearch = 0.f;

    /** Time since last path recalculation (for optimization). */
    float TimeSincePathRecalc = 0.f;

    /** Fixed interval for searching new targets (in seconds). */
    static constexpr float SearchInterval = 0.5f;

    /** Array of world-space waypoints from A* pathfinding. */
    TArray<FVector> PathPoints;

    /** Current index in the PathPoints array. */
    int32 CurrentPathIndex = 0;

    /** Reference to the GridManager for pathfinding queries. */
    UPROPERTY()
    AGridManager* GridMgr;
};