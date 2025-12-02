#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharacter.h"
#include "EnemyPawn.h"
#include "GridManager.h"
#include "GameManager.generated.h"

/**
 * AGameManager
 * 
 * Manages enemy waves: spawning, timing, difficulty progression.
 * Place one in the level and configure spawn points/enemy class.
 * Automatically generates spawn points from GridManager if none set.
 */
UCLASS(Blueprintable, BlueprintType)
class GROUP2_EXAM_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// =============================================================
	// SPAWNING PROPERTIES
	// =============================================================
	/** Transform locations to spawn enemies from. Auto-generated if empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<FTransform> SpawnTransforms;

	/** Enemy class to spawn. */
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
	/** Delay between spawning each enemy in a wave. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float TimeBetweenEnemiesInWave = 1.5f;

	/** Delay after a wave ends (all dead) before next wave starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float TimeBetweenWaves = 20.0f;

	// =============================================================
	// RUNTIME INFO (Blueprint readable)
	// =============================================================
	/** Current wave number. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	int32 CurrentWaveNum = 0;

	/** Enemies alive from current wave. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
	int32 ActiveEnemiesCount = 0;

	// =============================================================
	// PUBLIC FUNCTIONS
	// =============================================================
	UFUNCTION(BlueprintCallable, Category = "Waves")
	void StartNewWave();

	UFUNCTION(BlueprintCallable, Category = "Waves")
	int32 GetEnemiesToSpawnThisWave() const { return EnemiesToSpawnThisWave; }

	
	void OnEnemyDeath(AEnemyCharacter* Enemy);

private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	void SpawnNextEnemy();


	void AutoGenerateSpawnPoints();

	// =============================================================
	// PRIVATE DATA
	// =============================================================
	int32 EnemiesToSpawnThisWave = 0;
	TArray<AEnemyCharacter*> ActiveEnemies;
	bool bSpawningCurrentWave = false;

	FTimerHandle EnemySpawnTimerHandle;
	FTimerHandle NextWaveTimerHandle;
};