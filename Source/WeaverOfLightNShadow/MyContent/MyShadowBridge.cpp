#include "MyShadowBridge.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "UObject/UObjectIterator.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"

AMyShadowBridge::AMyShadowBridge()
{
    PrimaryActorTick.bCanEverTick = false;

    // ---------- Mesh setup ------------
    BridgeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BridgeMesh"));
    SetRootComponent(BridgeMesh);

    // Start hidden and with no collision until sufficient light is detected
    SetActive(false);

    // Default local sample points, can be overriden in BeginPlay
    SampleLocalPoints = { FVector::ZeroVector, FVector(150.f, 0.f, 0.f), FVector(300.f, 0.f, 0.f) };
}

void AMyShadowBridge::BeginPlay()
{
    Super::BeginPlay();

    /*
    * Distribute lighting sample points along the bridge based
    * on mesh bounds. This allows any part of the bridge to be
    * activated by light, instead of a single location.
    */
    if (BridgeMesh)
    {
        const FBoxSphereBounds B = BridgeMesh->Bounds;
        const float Len = B.BoxExtent.X * 1.8f;
        if (Len > 1.f)
        {
            SampleLocalPoints.Empty();
            SampleLocalPoints.Add(FVector(0.f, 0.f, 0.f));
            SampleLocalPoints.Add(FVector(Len * 0.33, 0.f, 0.f));
            SampleLocalPoints.Add(FVector(Len * 0.66, 0.f, 0.f));
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Bridge (BeginPlay)."));

    // Periodically re-evaluate lighting instead of checking every frame, cheaper
    GetWorldTimerManager().SetTimer(CheckTimer, this, &AMyShadowBridge::CheckLighting, CheckInterval, true, 0.05f);
}

void AMyShadowBridge::SetActive(bool bActive)
{
    bIsActive = bActive;

    if (BridgeMesh)
    {
        // Toggle both visibility and collision
        BridgeMesh->SetVisibility(bActive, true);
        BridgeMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics
            : ECollisionEnabled::NoCollision);
    }
    UE_LOG(LogTemp, Warning, TEXT("SetActive called"));
}

void AMyShadowBridge::CheckLighting()
{
    if (!BridgeMesh) return;

    const FTransform T = BridgeMesh->GetComponentTransform();
    float MaxSample = 0.f;

    /* 
    * Evaluate lighting at each sample point on the bridge and
    * activate bridge if Any porint exceeds the threshold
    */ 
    for (const FVector& Local : SampleLocalPoints)
    {
        const FVector WorldP = T.TransformPosition(Local);
        MaxSample = FMath::Max(MaxSample, AccumulateIntensityAtPoint(WorldP));
        if (MaxSample >= Threshold) break; // Early exit once threshold is reached
    }

    const bool bShouldBeActive = (MaxSample >= Threshold);
    // Only toggle when state actually changes
    if (bShouldBeActive != bIsActive)
    {
        SetActive(bShouldBeActive);
    }
}

float AMyShadowBridge::AccumulateIntensityAtPoint(const FVector& P) const
{
    float Sum = 0.f;

    // Accumulate all point lights in the world with a simplified distance based falloff model
    for (TObjectIterator<UPointLightComponent> It; It; ++It)
    {
        const UPointLightComponent* PLC = *It;
        if (!PLC || PLC->GetWorld() != GetWorld()) continue;
        if (!PLC->IsVisible() || !PLC->bAffectsWorld) continue;

        Sum += PointLightIntensityAt(PLC, P);
    }

    // Accumulate all spot lights in the world with both distance and cone angle falloff
    for (TObjectIterator<USpotLightComponent> It; It; ++It)
    {
        const USpotLightComponent* SLC = *It;
        if (!SLC || SLC->GetWorld() != GetWorld()) continue;
        if (!SLC->IsVisible() || !SLC->bAffectsWorld) continue;

        Sum += SpotLightIntensityAt(SLC, P);
    }

    return Sum;
}

float AMyShadowBridge::PointLightIntensityAt(const UPointLightComponent* L, const FVector& P)
{
    if (!L || !L->IsVisible()) return 0.f;

    const FVector Lpos = L->GetComponentLocation();
    const float   R = L->AttenuationRadius;
    const float   d = FVector::Distance(Lpos, P);
    if (d >= R || R <= 1.f) return 0.f;

    /*
    * Quadratic distance falloff
    * - Full intensity at source (1)
    * - Fades to zero at attenuation radius
    */
    const float t = 1.f - (d / R);
    const float falloff = t * t;

    return L->Intensity * falloff;
}

float AMyShadowBridge::SpotLightIntensityAt(const USpotLightComponent* L, const FVector& P)
{
    if (!L || !L->IsVisible()) return 0.f;

    const FVector Lpos = L->GetComponentLocation();
    const float   R = L->AttenuationRadius;
    const float   d = FVector::Distance(Lpos, P);
    if (d >= R || R <= 1.f) return 0.f;

    // Distance falloff(same as point light)
    const float t = 1.f - (d / R);
    const float distFalloff = t * t;

    /*
    * Angular falloff
    * Spotlights emit light within a cone defined by inner and outer cone angles
    */
    const FVector Dir = L->GetComponentTransform().GetUnitAxis(EAxis::X);
    const float cosTheta = FVector::DotProduct(Dir, (P - Lpos).GetSafeNormal());
    const float thetaDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(cosTheta, -1.f, 1.f)));

    const float inner = L->InnerConeAngle;
    const float outer = L->OuterConeAngle;
    if (thetaDeg >= outer) return 0.f;

    float ang = 1.f;
    if (thetaDeg > inner && outer > inner)
    {
        ang = 1.f - ((thetaDeg - inner) / (outer - inner)); // 1 at inner, 0 at outer
    }

    return L->Intensity * distFalloff * ang;
}
