#include "TDGameMode.h"
#include "TopDownCameraPawn.h"
#include "TopDownPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

ATDGameMode::ATDGameMode()
{
    DefaultPawnClass = ATopDownCameraPawn::StaticClass();
    PlayerControllerClass = ATopDownPlayerController::StaticClass();
}

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