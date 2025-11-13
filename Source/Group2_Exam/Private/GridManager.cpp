#include "GridManager.h"
#include "NodeActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnGrid();
	MarkObstacles();
}

// =============================================================
// SPAWN GRID
// =============================================================
void AGridManager::SpawnGrid()
{
	ClearGrid();

	if (!NodeClass) return;
	if (GridSizeX <= 0 || GridSizeY <= 0 || CellSize <= 0.f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	Grid.Reserve(GridSizeX * GridSizeY);

	for (int32 Y = 0; Y < GridSizeY; ++Y)
	{
		for (int32 X = 0; X < GridSizeX; ++X)
		{
			FVector SpawnLoc = GridOrigin + FVector(X * CellSize, Y * CellSize, 0.f);
			SpawnLoc += FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f); // center

			ANodeActor* Node = World->SpawnActor<ANodeActor>(NodeClass, SpawnLoc, FRotator::ZeroRotator);
			if (Node)
			{
				Node->GridIndex = FIntPoint(X, Y);
				Node->bIsWalkable = true;
				Grid.Add(Node);

				if (bDrawDebugGrid)
				{
					DrawDebugBox(World, SpawnLoc, FVector(CellSize * 0.5f), FColor::Green, true, -1.f, 0, 1.f);
				}
			}
		}
	}
}

// =============================================================
// CLEAR GRID
// =============================================================
void AGridManager::ClearGrid()
{
	for (ANodeActor* Node : Grid)
	{
		if (Node && Node->IsValidLowLevel())
		{
			Node->Destroy();
		}
	}
	Grid.Empty();
}

// =============================================================
// MARK OBSTACLES (overlap with static mesh actors)
// =============================================================
void AGridManager::MarkObstacles()
{
	if (Grid.Num() == 0) return;

	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> OverlapActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };

	for (int32 Idx = 0; Idx < Grid.Num(); ++Idx)
	{
		ANodeActor* Node = Grid[Idx];
		if (!Node) continue;

		FVector Center = Node->GetActorLocation();
		FBox Box = FBox::BuildAABB(Center, FVector(CellSize * 0.4f));

		OverlapActors.Empty();

		// FIXED: Pass empty array, not nullptr
		TArray<AActor*> ActorsToIgnore;
		UKismetSystemLibrary::BoxOverlapActors(
			World,
			Box.GetCenter(),
			Box.GetExtent(),
			ObjectTypes,
			AActor::StaticClass(),
			ActorsToIgnore,     // ← EMPTY ARRAY
			OverlapActors
		);

		bool bBlocked = false;
		for (AActor* Actor : OverlapActors)
		{
			if (Actor != this && !Actor->IsA(ANodeActor::StaticClass()))
			{
				bBlocked = true;
				break;
			}
		}

		Node->bIsWalkable = !bBlocked;

		if (bDrawDebugGrid)
		{
			FColor Color = Node->bIsWalkable ? FColor::Green : FColor::Red;
			DrawDebugBox(World, Center, FVector(CellSize * 0.5f), Color, true, -1.f, 0, 2.f);
		}
	}
}

// =============================================================
// WORLD to GRID
// =============================================================
FIntPoint AGridManager::WorldToGridIndex(const FVector& WorldLocation) const
{
	FVector Local = WorldLocation - GridOrigin;
	int32 X = FMath::FloorToInt(Local.X / CellSize);
	int32 Y = FMath::FloorToInt(Local.Y / CellSize);
	return FIntPoint(X, Y);
}

// =============================================================
// GRID to WORLD (center of cell)
// =============================================================
FVector AGridManager::GridToWorldCenter(const FIntPoint& GridIndex) const
{
	FVector World = GridOrigin + FVector(GridIndex.X * CellSize, GridIndex.Y * CellSize, 0.f);
	World += FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f);
	return World;
}

// =============================================================
// A* PATHFINDING (grid indices)
// =============================================================
TArray<FIntPoint> AGridManager::FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx)
{
	TArray<FIntPoint> EmptyPath;
	if (!IsValidIndex(StartIdx) || !IsValidIndex(EndIdx)) return EmptyPath;

	ANodeActor* StartNode = Grid[StartIdx.X + StartIdx.Y * GridSizeX];
	ANodeActor* EndNode   = Grid[EndIdx.X   + EndIdx.Y   * GridSizeX];

	if (!StartNode || !EndNode || !StartNode->bIsWalkable || !EndNode->bIsWalkable)
		return EmptyPath;

	// A* data
	TSet<FIntPoint> OpenSet;
	TSet<FIntPoint> ClosedSet;
	TMap<FIntPoint, FIntPoint> CameFrom;
	TMap<FIntPoint, float> GScore;
	TMap<FIntPoint, float> FScore;

	OpenSet.Add(StartIdx);
	GScore.Add(StartIdx, 0.f);
	FScore.Add(StartIdx, Heuristic(StartIdx, EndIdx));

	while (!OpenSet.IsEmpty())
	{
		// Find lowest F
		FIntPoint Current = OpenSet.Array()[0];
		for (const FIntPoint& Idx : OpenSet)
		{
			if (FScore.FindRef(Idx) < FScore.FindRef(Current))
				Current = Idx;
		}

		if (Current == EndIdx)
		{
			return ReconstructPath(CameFrom, Current);
		}

		OpenSet.Remove(Current);
		ClosedSet.Add(Current);

		for (const FIntPoint& Neighbor : GetNeighbors(Current))
		{
			if (ClosedSet.Contains(Neighbor)) continue;

			ANodeActor* NeighborNode = Grid[Neighbor.X + Neighbor.Y * GridSizeX];
			if (!NeighborNode || !NeighborNode->bIsWalkable) continue;

			float TentativeG = GScore.FindRef(Current) + 1.f; // uniform cost

			if (!OpenSet.Contains(Neighbor))
			{
				OpenSet.Add(Neighbor);
			}
			else if (TentativeG >= GScore.FindRef(Neighbor))
			{
				continue;
			}

			CameFrom.Add(Neighbor, Current);
			GScore.Add(Neighbor, TentativeG);
			FScore.Add(Neighbor, TentativeG + Heuristic(Neighbor, EndIdx));
		}
	}

	return EmptyPath; // no path
}

// =============================================================
// A* HELPERS
// =============================================================
float AGridManager::Heuristic(const FIntPoint& A, const FIntPoint& B) const
{
	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y); // Manhattan
}

TArray<FIntPoint> AGridManager::ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const
{
	TArray<FIntPoint> Path;
	Path.Add(Current);

	while (CameFrom.Contains(Current))
	{
		Current = CameFrom[Current];
		Path.Insert(Current, 0);
	}

	return Path;
}

bool AGridManager::IsValidIndex(const FIntPoint& Idx) const
{
	return Idx.X >= 0 && Idx.X < GridSizeX && Idx.Y >= 0 && Idx.Y < GridSizeY;
}

TArray<FIntPoint> AGridManager::GetNeighbors(const FIntPoint& Idx) const
{
	TArray<FIntPoint> Neighbors;
	const int32 DirsX[] = { -1, 1, 0, 0 };
	const int32 DirsY[] = { 0, 0, -2, 2 }; // 4-way

	for (int32 i = 0; i < 4; ++i)
	{
		FIntPoint N = FIntPoint(Idx.X + DirsX[i], Idx.Y + DirsY[i]);
		if (IsValidIndex(N))
			Neighbors.Add(N);
	}
	return Neighbors;
}