#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGameManager::AGameManager()
{
    PrimaryActorTick.bCanEverTick = true;
}
/*
void AGameManager::BeginPlay()
{
    Super::BeginPlay();
    StartNextWave();
}

void AGameManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bWaveActive) return;

    SpawnCountdown -= DeltaTime;
    if (SpawnCountdown <= 0.0f && RemainingEnemiesInWave > 0)
    {
        SpawnEnemy();
        RemainingEnemiesInWave--;
        SpawnCountdown = CurrentWave.SpawnDelaySeconds;
    }

    if (RemainingEnemiesInWave <= 0)
    {
        WaveEndCountdown += DeltaTime;
        if (WaveEndCountdown >= CurrentWave.WaveDelaySeconds)
        {
            CurrentWaveIndex++;
            StartNextWave();
        }
    }
}

void AGameManager::StartNextWave()
{
    if (CurrentWaveIndex >= WaveConfigs.Num())
    {
        // Endless mode? Restart waves or win condition handled elsewhere
        CurrentWaveIndex = 0;
    }

    CurrentWave = WaveConfigs[CurrentWaveIndex];
    RemainingEnemiesInWave = CurrentWave.NumEnemies;
    SpawnCountdown = CurrentWave.SpawnDelaySeconds;
    WaveEndCountdown = 0.0f;
    bWaveActive = true;

    UE_LOG(LogTemp, Log, TEXT("Started Wave %d: %d enemies"), CurrentWaveIndex, RemainingEnemiesInWave);
}

void AGameManager::SpawnEnemy()
{
    if (SpawnPoints.Num() == 0) return;

    int32 RandIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
    FVector SpawnLoc = SpawnPoints[RandIndex];
    SpawnLoc.Z += 1000.0f;

    FHitResult Hit;
    FVector TraceEnd = SpawnLoc - FVector(0.0f, 0.0f, 2000.0f);
    FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(50.0f));

    bool bHit = GetWorld()->SweepSingleByChannel(Hit, SpawnLoc, TraceEnd, FQuat::Identity, ECC_WorldStatic, BoxShape);
    if (bHit)
    {
        SpawnLoc = Hit.Location;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (CurrentWave.EnemyClass)
    {
        AEnemyPawn* NewEnemy = GetWorld()->SpawnActor<AEnemyPawn>(CurrentWave.EnemyClass, SpawnLoc, FRotator::ZeroRotator, Params);
        if (NewEnemy)
        {
            NewEnemy->MaxHealth *= CurrentWave.HealthMultiplier;
            NewEnemy->CurrentHealth = NewEnemy->MaxHealth;
            NewEnemy->MoveSpeed *= CurrentWave.SpeedMultiplier;
            UE_LOG(LogTemp, Log, TEXT("Spawned enemy for wave %d"), CurrentWaveIndex);
        }
    }
}

*/