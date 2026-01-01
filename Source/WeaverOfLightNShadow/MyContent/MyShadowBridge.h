#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyShadowBridge.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class USpotLightComponent;

/*
* AMyShadowBridge
* 
* A light-driven bridge that becomes visible and collidable
* when sufficient light intensity is detected along its surface
* 
* The bridge samples nearby PointLights(Torches) and SpotLights(Lumos)
* and enables itself once the threshold is reached
*/
UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyShadowBridge : public AActor
{
    GENERATED_BODY()

public:
    AMyShadowBridge();

protected:
    // Initialize sampling points and start lighting checks
    virtual void BeginPlay() override;

    // ------------ Components --------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    UStaticMeshComponent* BridgeMesh;

    // ------------ Activation parameters ------------

    // Minimum accumulated light intensity required to activate the bridge
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    float Threshold = 50.f;

    // Interval(seconds) between lighting checks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    float CheckInterval = 0.10f;

    // Local-space sample points used to probe lighting along the bridge
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    TArray<FVector> SampleLocalPoints;

    // Current state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge|Activation")
    bool bIsActive = false;

    FTimerHandle CheckTimer;

    // ----------- Gameplay Action ---------------
   
    // Enable or disable visibility and collision
    UFUNCTION(BlueprintCallable, Category = "Bridge|Activation")
    void SetActive(bool bActive);

    //Periodiccally samples light intensity
    UFUNCTION()
    void CheckLighting();

    // Sum intensity from all point/spot lights at a world-space point
    float AccumulateIntensityAtPoint(const FVector& P) const;

    // Distance-based falloff model for a point light
    static float PointLightIntensityAt(const UPointLightComponent* L, const FVector& P);

    // Distance + angular falloff model for a spot light
    static float SpotLightIntensityAt(const USpotLightComponent* L, const FVector& P);
};
