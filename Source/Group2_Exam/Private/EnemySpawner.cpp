// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#include "EnemySpawner.h"
#include "EnemyPawn.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

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

    FVector SpawnLoc = GetActorLocation();
    SpawnLoc.X += FMath::RandRange(-300.0f, 300.0f);
    SpawnLoc.Y += FMath::RandRange(-300.0f, 300.0f);
    SpawnLoc.Z += 1000.0f;  // High start for trace.

    // Box size for sweep (match your enemy's box/mesh - e.g., half-extent 50u cube)
    FVector BoxExtent = FVector(50.0f, 50.0f, 50.0f);  // Dynamic? Get from default object if needed.

    // Sweep DOWN with BOX shape
    FHitResult Hit;
    FVector TraceEnd = SpawnLoc - FVector(0, 0, 2000.0f);
    FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent);
    bool bHit = GetWorld()->SweepSingleByChannel(
        Hit, SpawnLoc, TraceEnd, FQuat::Identity,
        ECC_WorldStatic, BoxShape);

    if (bHit && Hit.bBlockingHit)
    {
        SpawnLoc = Hit.Location;  // Exact center on ground
        UE_LOG(LogTemp, Log, TEXT("✅ Box HIT at Z=%.1f"), SpawnLoc.Z);
    }
    else
    {
        // Fallback: Spawner Z + half-height
        SpawnLoc.Z = GetActorLocation().Z + BoxExtent.Z;
        UE_LOG(LogTemp, Warning, TEXT("❌ Sweep MISS → Fallback Z=%.1f"), SpawnLoc.Z);
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AEnemyPawn* NewEnemy = GetWorld()->SpawnActor<AEnemyPawn>(EnemyClass, SpawnLoc, GetActorRotation(), Params);
    if (NewEnemy)
    {
        FVector FinalLoc = NewEnemy->GetActorLocation();
        UE_LOG(LogTemp, Log, TEXT("🎯 SPAWNED %s at X=%.0f Y=%.0f Z=%.0f"), 
               *NewEnemy->GetName(), FinalLoc.X, FinalLoc.Y, FinalLoc.Z);
    }
}


