#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

#include "Math/UnrealMathUtility.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

	TimeBetweenWaves = (TimeBetweenEnemiesInWave * EnemiesToSpawnThisWave) + 5.0f;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	AutoGenerateSpawnPoints();

	if (SpawnTransforms.Num() > 0 && EnemyClass)
	{
		GetWorldTimerManager().SetTimer(NextWaveTimerHandle, this, &AGameManager::StartNewWave, TimeBetweenWaves, true);
	}


	
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ActiveEnemiesCount = ActiveEnemies.Num();

	
	
	
}

void AGameManager::StartNewWave()
{
	CurrentWaveNum++;


	if (CurrentWaveNum == 7)
	{
		EnemiesPerWaveIncrement++;
	}
	
	EnemiesToSpawnThisWave = BaseEnemiesPerWave + (CurrentWaveNum - 1) * EnemiesPerWaveIncrement;
	bSpawningCurrentWave = true;
	ActiveEnemies.Empty();

	GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &AGameManager::SpawnNextEnemy, TimeBetweenEnemiesInWave, true);

	UE_LOG(LogTemp, Warning, TEXT("🌊 WAVE %d STARTED! Spawning %d enemies"), CurrentWaveNum, EnemiesToSpawnThisWave);
}

void AGameManager::SpawnNextEnemy()
{
	if (EnemiesToSpawnThisWave <= 0)
	{
		GetWorldTimerManager().ClearTimer(EnemySpawnTimerHandle);
		bSpawningCurrentWave = false;

		

		return;
	}

	if (SpawnTransforms.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawn transforms!"));
		return;
	}

	int32 SpawnIdx = FMath::RandRange(0, SpawnTransforms.Num() - 1);
	FTransform SpawnTransform = SpawnTransforms[SpawnIdx];

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnTransform);
	if (NewEnemy)
	{
		NewEnemy->SpawnDefaultController();
		NewEnemy->GameManager = this;
		ActiveEnemies.Add(NewEnemy);
		EnemiesToSpawnThisWave--;
	}
}

void AGameManager::OnEnemyDeath(AEnemyCharacter* Enemy)
{
	ActiveEnemies.RemoveSwap(Enemy);

	
}

void AGameManager::AutoGenerateSpawnPoints()
{
	if (SpawnTransforms.Num() > 0) return;

	TArray<AActor*> GridManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), GridManagers);

	if (GridManagers.Num() == 0) return;

	AGridManager* GridMgr = Cast<AGridManager>(GridManagers[0]);
	if (!GridMgr) return;

	int32 NumSpawnPoints = FMath::Clamp(GridMgr->GridSizeY / 6, 3, 8);
	for (int32 i = 0; i < NumSpawnPoints; ++i)
	{
		float YFraction = (float)i / (NumSpawnPoints - 1);
		int32 GridY = FMath::RoundToInt(YFraction * (GridMgr->GridSizeY - 1));
		
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