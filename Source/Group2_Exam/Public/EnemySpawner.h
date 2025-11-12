

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


class AEnemyCharacter;


UCLASS()
class GROUP2_EXAM_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnTime = 2.0f;


private:
	FTimerHandle SpawnTimer;

	void SpawnEnemy();

	
};
