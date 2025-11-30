#include "GridManager.h"
#include "NodeActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * AGridManager: Manages a 2D grid for pathfinding.
 * Spawns nodes, marks obstacles, and provides A* pathfinding.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;  // No updates after setup.
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Spawns the grid and marks obstacles.
void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    SpawnGrid();
    MarkObstacles();  // Detect static obstacles post-spawn.
}

// =============================================================
// SPAWN GRID
// =============================================================
// Creates and populates the grid with node actors.
void AGridManager::SpawnGrid()
{
    ClearGrid();  // Reset if respawning.

    if (!NodeClass) return;
    if (GridSizeX <= 0 || GridSizeY <= 0 || CellSize <= 0.f) return;

    UWorld* World = GetWorld();
    if (!World) return;

    Grid.Reserve(GridSizeX * GridSizeY);  // Pre-allocate for perf.

    for (int32 Y = 0; Y < GridSizeY; ++Y)
    {
        for (int32 X = 0; X < GridSizeX; ++X)
        {
            // Center spawn in cell for alignment.
            FVector SpawnLoc = GridOrigin + FVector(X * CellSize, Y * CellSize, 0.f);
            SpawnLoc += FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f);

            ANodeActor* Node = World->SpawnActor<ANodeActor>(NodeClass, SpawnLoc, FRotator::ZeroRotator);
            if (Node)
            {
                Node->GridIndex = FIntPoint(X, Y);
                Node->bIsWalkable = true;
                Grid.Add(Node);

                // Debug: Draw cell boxes if enabled.
                if (bDrawDebugGrid)
                {
                    DrawDebugBox(World, SpawnLoc, FVector(CellSize * 0.5f), FColor::Green, true);
                }
            }
        }
    }
}

// =============================================================
// CLEAR GRID
// =============================================================
// Destroys all nodes.
void AGridManager::ClearGrid()
{
    for (ANodeActor* Node : Grid)
    {
        if (Node) Node->Destroy();
    }
    Grid.Empty();
}

// =============================================================
// MARK OBSTACLES
// =============================================================
// Marks non-walkable based on overlaps.
void AGridManager::MarkObstacles()
{
    for (ANodeActor* Node : Grid)
    {
        if (!Node) continue;

        // Trace for obstacles (e.g., world static; adjust channel).
        FHitResult Hit;
        ECollisionChannel TraceChannel = ECC_WorldStatic;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Node->GetActorLocation(), Node->GetActorLocation() + FVector(0,0,-1000), TraceChannel, Params);
        if (bHit && Hit.GetActor() /* check if obstacle */ )
        {
            Node->bIsWalkable = false;
            Node->SetState(ENodeState::Blocked);
        }
    }
}

// =============================================================
// FIND PATH
// =============================================================
// A* pathfinding.
TArray<FIntPoint> AGridManager::FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx)
{
    if (!IsValidIndex(StartIdx) || !IsValidIndex(EndIdx)) return TArray<FIntPoint>();

    TArray<FIntPoint> EmptyPath;

    TSet<FIntPoint> OpenSet;
    TMap<FIntPoint, FIntPoint> CameFrom;
    TMap<FIntPoint, float> GScore;
    TMap<FIntPoint, float> FScore;

    // Init start.
    OpenSet.Add(StartIdx);
    GScore.Add(StartIdx, 0.f);
    FScore.Add(StartIdx, Heuristic(StartIdx, EndIdx));

    while (!OpenSet.IsEmpty())
    {
        // Find lowest F score (simple loop; use heap for large grids).
        FIntPoint Current;
        float MinF = FLT_MAX;
        for (FIntPoint P : OpenSet)
        {
            float F = FScore.FindRef(P);
            if (F < MinF)
            {
                MinF = F;
                Current = P;
            }
        }

        if (Current == EndIdx)
        {
            return ReconstructPath(CameFrom, Current);
        }

        OpenSet.Remove(Current);

        // Check neighbors.
        TArray<FIntPoint> Neighbors = GetNeighbors(Current);
        for (FIntPoint Neighbor : Neighbors)
        {
            if (!IsValidIndex(Neighbor)) continue;

            ANodeActor* NeighborNode = Grid[Neighbor.X + Neighbor.Y * GridSizeX];
            if (!NeighborNode || !NeighborNode->bIsWalkable) continue;

            // Uniform cost (1 per step).
            float TentativeG = GScore.FindRef(Current) + 1.f;

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

    return EmptyPath;  // No path.
}


// =============================================================
// CONVERSION HELPERS
// =============================================================
// Converts world location to grid index.
FIntPoint AGridManager::WorldToGridIndex(const FVector& WorldLocation) const
{
    FVector Local = WorldLocation - GridOrigin;
    int32 X = FMath::FloorToInt(Local.X / CellSize);
    int32 Y = FMath::FloorToInt(Local.Y / CellSize);
    return FIntPoint(X, Y);
}

// Converts grid index to world center position.
FVector AGridManager::GridToWorldCenter(const FIntPoint& GridIndex) const
{
    FVector World = GridOrigin + FVector(GridIndex.X * CellSize, GridIndex.Y * CellSize, 0.f);
    World += FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f);
    return World;
}



// =============================================================
// PATHFINDING HELPERS
// =============================================================
// Manhattan distance heuristic.
float AGridManager::Heuristic(const FIntPoint& A, const FIntPoint& B) const
{
    return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

// Reconstructs path from came-from map.
TArray<FIntPoint> AGridManager::ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const
{
    TArray<FIntPoint> Path;
    Path.Add(Current);

    while (CameFrom.Contains(Current))
    {
        Current = CameFrom[Current];
        Path.Insert(Current, 0);  // Build in reverse.
    }

    return Path;
}

// Checks if grid index is within bounds.
bool AGridManager::IsValidIndex(const FIntPoint& Idx) const
{
    return Idx.X >= 0 && Idx.X < GridSizeX && Idx.Y >= 0 && Idx.Y < GridSizeY;
}

// Gets 4-directional neighbors.
TArray<FIntPoint> AGridManager::GetNeighbors(const FIntPoint& Idx) const
{
    TArray<FIntPoint> Neighbors;
    const int32 DirsX[] = { -1, 1, 0, 0 };
    const int32 DirsY[] = { 0, 0, -1, 1 };

    for (int32 i = 0; i < 4; ++i)
    {
        FIntPoint N = FIntPoint(Idx.X + DirsX[i], Idx.Y + DirsY[i]);
        if (IsValidIndex(N))
            Neighbors.Add(N);
    }
    return Neighbors;
}