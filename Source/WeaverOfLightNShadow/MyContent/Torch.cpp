
#include "Torch.h"
#include <Kismet/GameplayStatics.h>

ATorch::ATorch()
{
	PrimaryActorTick.bCanEverTick = false;

	// ---------- Mesh setup -------------
	TorchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
	SetRootComponent(TorchMesh);
	TorchMesh->SetMobility(EComponentMobility::Static);

	// ----------- Light setup -----------
	TorchLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TorchLight"));
	TorchLight->SetupAttachment(TorchMesh);
	TorchLight->SetMobility(EComponentMobility::Stationary);

	// Position the light above the torch mesh
	TorchLight->SetRelativeLocation(FVector(0, 0, 50));

	// Lighting setup to make the light more like a flame
	TorchLight->IntensityUnits = ELightUnits::Lumens;
	TorchLight->SetIntensity(900.f);
	TorchLight->bUseTemperature = true;
	TorchLight->Temperature = 2000.f;
	TorchLight->CastShadows = true;

}

void ATorch::BeginPlay()
{
	Super::BeginPlay();
	// Initialize light visibility based on the starting state
	TorchLight->SetVisibility(bIsLit);
}

void ATorch::ToggleLight()
{
	SetLit(!bIsLit);
}

void ATorch::SetLit(bool bNewLit)
{
	bIsLit = bNewLit;
	TorchLight->SetVisibility(bIsLit);
	// Play different sound based on the new state
	if (bIsLit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LightupSound, GetActorLocation());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, LightoffSound, GetActorLocation());
	}
}
