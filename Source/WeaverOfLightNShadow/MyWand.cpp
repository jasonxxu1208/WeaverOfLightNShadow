// MyWand.cpp
#include "MyWand.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Torch.h"
#include "MyGhost.h"

AMyWand::AMyWand()
{
    PrimaryActorTick.bCanEverTick = false;

    // --------- Mesh setup -------------

    // Create wand mehs and make it the root component
    WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
    SetRootComponent(WandMesh);

    // Assign default mesh and material (can be overridden in Blueprint)
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

    // disable collision and physics to prevent pushing character
    WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WandMesh->SetSimulatePhysics(false);
    WandMesh->SetEnableGravity(false);

    // ----------- Spot light(Lumos) setup --------------
    
    // Create the spotlight component for Lumos spell and attach it to the wand
    StrongLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("StrongLight"));
    StrongLight->SetupAttachment(WandMesh);

    // Adjusting the position and rotation to make it at the top of the wand and point forward
    StrongLight->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    StrongLight->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

    // Only enabled when Lumos is casted
    StrongLight->SetVisibility(false);
    ApplyStrongLightSettings();
}

void AMyWand::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Wand spawned (BeginPlay)."));
}

//void AMyWand::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//}

void AMyWand::ToggleLight(AActor* TargetActor)
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleLight"));
    // Use provided target if available, if not perform a raycast
    constexpr float InteractDistance = 1500.f;
    AActor* AimActor = TargetActor ? TargetActor : GetAimedActor(InteractDistance);
    if (!AimActor) 
    {
        UE_LOG(LogTemp, Warning, TEXT("ToggleLight: No targe in the range"));
        return;
    }
    // Only interact with torches
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
    // Get attack target from parameter or raycast
    AActor* AimActor = TargetEnemy ? TargetEnemy : GetAimedActor(500.f);
    
    if (!AimActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack: no target"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Name of Target"), *AimActor->GetName());
    // Only interact with actors with tag Enemy
    if (!AimActor->ActorHasTag(FName("Enemy")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack: %s is not an enemy"), *AimActor->GetName());
        return;
    }
    // Check if charges available
    if (!ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough charges to attack"));
        return;
    }
    // Play attack sound and destroy enemy
    UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    AimActor->Destroy();
    UE_LOG(LogTemp, Warning, TEXT("Attack: enemy destroyed"));
}

void AMyWand::ActivateStrongLight()
{
    // Prevent re-activating
    if (bIsStrongLightActive)
    {
        return;
    }
    // Check if charges available
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
        // Play Lumos sound
        UGameplayStatics::PlaySoundAtLocation(this, LumosSound, GetActorLocation());
    }
    StrongLight->SetVisibility(true);
    // Disable strong light after duration
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
    // Get the character's camera viewpoint for raycasting
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return nullptr;

    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector End = CameraLocation + (CameraRotation.Vector() * MaxDistance);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;

    // Ignore the wand and its owning during raycast
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
    // Reset Lumos state and hide spotlight
    bIsStrongLightActive = false;
    StrongLight->SetVisibility(false);
    UE_LOG(LogTemp, Warning, TEXT("Strong light deactivated"));
}

void AMyWand::ApplyStrongLightSettings()
{
    if (!StrongLight) return;
    // Apply all the details(parameters)
    StrongLight->SetIntensity(StrongLightIntensity);
    StrongLight->SetAttenuationRadius(StrongLightAttenuationRadius);
    StrongLight->SetInnerConeAngle(StrongLightInnerCone);
    StrongLight->SetOuterConeAngle(StrongLightOuterCone);
    StrongLight->SetLightColor(StrongLightColor);
    StrongLight->SetCastShadows(bStrongLightCastShadows);
}