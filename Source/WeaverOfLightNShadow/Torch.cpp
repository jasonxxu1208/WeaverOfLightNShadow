// Fill out your copyright notice in the Description page of Project Settings.

#include "Torch.h"
#include <Kismet/GameplayStatics.h>
// Sets default values
ATorch::ATorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TorchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
	SetRootComponent(TorchMesh);
	TorchMesh->SetMobility(EComponentMobility::Static);

	TorchLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TorchLight"));
	TorchLight->SetupAttachment(TorchMesh);
	TorchLight->SetMobility(EComponentMobility::Stationary);
	TorchLight->SetRelativeLocation(FVector(0, 0, 50));
	TorchLight->IntensityUnits = ELightUnits::Lumens;
	TorchLight->SetIntensity(900.f);
	TorchLight->bUseTemperature = true;
	TorchLight->Temperature = 2000.f;
	TorchLight->CastShadows = true;

}

// Called when the game starts or when spawned
void ATorch::BeginPlay()
{
	Super::BeginPlay();
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
	if (bIsLit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LightupSound, GetActorLocation());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, LightoffSound, GetActorLocation());
	}
}
