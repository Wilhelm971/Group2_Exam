#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyPawn;  // Forward declaration

/*
 * This is an old class, that isn't used anymore. We didn't have time to delete it, so just ignore it.
 */
UCLASS()
class GROUP2_EXAM_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR
	// =============================================================
	/** Sets default values for this actor's properties. */
	AEnemySpawner();

protected:
	// =============================================================
	// OVERRIDES
	// =============================================================
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	// =============================================================
	// SPAWNER PROPERTIES
	// =============================================================
	/** Class of the enemy to spawn. */
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AEnemyPawn> EnemyClass;

	/** Time interval between spawns. */
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnTime = 2.0f;

private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	/** Spawns a single enemy instance. */
	void SpawnEnemy();

	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** Timer handle for spawning interval. */
	FTimerHandle SpawnTimer;
};