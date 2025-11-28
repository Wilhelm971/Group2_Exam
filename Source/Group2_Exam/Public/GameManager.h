#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPawn.h"
#include "GameManager.generated.h"

USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumEnemies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDelaySeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveDelaySeconds = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyPawn> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 1.0f;
};

UCLASS()
class GROUP2_EXAM_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	AGameManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves")
	TArray<FWaveConfig> WaveConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
	int32 CurrentWaveIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FVector> SpawnPoints;

	UFUNCTION(BlueprintCallable, Category = "Waves")
	void StartNextWave();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waves")
	int32 RemainingEnemiesInWave = 0;

private:
	FWaveConfig CurrentWave;
	float SpawnCountdown = 0.0f;
	float WaveEndCountdown = 0.0f;
	bool bWaveActive = false;

	void SpawnEnemy();
};