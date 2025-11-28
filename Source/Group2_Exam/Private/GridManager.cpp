#include "GridManager.h"
#include "NodeActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// AGridManager: Manages a 2D grid for pathfinding.
// Spawns nodes, marks obstacles, and provides A* pathfinding.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Spawns the grid and marks obstacles.
void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    SpawnGrid();
    MarkObstacles();
}

// =============================================================
// SPAWN GRID
// =============================================================
// Creates and populates the grid with node actors.
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
            SpawnLoc += FVector(CellSize * 0.5f, CellSize * 0.5f, 0.f);  // Center in cell.

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
// Destroys all grid nodes and clears the array.
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
// MARK OBSTACLES
// =============================================================
// Checks for overlaps with static actors to mark non-walkable nodes.
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
        FVector Extent = FVector(CellSize * 0.4f);  // Slightly smaller for tolerance.

        OverlapActors.Empty();

        TArray<AActor*> ActorsToIgnore;  // Empty ignore list.
        UKismetSystemLibrary::BoxOverlapActors(
            World,
            Center,
            Extent,
            ObjectTypes,
            AActor::StaticClass(),
            ActorsToIgnore,
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
// PATHFINDING
// =============================================================
// Finds A* path between grid indices.
TArray<FIntPoint> AGridManager::FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx)
{
    TArray<FIntPoint> EmptyPath;
    if (!IsValidIndex(StartIdx) || !IsValidIndex(EndIdx)) return EmptyPath;

    ANodeActor* StartNode = Grid[StartIdx.X + StartIdx.Y * GridSizeX];
    ANodeActor* EndNode = Grid[EndIdx.X + EndIdx.Y * GridSizeX];

    if (!StartNode || !EndNode || !StartNode->bIsWalkable || !EndNode->bIsWalkable)
        return EmptyPath;

    // A* structures.
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
        // Find node with lowest FScore.
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

            float TentativeG = GScore.FindRef(Current) + 1.f;  // Uniform cost.

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

    return EmptyPath;  // No path found.
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
        Path.Insert(Current, 0);
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
    const int32 DirsY[] = { 0, 0, -1, 1 };  // Fixed: Was -2,2 – assuming typo, changed to standard 4-way.

    for (int32 i = 0; i < 4; ++i)
    {
        FIntPoint N = FIntPoint(Idx.X + DirsX[i], Idx.Y + DirsY[i]);
        if (IsValidIndex(N))
            Neighbors.Add(N);
    }
    return Neighbors;
}