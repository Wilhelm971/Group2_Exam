


#include "BaseHQ.h"

// Sets default values
ABaseHQ::ABaseHQ()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	PowerPerPulse = 50.0f;
	PulseInterval = 3.0f;

	ConnectionRange = 800.0f;

}

// Called when the game starts or when spawned
void ABaseHQ::BeginPlay()
{
	Super::BeginPlay();
	StartPulsing();
	
}


void ABaseHQ::StartPulsing()
{
	GetWorldTimerManager().SetTimer(PulseTimerHandle, this , &ABaseHQ::Pulse, PulseInterval, true, 0.5f);
}

void ABaseHQ::EndPulsing()
{
	GetWorldTimerManager().ClearTimer(PulseTimerHandle);
}

void ABaseHQ::Pulse()
{
}

void ABaseHQ::ReceiveDamage(float amount)
{
	CurrentHealth -= amount;

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;

		EndPulsing();

		UE_LOG(LogTemp, Display, TEXT("HQ destroyed! Game Over"));
	}
}


