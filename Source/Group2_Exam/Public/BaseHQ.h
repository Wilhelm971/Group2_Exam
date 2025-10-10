

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseHQ.generated.h"

UCLASS()
class GROUP2_EXAM_API ABaseHQ : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseHQ();
	
	// Called when the game starts or when spawned
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

	// Range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float ConnectionRange;


	// Functions
	
	UFUNCTION ()
	void ReceiveDamage(float amount);

	UFUNCTION()
	void StartPulsing();

	UFUNCTION()
	void EndPulsing();

	UFUNCTION()
	void Pulse();


	
protected:
	FTimerHandle PulseTimerHandle;


};
