

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ANodeActor;

UCLASS()
class GROUP2_EXAM_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(CallInEditor, Category = "Pathfinding")
	void SpawnGrid();

	UFUNCTION(CallInEditor, Category = "Pathfinding")
	void StartPathfinding();

	UFUNCTION(CallInEditor, Category = "Pathfinding")
	void ClearGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	TSubclassOf<ANodeActor> NodeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
	float StepDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSizeY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float NodeSpacing = 100.0f;

	//SpawnLocation of the ENEMY
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FIntPoint StartCoords = FIntPoint(0, 0);

	//Add for more locations on the way?
	
	//The Location of the HQ
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FIntPoint EndCoords = FIntPoint(9, 9);

	UPROPERTY()
	TArray<ANodeActor*> Grid;

private:
	UPROPERTY()
	TArray<ANodeActor*> OpenSet;

	UPROPERTY()
	TArray<ANodeActor*> ClosedSet;

	UPROPERTY()
	ANodeActor* StartNode = nullptr;

	UPROPERTY()
	ANodeActor* EndNode = nullptr;

	FTimerHandle StepTimerHandle;
	bool bPathfindingInProgress = false;

	ANodeActor* GetNode(int32 X, int32 Y);
	TArray<ANodeActor*> GetNeighbors(ANodeActor* Node);
	float GetHeuristic(ANodeActor* A, ANodeActor* B);
	TArray<ANodeActor*> ReconstructPath(ANodeActor* EndNode);
	void ResetNodes();
	void StepAStar();
	void HandleNodeExpansion(ANodeActor* CurrentNode);
};
