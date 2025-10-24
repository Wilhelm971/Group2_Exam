/*

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class GROUP2_EXAM_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 MaxActive;

private:
	FTimerHandle SpawnTimer;
	void SpawnOne();
};
*/