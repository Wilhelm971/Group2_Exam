

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "BaseHQ.generated.h"

class APowerNetworkManager;

UCLASS()
class GROUP2_EXAM_API ABaseHQ : public ABuilding
{
	GENERATED_BODY()
	
public:	
	ABaseHQ();
	
	virtual void BeginPlay() override;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HQ")
	float MaxHealth;

	UPROPERTY ( VisibleAnywhere, BlueprintReadOnly, Category = "HQ")
	float CurrentHealth;


	// Pulse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float PowerPerPulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float PulseInterval;

	// Range used for auto-connecting nearby buildings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float ConnectionRange;


	// Link to manager in the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power")
	APowerNetworkManager* PowerNetworkManager = nullptr;

	
	// Start/stop pulsing
	UFUNCTION()
	void StartPulsing();

	UFUNCTION()
	void StopPulsing();

	UFUNCTION()
	void Pulse();

	// Damage the HQ
	UFUNCTION()
	void ReceiveDamage(float Amount);

protected:
	FTimerHandle PulseTimerHandle;


};
