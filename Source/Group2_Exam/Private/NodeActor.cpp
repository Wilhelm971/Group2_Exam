#include "NodeActor.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// ANodeActor: Represents a single node in the grid system.
// Used for pathfinding and grid-based navigation.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
ANodeActor::ANodeActor()
{
	// Enable ticking if needed for dynamic updates.
	PrimaryActorTick.bCanEverTick = true;
}