


#include "EnemySpawner.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	if (EnemyClass)
	{
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnTime, true);
	}
	
}

void AEnemySpawner::SpawnEnemy()
{

	if (!EnemyClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, GetActorLocation(), GetActorRotation(), Params);
}



