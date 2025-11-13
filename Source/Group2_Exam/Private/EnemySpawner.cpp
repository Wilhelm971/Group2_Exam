


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

	// Random X/Y offset around spawner
	FVector SpawnLoc = GetActorLocation();
	SpawnLoc.X += FMath::RandRange(-300.0f, 300.0f);
	SpawnLoc.Y += FMath::RandRange(-300.0f, 300.0f);
	SpawnLoc.Z += 1000.0f;  // Start high

	// TRACE DOWN to ground
	FHitResult Hit;
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, SpawnLoc, SpawnLoc - FVector(0,0,2000.0f), FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(60.0f));  // Enemy size

	if (bHit && Hit.bBlockingHit)
	{
		SpawnLoc = Hit.Location;  // NO OFFSET - exact ground level
		UE_LOG(LogTemp, Log, TEXT("Ground hit at Z=%.1f"), SpawnLoc.Z);
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLoc, GetActorRotation(), Params);
	if (NewEnemy)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Spawned %s at %.1f Z (ground level!)"), *NewEnemy->GetName(), NewEnemy->GetActorLocation().Z);
	}
}



