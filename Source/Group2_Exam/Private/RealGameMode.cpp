
#include "RealGameMode.h"
#include "PlacementController.h"/


ARealGameMode::ARealGameMode()
{
	PlayerControllerClass = APlacementController::StaticClass();



	DefaultPawnClass = nullptr;

}
