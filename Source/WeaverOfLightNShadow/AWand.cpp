// AWand.cpp
#include "AWand.h"

AAWand::AAWand()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
    WandMesh->SetupAttachment(RootComponent);
}

void AAWand::BeginPlay()
{
    Super::BeginPlay();
}

void AAWand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 实现函数（即使暂时为空）
void AAWand::ToggleLight()
{
    // 暂时留空，后续实现
}

void AAWand::SuperIlluminate()
{
    // 暂时留空，后续实现
}

void AAWand::Attack()
{
    // 暂时留空，后续实现
}
