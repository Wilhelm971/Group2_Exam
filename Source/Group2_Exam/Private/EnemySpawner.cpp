


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

	FVector SpawnLoc = GetActorLocation();
	SpawnLoc += FVector(
		FMath::RandRange(-300.0f, 300.0f),
		FMath::RandRange(-300.0f, 300.0f),
		50.0f  // Slight height offset
	);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLoc, GetActorRotation(), Params);
	if (NewEnemy)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s"), *NewEnemy->GetName());
	}
}



