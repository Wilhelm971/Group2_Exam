

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerNetworkManager.generated.h"



class ABuilding;
class ABaseHQ;

USTRUCT(BlueprintType)
struct FBuildingConnection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABuilding* BuildingA = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABuilding* BuildingB = nullptr;
};



UCLASS()
class GROUP2_EXAM_API APowerNetworkManager : public AActor
{
	GENERATED_BODY()

public:
	APowerNetworkManager();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	// Editor-exposed manual connection list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	TArray<FBuildingConnection> ManualConnections;

	// HQ (exposed so designer can assign)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	ABaseHQ* HQReference = nullptr;

	// Pulse settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	float PulseInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	float PowerPerPulse = 100.f;



	// Public API
	UFUNCTION(BlueprintCallable)
	void RegisterBuilding(ABuilding* Building);

	UFUNCTION(BlueprintCallable)
	void UnregisterBuilding(ABuilding* Building);

	UFUNCTION(BlueprintCallable)
	void AddConnection(ABuilding* A, ABuilding* B);

	UFUNCTION(BlueprintCallable)
	void RemoveConnection(ABuilding* A, ABuilding* B);

	UFUNCTION(BlueprintCallable)
	void PulsePower();

private:
	// ------------------------------
	// Internal graph: do NOT mark as UPROPERTY (UHT limitation)
	// ------------------------------
	TMap<ABuilding*, TArray<ABuilding*>> AdjacencyList;

	// Registered building list (this also should not be UPROPERTY if it stores TArray of non-UProperty-friendly types)
	TArray<ABuilding*> RegisteredBuildings;

	
	// Helper to build adjacency list from ManualConnections at BeginPlay (or in editor-time utility)
	void BuildAdjacencyFromManualConnections();

	/** Helper: sends power recursively */
	void PropagatePower(ABuilding* From, float RemainingPower);

	/** Timer handle for repeated pulses */
	FTimerHandle PulseTimerHandle;
};
