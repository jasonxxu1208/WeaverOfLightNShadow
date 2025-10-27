// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWand.h"

// Sets default values
AMyWand::AMyWand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// 创建魔杖网格体
	WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
	WandMesh->SetupAttachment(RootComponent);
}

void AMyWand::BeginPlay()
{
	Super::BeginPlay();
}

void AMyWand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyWand::SimpleToggleLight()
{
	// 最简单的实现 - 什么都不做
	// 先确保能编译，后续再添加功能
}