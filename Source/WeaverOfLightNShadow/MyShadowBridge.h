#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyShadowBridge.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class USpotLightComponent;

UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyShadowBridge : public AActor
{
    GENERATED_BODY()

public:
    AMyShadowBridge();

protected:
    virtual void BeginPlay() override;

    /** The visible/collidable bridge geometry */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    UStaticMeshComponent* BridgeMesh;

    /** Intensity threshold (pseudo-lux) needed to turn the bridge on */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    float Threshold = 50.f;

    /** How often to re-check lighting (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    float CheckInterval = 0.10f;

    /** Probe positions in *local* space where we evaluate the light */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Activation")
    TArray<FVector> SampleLocalPoints;

    /** Current active state */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge|Activation")
    bool bIsActive = false;

    FTimerHandle CheckTimer;

    /** Make the bridge appear/disappear and toggle collision */
    UFUNCTION(BlueprintCallable, Category = "Bridge|Activation")
    void SetActive(bool bActive);

    /** Periodic light sampling */
    UFUNCTION()
    void CheckLighting();

    /** Sum intensity from all point/spot lights at a world-space point */
    float AccumulateIntensityAtPoint(const FVector& P) const;

    /** Simple distance falloff model for a point light */
    static float PointLightIntensityAt(const UPointLightComponent* L, const FVector& P);

    /** Distance + angular falloff model for a spot light */
    static float SpotLightIntensityAt(const USpotLightComponent* L, const FVector& P);
};
