#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"

/**
 * AGameManager: Manages enemy waves, spawning, and progression.
 * Auto-generates spawns from grid if needed; uses timers for pacing.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
AGameManager::AGameManager()
{
    PrimaryActorTick.bCanEverTick = true;  // For updating active count.
}

// =============================================================
// BEGIN PLAY
// =============================================================
void AGameManager::BeginPlay()
{
    Super::BeginPlay();

    AutoGenerateSpawnPoints();  // Fallback if no manual spawns.

    // Start game if setup valid.
    if (SpawnTransforms.Num() > 0 && EnemyClass)
    {
        StartNewWave();
    }
}

// =============================================================
// TICK
// =============================================================
void AGameManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ActiveEnemiesCount = ActiveEnemies.Num();  // Update for UI/monitoring.
}

// =============================================================
// START NEW WAVE
// =============================================================
void AGameManager::StartNewWave()
{
    CurrentWaveNum++;  // Increment wave counter.

    // Calculate enemies: Linear progression.
    EnemiesToSpawnThisWave = BaseEnemiesPerWave + (CurrentWaveNum - 1) * EnemiesPerWaveIncrement;
    bSpawningCurrentWave = true;
    ActiveEnemies.Empty();  // Reset active list.

    // Set looping timer for spawning.
    GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &AGameManager::SpawnNextEnemy, TimeBetweenEnemiesInWave, true);

    UE_LOG(LogTemp, Warning, TEXT("🌊 WAVE %d STARTED! Spawning %d enemies"), CurrentWaveNum, EnemiesToSpawnThisWave);
}

// =============================================================
// SPAWN NEXT ENEMY
// =============================================================
void AGameManager::SpawnNextEnemy()
{
    // End spawning if done.
    if (EnemiesToSpawnThisWave <= 0)
    {
        GetWorldTimerManager().ClearTimer(EnemySpawnTimerHandle);
        bSpawningCurrentWave = false;

        // If all already dead (fast wave), start next.
        if (ActiveEnemies.Num() == 0)
        {
            GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this, &AGameManager::StartNewWave, TimeBetweenWaves, false);
        }

        return;
    }

    if (SpawnTransforms.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No spawn transforms!"));
        return;
    }

    // Random spawn point.
    int32 SpawnIdx = FMath::RandRange(0, SpawnTransforms.Num() - 1);
    FTransform SpawnTransform = SpawnTransforms[SpawnIdx];

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnTransform);
    if (NewEnemy)
    {
        NewEnemy->GameManager = this;  // Link for death notifications.
        ActiveEnemies.Add(NewEnemy);
        EnemiesToSpawnThisWave--;
    }
}

// =============================================================
// ON ENEMY DEATH
// =============================================================
void AGameManager::OnEnemyDeath(AEnemyCharacter* Enemy)
{
    ActiveEnemies.RemoveSwap(Enemy);  // Fast remove.

    // If wave complete and not spawning, start next after delay.
    if (ActiveEnemies.Num() == 0 && !bSpawningCurrentWave)
    {
        GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this, &AGameManager::StartNewWave, TimeBetweenWaves, false);
    }
}

// =============================================================
// AUTO GENERATE SPAWN POINTS
// =============================================================
void AGameManager::AutoGenerateSpawnPoints()
{
    if (SpawnTransforms.Num() > 0) return;  // Skip if manual.

    TArray<AActor*> GridManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), GridManagers);

    if (GridManagers.Num() == 0) return;

    AGridManager* GridMgr = Cast<AGridManager>(GridManagers[0]);
    if (!GridMgr) return;

    // Calculate points: Clamped between 3-8 along Y.
    int32 NumSpawnPoints = FMath::Clamp(GridMgr->GridSizeY / 6, 3, 8);
    for (int32 i = 0; i < NumSpawnPoints; ++i)
    {
        float YFraction = (float)i / (NumSpawnPoints - 1);
        int32 GridY = FMath::RoundToInt(YFraction * (GridMgr->GridSizeY - 1));
        
        // Center in first column, offset Z for spawn height.
        FVector SpawnLocation(
            GridMgr->GridOrigin.X + GridMgr->CellSize * 0.5f,  // Center of first column (X=0)
            GridMgr->GridOrigin.Y + GridY * GridMgr->CellSize + GridMgr->CellSize * 0.5f,
            GridMgr->GridOrigin.Z + 100.0f
        );

        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
        SpawnTransforms.Add(SpawnTransform);
    }

    UE_LOG(LogTemp, Log, TEXT("Auto-generated %d spawn points on the grid"), SpawnTransforms.Num());
}