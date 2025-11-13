#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ANodeActor;

USTRUCT(BlueprintType)
struct FNode
{
	GENERATED_BODY()

	UPROPERTY()
	ANodeActor* NodeActor = nullptr;

	FIntPoint Index = FIntPoint(-1, -1);
	bool bIsWalkable = true;
	float G = 0.f;
	float H = 0.f;
	float F = 0.f;
	TObjectPtr<ANodeActor> Parent = nullptr;

	FNode() = default;
	FNode(ANodeActor* InActor, const FIntPoint& InIdx, bool bWalkable)
		: NodeActor(InActor), Index(InIdx), bIsWalkable(bWalkable) {}
};

UCLASS()
class GROUP2_EXAM_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

protected:
	virtual void BeginPlay() override;

public:
	// ==================== GRID SETTINGS ====================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	TSubclassOf<ANodeActor> NodeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector GridOrigin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridSizeX = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridSizeY = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float CellSize = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	bool bDrawDebugGrid = true;

	// ==================== RUNTIME DATA ====================
	UPROPERTY()
	TArray<ANodeActor*> Grid;  // Flat array: [X + Y * GridSizeX]

	// ==================== PUBLIC API ====================
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	TArray<FIntPoint> FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	FIntPoint WorldToGridIndex(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	FVector GridToWorldCenter(const FIntPoint& GridIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SpawnGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void ClearGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void MarkObstacles();

private:
	// A* helpers
	float Heuristic(const FIntPoint& A, const FIntPoint& B) const;
	TArray<FIntPoint> ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const;
	bool IsValidIndex(const FIntPoint& Idx) const;
	TArray<FIntPoint> GetNeighbors(const FIntPoint& Idx) const;
};