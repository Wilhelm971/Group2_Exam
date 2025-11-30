#include "TDGameMode.h"
#include "TopDownCameraPawn.h"
#include "TopDownPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

/**
 * ATDGameMode: Manages game state, pawn/controller defaults, and win/lose screens.
 */

ATDGameMode::ATDGameMode()
{
    DefaultPawnClass = ATopDownCameraPawn::StaticClass();  // Top-down camera setup.
    PlayerControllerClass = ATopDownPlayerController::StaticClass();  // Custom input handling.
}

// Triggers victory: Pause and show UI.
void ATDGameMode::Victory()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetPause(true);  // Freeze gameplay.
        PC->SetInputMode(FInputModeUIOnly());  // UI focus.

        if (VictoryWidgetClass)
        {
            UUserWidget* VictoryWidget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass);
            if (VictoryWidget)
            {
                VictoryWidget->AddToViewport(9999);  // Top layer for overlay.
            }
        }
    }
}

// Triggers defeat: Pause and show UI.
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