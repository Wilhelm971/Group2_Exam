#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "PowerCore.generated.h"


UCLASS()
class GROUP2_EXAM_API APowerCore : public APowerNode
{
	GENERATED_BODY()

public:
	APowerCore();

protected:
	virtual void BeginPlay() override;

public:
	// How often the core emits a power pulse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power")
	float PulseInterval = 5.0f;
	

	// Called periodically to emit power
	void EmitPowerPulse();

private:
	FTimerHandle TimerHandle_Pulse;
};
