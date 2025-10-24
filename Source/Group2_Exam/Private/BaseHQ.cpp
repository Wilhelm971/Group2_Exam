


#include "BaseHQ.h"
#include "PowerNetworkManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseHQ::ABaseHQ()
{
 	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 200.0f;
	CurrentHealth = MaxHealth;
	PowerPerPulse = 50.0f;
	PulseInterval = 2.0f;

	ConnectionRange = 900.0f;

}

// Called when the game starts or when spawned
void ABaseHQ::BeginPlay()
{
	Super::BeginPlay();

	// try auto-find manager if not assigned
	if (!PowerNetworkManager)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNetworkManager::StaticClass(), Found);
		if (Found.Num() > 0)
			PowerNetworkManager = Cast<APowerNetworkManager>(Found[0]);
	}

	// register self with manager if available
	if (PowerNetworkManager)
		PowerNetworkManager->RegisterBuilding(this);

	StartPulsing();

	
}


void ABaseHQ::StartPulsing()
{
	if (!GetWorld()) return;
	GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &ABaseHQ::Pulse, PulseInterval, true, 0.5f);
}

void ABaseHQ::StopPulsing()
{
	GetWorldTimerManager().ClearTimer(PulseTimerHandle);
}

void ABaseHQ::Pulse()
{
	if (PowerNetworkManager)
	{
		PowerNetworkManager->PulsePower(this, PowerPerPulse);
		// UI feedback
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
			FString::Printf(TEXT("HQ pulse %.1f"), PowerPerPulse));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseHQ: no PowerNetworkManager assigned"));
	}
}

void ABaseHQ::ReceiveDamage(float amount)
{
	CurrentHealth -= amount;
	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;
		StopPulsing();
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HQ destroyed - Game Over"));
		// TODO: notify GameMode
	}
}


