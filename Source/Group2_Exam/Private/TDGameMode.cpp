#include "TDGameMode.h"
#include "TopDownCameraPawn.h"
#include "TopDownPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

/**
 * @brief Default constructor for ATDGameMode.
 * 
 * Sets the default pawn and player controller classes.
 */
ATDGameMode::ATDGameMode()
{
    DefaultPawnClass = ATopDownCameraPawn::StaticClass();
    PlayerControllerClass = ATopDownPlayerController::StaticClass();
}

/**
 * @brief Triggers the victory condition.
 * 
 * Pauses the game, sets UI input mode, and displays the victory widget.
 */
void ATDGameMode::Victory()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetPause(true);
        PC->SetInputMode(FInputModeUIOnly());

        if (VictoryWidgetClass)
        {
            UUserWidget* VictoryWidget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass);
            if (VictoryWidget)
            {
                VictoryWidget->AddToViewport(9999);  // Top layer
            }
        }
    }
}

/**
 * @brief Triggers the defeat condition.
 * 
 * Pauses the game, sets UI input mode, and displays the lose widget.
 */
void ATDGameMode::Defeat()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetPause(true);
        PC->SetInputMode(FInputModeUIOnly());

        if (LoseWidgetClass)
        {
            UUserWidget* LoseWidget = CreateWidget<UUserWidget>(PC, LoseWidgetClass);
            if (LoseWidget)
            {
                LoseWidget->AddToViewport(9999);
            }
        }
    }
}