// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#include "EnemySpawner.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// AEnemySpawner: Actor responsible for spawning enemies at intervals.
// Enemies are spawned with random offsets and adjusted to ground level using traces.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values for the spawner.
AEnemySpawner::AEnemySpawner()
{
    // Disable ticking as spawning is timer-based.
    PrimaryActorTick.bCanEverTick = false;
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Called when the game starts or when the spawner is spawned.
void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    if (EnemyClass)
    {
        // Set up a repeating timer to spawn enemies.
        GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnTime, true);
    }
}

// =============================================================
// SPAWN ENEMY
// =============================================================
// Spawns a single enemy with random offset and ground adjustment.
void AEnemySpawner::SpawnEnemy()
{
    if (!EnemyClass) return;

    // Calculate random spawn location with offset.
    FVector SpawnLoc = GetActorLocation();
    SpawnLoc.X += FMath::RandRange(-300.0f, 300.0f);
    SpawnLoc.Y += FMath::RandRange(-300.0f, 300.0f);
    SpawnLoc.Z += 1000.0f;  // Start above ground to trace down.

    // Trace downward to find ground level.
    FHitResult Hit;
    bool bHit = GetWorld()->SweepSingleByChannel(
        Hit, SpawnLoc, SpawnLoc - FVector(0, 0, 2000.0f), FQuat::Identity,
        ECC_Visibility, FCollisionShape::MakeSphere(60.0f));  // Sphere size matches enemy.

    if (bHit && Hit.bBlockingHit)
    {
        SpawnLoc = Hit.Location;  // Set to exact ground hit location.
        UE_LOG(LogTemp, Log, TEXT("Ground hit at Z=%.1f"), SpawnLoc.Z);
    }

    // Spawn parameters with collision handling.
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Spawn the enemy.
    AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLoc, GetActorRotation(), Params);
    if (NewEnemy)
    {
        UE_LOG(LogTemp, Log, TEXT("✅ Spawned %s at %.1f Z (ground level!)"), *NewEnemy->GetName(), NewEnemy->GetActorLocation().Z);
    }
}


