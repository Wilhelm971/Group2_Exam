

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
	ABuilding* A = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABuilding* B = nullptr;
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
	// Manual connections (editor-friendly). Used at BeginPlay to build internal graph.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	TArray<FBuildingConnection> ManualConnections;

	// Pulse travel speed (units per second) used for visual travel time or per-edge delay multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power Network")
	float TravelDelayPerEdge;
	
	// Internal adjacency list (not a UPROPERTY due to UHT limits)
	TMap<ABuilding*, TArray<ABuilding*>> AdjacencyList;

	// Registered buildings (dynamically added)
	TArray<ABuilding*> RegisteredBuildings;

	// Register/unregister buildings at runtime
	UFUNCTION(BlueprintCallable, Category="Power Network")
	void RegisterBuilding(ABuilding* Building);

	UFUNCTION(BlueprintCallable, Category="Power Network")
	void UnregisterBuilding(ABuilding* Building);
	
	// Add / remove connection at runtime
	UFUNCTION(BlueprintCallable, Category="Power Network")
	void AddConnection(ABuilding* A, ABuilding* B);

	UFUNCTION(BlueprintCallable, Category="Power Network")
	void RemoveConnection(ABuilding* A, ABuilding* B);

	// Pulse from a start building (HQ) with a given amount. This starts a delayed propagation so you can see transfer.
	UFUNCTION(BlueprintCallable, Category="Power Network")
	void PulsePower(ABuilding* Start, float InitialAmount);

	// Debug draw adjacency
	UFUNCTION(BlueprintCallable, Category="Power Network")
	void DrawNetworkDebug(float Duration = 2.0f);


private:
	// Build adjacency from ManualConnections (called in BeginPlay)
	void BuildAdjacencyFromManualConnections();

	// Internal: schedule delivery to neighbor with delay
	void ScheduleDeliver(ABuilding* Target, float Amount, float Delay);

	// Internal: actual deliver called by timer
	void DeliverPower(ABuilding* Target, float Amount);

	// Helper: recursively propagate from Start with BFS, but schedule each hop slightly delayed to show travel
	void StartPropagationWithDelays(ABuilding* Start, float InitialAmount);

};
