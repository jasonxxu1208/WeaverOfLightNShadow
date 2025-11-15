// MyWand.cpp
#include "MyWand.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Torch.h"
#include "MyGhost.h"

AMyWand::AMyWand()
{
    PrimaryActorTick.bCanEverTick = true;

    // --- Mesh setup ---
    WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
    SetRootComponent(WandMesh);

    // Assign mesh & material (can be overridden in Blueprint)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder"));
    if (MeshFinder.Succeeded())
    {
        WandMesh->SetStaticMesh(MeshFinder.Object);
        WandMesh->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.5f));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/FirstPerson/MI_FirstPersonColorway"));
    if (MaterialFinder.Succeeded())
    {
        WandMesh->SetMaterial(0, MaterialFinder.Object);
    }

    // Collision: disable to prevent pushing character
    WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WandMesh->SetSimulatePhysics(false);
    WandMesh->SetEnableGravity(false);

    // --- Spot light setup ---
    StrongLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("StrongLight"));
    StrongLight->SetupAttachment(WandMesh);

    StrongLight->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    StrongLight->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

    StrongLight->SetVisibility(false);
    ApplyStrongLightSettings();
}

void AMyWand::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Wand spawned (BeginPlay)."));
}

void AMyWand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyWand::ToggleLight(AActor* TargetActor)
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleLight"));
    constexpr float InteractDistance = 1500.f;
    AActor* AimActor = TargetActor ? TargetActor : GetAimedActor(InteractDistance);
    if (!AimActor) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ToggleLight: No targe in the range"));
        return;
    }
    ATorch* Torch = Cast<ATorch>(AimActor);
    if (Torch)
    {
        Torch->ToggleLight();
        UE_LOG(LogTemp, Warning, TEXT("ToggleLight: Toggled Torch %s"), *Torch->GetName());
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("ToggleLight: Target %s is not a Torch"), *AimActor->GetName());
    }
}

void AMyWand::AttackEnemy(AActor* TargetEnemy)
{
    UE_LOG(LogTemp, Warning, TEXT("LMB pressed Attact()called, Count left: %d"), ChargeCount);
    AActor* AimActor = TargetEnemy ? TargetEnemy : GetAimedActor(500.f);
    if (!ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough charges to attack"));
        return;
    }

    //AActor* HitActor = TargetEnemy;
    UE_LOG(LogTemp, Warning, TEXT("Name of Target"), *AimActor->GetName());
    if (!AimActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack: no target"));
        return;
    }

    if (AimActor->ActorHasTag(FName("Enemy")))
    {
        AimActor->Destroy();
        UE_LOG(LogTemp, Warning, TEXT("Attack: enemy destroyed"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack: %s is not an enemy"), *AimActor->GetName());
    }
}

void AMyWand::ActivateStrongLight()
{
    if (bIsStrongLightActive)
    {
        return;
    }
    if (!ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough charges for strong light!"));
        return;
    }

    bIsStrongLightActive = true;
    ApplyStrongLightSettings();
    if (StrongLight)
    {
        UE_LOG(LogTemp, Warning, TEXT("Strong Light founded"));
    }
    StrongLight->SetVisibility(true);

    GetWorld()->GetTimerManager().SetTimer(
        StrongLightTimer,
        this,
        &AMyWand::DeactivateStrongLight,
        StrongLightDuration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Strong light activated! Charges left: %d"), ChargeCount);
}

AActor* AMyWand::GetAimedActor(float MaxDistance)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return nullptr;

    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector End = CameraLocation + (CameraRotation.Vector() * MaxDistance);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    if (GetOwner())
    {
        CollisionParams.AddIgnoredActor(GetOwner());
    }

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECC_Visibility, CollisionParams))
    {
        return HitResult.GetActor();
    }

    return nullptr;
}

bool AMyWand::ConsumeCharge()
{
    if (ChargeCount > 0)
    {
        ChargeCount--;
        UE_LOG(LogTemp, Warning, TEXT("Charge consumed! Remaining: %d"), ChargeCount);
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("No charges remaining!"));
    return false;
}

void AMyWand::DeactivateStrongLight()
{
    bIsStrongLightActive = false;
    StrongLight->SetVisibility(false);
    UE_LOG(LogTemp, Warning, TEXT("Strong light deactivated"));
}

void AMyWand::ApplyStrongLightSettings()
{
    if (!StrongLight) return;

    StrongLight->SetIntensity(StrongLightIntensity);
    StrongLight->SetAttenuationRadius(StrongLightAttenuationRadius);
    StrongLight->SetInnerConeAngle(StrongLightInnerCone);
    StrongLight->SetOuterConeAngle(StrongLightOuterCone);
    StrongLight->SetLightColor(StrongLightColor);
    StrongLight->SetCastShadows(bStrongLightCastShadows);
}