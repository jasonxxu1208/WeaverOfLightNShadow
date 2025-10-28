// MyWand.cpp
#include "MyWand.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

AMyWand::AMyWand()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
    WandMesh->SetupAttachment(RootComponent);

    StrongLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("StrongLight"));
    StrongLight->SetupAttachment(RootComponent);
    StrongLight->SetVisibility(false);
    StrongLight->SetIntensity(5000.0f);
    StrongLight->SetOuterConeAngle(25.0f);
    StrongLight->SetAttenuationRadius(800.0f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder"));
    if (MeshFinder.Succeeded())
    {
        WandMesh->SetStaticMesh(MeshFinder.Object);
        WandMesh->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.5f));
    }

    
    static ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFinder(TEXT("/Game/FirstPerson/MI_FirstPersonColorway"));
    if (MaterialFinder.Succeeded())
    {
        WandMesh->SetMaterial(0, MaterialFinder.Object);
    }

    WandMesh->SetSimulatePhysics(false);
    WandMesh->SetEnableGravity(false);
    WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WandMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

    if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(RootComponent))
    {
        PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

}

void AMyWand::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("=== Wand testing start ==="));
    UE_LOG(LogTemp, Warning, TEXT("Wand count: %d"), ChargeCount);

    ToggleLight(nullptr);
    AttackEnemy(nullptr);
    ActivateStrongLight();

    UE_LOG(LogTemp, Warning, TEXT("Count left: %d"), ChargeCount);
    UE_LOG(LogTemp, Warning, TEXT("=== Test over ==="));
}

void AMyWand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyWand::ToggleLight(AActor* TargetActor)
{
    UE_LOG(LogTemp, Warning, TEXT("ToggleLight"));
}

void AMyWand::AttackEnemy(AActor* TargetEnemy)
{
    UE_LOG(LogTemp, Warning, TEXT("LMB pressed Attact()called, Count left: %d"), ChargeCount);
    if (ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Attack succeed！"));
    }
    // 消耗次数
    if (!ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough charges to attack!"));
        return;
    }

    if (TargetEnemy && TargetEnemy->ActorHasTag("Enemy"))
    {
        TargetEnemy->Destroy();
        UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed! Charge consumed."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid enemy target for attack"));
        // 注意：即使没有敌人，次数也已经消耗了
    }
}

void AMyWand::ActivateStrongLight()
{
    UE_LOG(LogTemp, Warning, TEXT("GPressed - ActivateStrongLight called，Count left: %d"), ChargeCount);
    if (ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Illuminate！"));
    }
    // 消耗次数
    if (!ConsumeCharge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough charges for strong light!"));
        return;
    }

    bIsStrongLightActive = true;
    StrongLight->SetVisibility(true);

    GetWorld()->GetTimerManager().SetTimer(
        StrongLightTimer,
        this,
        &AMyWand::DeactivateStrongLight,
        StrongLightDuration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Strong light activated! Charge consumed."));
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