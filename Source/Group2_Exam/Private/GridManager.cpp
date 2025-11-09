#include "GridManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "NodeActor.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

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

void AGridManager::SpawnGrid()
{
	ClearGrid();

	if (!NodeClass) { return; }

	UWorld* World = GetWorld();
	for (int32 y = 0; y < GridSizeY; ++y)
	{
		for (int32 x = 0; x < GridSizeX; ++x)
		{
			FVector Location = GetActorLocation() + FVector(x * NodeSpacing, y * NodeSpacing, 0);
			FRotator Rotation = FRotator::ZeroRotator;
			ANodeActor* Node = World->SpawnActor<ANodeActor>(NodeClass, Location, Rotation);

			if (Node)
			{
				Node->X = x;
				Node->Y = y;
				Grid.Add(Node);
			}
		}
	}
}

void AGridManager::StartPathfinding()
{
	if (bPathfindingInProgress) return;
	ResetNodes();

	StartNode = GetNode(StartCoords.X, StartCoords.Y);
	EndNode = GetNode(EndCoords.X, EndCoords.Y);

	if (!StartNode || !EndNode) return;

	StartNode->SetState(ENodeState::Start);
	EndNode->SetState(ENodeState::Target);

	StartNode->GCost = 0.f;
	StartNode->HCost = GetHeuristic(StartNode, EndNode);
	StartNode->CalculateFCost();

	OpenSet.Add(StartNode);
	StartNode->SetState(ENodeState::Open);

	bPathfindingInProgress = true;
	GetWorldTimerManager().SetTimer(StepTimerHandle, this, &AGridManager::StepAStar, StepDelay, true);
}

void AGridManager::ResetNodes()
{
	GetWorldTimerManager().ClearTimer(StepTimerHandle);
	bPathfindingInProgress = false;

	for (ANodeActor* Node : Grid)
	{
		Node->GCost = 0.f;
		Node->HCost = 0.f;
		Node->FCost = 0.f;
		Node->Parent = nullptr;
		Node->SetState(Node->bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
	}

	OpenSet.Empty();
	ClosedSet.Empty();
}

void AGridManager::StepAStar()
{
	if (OpenSet.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(StepTimerHandle);
		bPathfindingInProgress = false;
		return;
	}

	ANodeActor* Current = OpenSet[0];

	// Select node with smallest f-cost
	for (ANodeActor* Node : OpenSet)
	{
		if (Node->FCost < Current->FCost || (Node->FCost == Current->FCost && Node->HCost < Current->HCost))
		{
			Current = Node;
		}
	}

	// Move current node from OPEN to CLOSED
	OpenSet.Remove(Current);
	ClosedSet.Add(Current);

	if (Current != StartNode && Current != EndNode)
	{
		Current->SetState(ENodeState::Closed);
	}

	// Check if we hit the goal node
	if (Current == EndNode)
	{
		TArray<ANodeActor*> Path = ReconstructPath(EndNode);
		for (ANodeActor* Step : Path)
		{
			if (Step != StartNode && Step != EndNode)
			{
				Step->SetState(ENodeState::Path);
			}
		}
		GetWorldTimerManager().ClearTimer(StepTimerHandle);
		bPathfindingInProgress = false;
		return;
	}

	// Expand neighbors of the current node
	HandleNodeExpansion(Current);
}

// Updating g, h, and f
void AGridManager::HandleNodeExpansion(ANodeActor* Current)
{
	for (ANodeActor* Neighbor : GetNeighbors(Current))
	{
		// Skip blocked nodes or already evaluated ones
		if (!Neighbor->bIsWalkable || ClosedSet.Contains(Neighbor)) continue;

		/*
		THEORY:
		Tentative cost g(n) = g(current) + movement cost + tile cost factor.
		For diagonal movement, we slightly increase the cost (√2 ≈ 1.414)
		*/
		float Dx = FMath::Abs(Current->X - Neighbor->X);
		float Dy = FMath::Abs(Current->Y - Neighbor->Y);
		float MoveCost = (Dx == 1 && Dy == 1) ? 1.414f : 1.0f;

		// This line connects to the "g(n)" definition
		float TentativeG = Current->GCost + MoveCost * Neighbor->TileCost;


		/*
		THEORY:
		If we’ve found a cheaper route to this neighbor,
		or it hasn’t been seen before, record this path.

		In pseudocode:
		if n not in OPEN or TentativeG < g(n)
		 parent(n) = current
		 g(n) = TentativeG
		 h(n) = heuristic(n)
		 f(n) = g(n) + h(n)
		*/

		if (!OpenSet.Contains(Neighbor) || TentativeG < Neighbor->GCost)
		{
			Neighbor->Parent = Current;
			Neighbor->GCost = TentativeG;
			Neighbor->HCost = GetHeuristic(Neighbor, EndNode);
			Neighbor->CalculateFCost();

			if (!OpenSet.Contains(Neighbor))
			{
				OpenSet.Add(Neighbor);
				if (Neighbor != StartNode && Neighbor != EndNode)
				{
					Neighbor->SetState(ENodeState::Open);
				}
			}
		}
	}
}

ANodeActor* AGridManager::GetNode(int32 X, int32 Y)
{
	if (X < 0 || Y < 0 || X >= GridSizeX || Y >= GridSizeY)
	{
		return nullptr;
	}

	return Grid[Y * GridSizeX + X];
}

/*
   Return all 8 potential neighbors (4 orthogonal + 4 diagonal)
   Similar to exploring edges in a graph.
*/
TArray<ANodeActor*> AGridManager::GetNeighbors(ANodeActor* Node)
{
	TArray<ANodeActor*> Neighbors;
	int32 Directions[8][2] =
	{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
	};
	for (auto& Direction : Directions)
	{
		ANodeActor* LocalNode = GetNode(Node->X + Direction[0], Node->Y + Direction[1]);
		if (LocalNode)
		{
			Neighbors.Add(LocalNode);
		}
	}
	return Neighbors;
}


/*
   h(n): Heuristic = straight-line (Euclidean) distance between nodes
   This ensures A* is informed—it estimates how close we are to the goal.
*/
float AGridManager::GetHeuristic(ANodeActor* A, ANodeActor* B)
{
	float Dx = A->X - B->X;
	float Dy = A->Y - B->Y;

	return FMath::Sqrt(Dx * Dx + Dy * Dy);
}


/*
   RECONSTRUCT PATH
   Follow Parent links from End back to Start.
   This directly corresponds to the final backtrack step in A* theory,
   where we recover the sequence of nodes representing the optimal route.
*/
TArray<ANodeActor*> AGridManager::ReconstructPath(ANodeActor* End)
{
	TArray<ANodeActor*> Path;
	for (ANodeActor* Node = End; Node; Node = Node->Parent)
	{
		Path.Insert(Node, 0);
	}
	return Path;
}
