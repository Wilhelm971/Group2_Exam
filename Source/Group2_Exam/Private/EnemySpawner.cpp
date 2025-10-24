/*
#include "EnemySpawner.h"
#include "Enemy.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SpawnInterval = 4.0f;
	MaxActive = 20;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnInterval > 0.f)
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnOne, SpawnInterval, true, 1.0f);
}

void AEnemySpawner::SpawnOne()
{
	if (!EnemyClass) return;
	// optional: limit active count
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Found);
	if (Found.Num() >= MaxActive) return;

	FActorSpawnParameters P;
	P.Owner = this;
	FVector Pos = GetActorLocation();
	GetWorld()->SpawnActor<AEnemy>(EnemyClass, Pos, FRotator::ZeroRotator, P);
}
*/