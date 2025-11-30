#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharacter.h"
#include "GridManager.h"
#include "GameManager.generated.h"

/**
 * AGameManager
 * 
 * Manages enemy waves: spawning, timing, difficulty progression.
 * Place one in the level and configure spawn points/enemy class.
 * Automatically generates spawn points from GridManager if none set.
 * 
 * @note Uses timers for spawning and wave delays; tracks active enemies.
 * @see AEnemyCharacter for spawned enemies.
 * @see AGridManager for auto-spawn points.
 */
UCLASS(Blueprintable, BlueprintType)
class GROUP2_EXAM_API AGameManager : public AActor
{
    GENERATED_BODY()

public:
    /**
     * Default constructor. Enables ticking.
     */
    AGameManager();

protected:
    /**
     * Called when the game starts: Auto-generates spawns and starts first wave.
     */
    virtual void BeginPlay() override;

public:
    /**
     * Called every frame to update active enemy count.
     * @param DeltaTime Time since last frame (in seconds).
     */
    virtual void Tick(float DeltaTime) override;

    // =============================================================
    // SPAWNING PROPERTIES
    // =============================================================
    
    /** Transform locations to spawn enemies from. Auto-generated if empty. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FTransform> SpawnTransforms;

    /** Enemy class to spawn. Set in editor or Blueprints. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<AEnemyCharacter> EnemyClass;

    // =============================================================
    // WAVE PROPERTIES
    // =============================================================
    
    /** Base number of enemies in wave 1. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
    int32 BaseEnemiesPerWave = 5;

    /** Additional enemies added each wave. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
    int32 EnemiesPerWaveIncrement = 1;

    // =============================================================
    // TIMING PROPERTIES
    // =============================================================
    
    /** Delay between spawning each enemy in a wave (in seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
    float TimeBetweenEnemiesInWave = 1.5f;

    /** Delay after a wave ends (all dead) before next wave starts (in seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
    float TimeBetweenWaves = 20.0f;

    // =============================================================
    // RUNTIME INFO (Blueprint readable)
    // =============================================================
    
    /** Current wave number. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
    int32 CurrentWaveNum = 0;

    /** Enemies alive from current wave. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
    int32 ActiveEnemiesCount = 0;

    // =============================================================
    // PUBLIC FUNCTIONS
    // =============================================================
    
    /**
     * Starts a new wave: Calculates enemies, sets spawning timer.
     */
    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartNewWave();

    /**
     * Gets the number of enemies to spawn in the current wave.
     * @return Enemies for this wave.
     */
    UFUNCTION(BlueprintCallable, Category = "Waves")
    int32 GetEnemiesToSpawnThisWave() const { return EnemiesToSpawnThisWave; }

    /**
     * Called on enemy death: Removes from active list and checks wave end.
     * @param Enemy The dead enemy.
     */
    void OnEnemyDeath(AEnemyCharacter* Enemy);

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    
    /**
     * Spawns the next enemy in the wave (timer callback).
     */
    void SpawnNextEnemy();

    /**
     * Auto-generates spawn points along the grid if none provided.
     */
    void AutoGenerateSpawnPoints();

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    
    /** Remaining enemies to spawn in current wave. */
    int32 EnemiesToSpawnThisWave = 0;
    
    /** List of active (alive) enemies. */
    TArray<AEnemyCharacter*> ActiveEnemies;
    
    /** Flag if currently spawning a wave. */
    bool bSpawningCurrentWave = false;

    /** Timer handle for enemy spawning in wave. */
    FTimerHandle EnemySpawnTimerHandle;
    
    /** Timer handle for delay to next wave. */
    FTimerHandle NextWaveTimerHandle;
};