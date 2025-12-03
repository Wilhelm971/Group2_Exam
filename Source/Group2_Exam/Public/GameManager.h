/**
 * @file GameManager.h
 * @brief Header file for the AGameManager class, which handles enemy wave spawning and management.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharacter.h"
#include "EnemyPawn.h"
#include "GridManager.h"
#include "GameManager.generated.h"

/**
 * @class AGameManager
 * @brief Manages enemy waves: spawning, timing, difficulty progression.
 * 
 * Place one in the level and configure spawn points/enemy class.
 * Automatically generates spawn points from GridManager if none set.
 */
UCLASS(Blueprintable, BlueprintType)
class GROUP2_EXAM_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor for AGameManager.
	 * 
	 * Sets up default properties.
	 */
	AGameManager();

protected:
	/**
	 * @brief Called when the game starts or when spawned.
	 * 
	 * Initializes wave system and auto-generates spawn points if needed.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief Called every frame.
	 * 
	 * Handles wave progression and enemy counting.
	 * @param DeltaTime Time since last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	// =============================================================
	// SPAWNING PROPERTIES
	// =============================================================
	/** 
	 * @brief Transform locations to spawn enemies from. Auto-generated if empty.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<FTransform> SpawnTransforms;

	/** 
	 * @brief Enemy class to spawn.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	// =============================================================
	// WAVE PROPERTIES
	// =============================================================
	/** 
	 * @brief Base number of enemies in wave 1.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	int32 BaseEnemiesPerWave = 2;

	/** 
	 * @brief Additional enemies added each wave.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	int32 EnemiesPerWaveIncrement = 1;

	// =============================================================
	// TIMING PROPERTIES
	// =============================================================
	/** 
	 * @brief Delay between spawning each enemy in a wave.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float TimeBetweenEnemiesInWave = 1.5f;

	/** 
	 * @brief Delay after a wave ends (all dead) before next wave starts.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float TimeBetweenWaves;

	// =============================================================
	// RUNTIME INFO (Blueprint readable)
	// =============================================================
	/** 
	 * @brief Current wave number.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	int32 CurrentWaveNum = 0;

	/** 
	 * @brief Enemies alive from current wave.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	int32 ActiveEnemiesCount = 0;

	// =============================================================
	// PUBLIC FUNCTIONS
	// =============================================================
	/**
	 * @brief Starts a new enemy wave.
	 */
	UFUNCTION(BlueprintCallable, Category = "Waves")
	void StartNewWave();

	/**
	 * @brief Gets the number of enemies to spawn in the current wave.
	 * 
	 * @return The calculated number of enemies for this wave.
	 */
	UFUNCTION(BlueprintCallable, Category = "Waves")
	int32 GetEnemiesToSpawnThisWave() const { return EnemiesToSpawnThisWave; }

	
	/**
	 * @brief Called when an enemy dies.
	 * 
	 * Updates active enemy count and checks for wave end.
	 * @param Enemy The enemy that died.
	 */
	void OnEnemyDeath(AEnemyCharacter* Enemy);

private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	/**
	 * @brief Spawns the next enemy in the wave.
	 */
	void SpawnNextEnemy();


	/**
	 * @brief Automatically generates spawn points using the GridManager.
	 */
	void AutoGenerateSpawnPoints();

	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** 
	 * @brief Number of enemies to spawn in the current wave.
	 */
	int32 EnemiesToSpawnThisWave = 0;
	
	/** 
	 * @brief List of currently active enemies.
	 */
	TArray<AEnemyCharacter*> ActiveEnemies;
	
	/** 
	 * @brief Flag indicating if the current wave is spawning.
	 */
	bool bSpawningCurrentWave = false;

	/** 
	 * @brief Timer handle for spawning enemies in the wave.
	 */
	FTimerHandle EnemySpawnTimerHandle;
	
	/** 
	 * @brief Timer handle for starting the next wave.
	 */
	FTimerHandle NextWaveTimerHandle;
};