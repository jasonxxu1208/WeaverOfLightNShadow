// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeaverOfLightNShadowCharacter.h"
#include "MyWand.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaverOfLightNShadow.h"
#include <Kismet/GameplayStatics.h>

AWeaverOfLightNShadowCharacter::AWeaverOfLightNShadowCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void AWeaverOfLightNShadowCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AWeaverOfLightNShadowCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AWeaverOfLightNShadowCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWeaverOfLightNShadowCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWeaverOfLightNShadowCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AWeaverOfLightNShadowCharacter::LookInput);

		//Wand interaction mapped via Enhanced Input
		EnhancedInputComponent->BindAction(ToggleTorchAction, ETriggerEvent::Started, this, &AWeaverOfLightNShadowCharacter::HandleToggleTorch);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AWeaverOfLightNShadowCharacter::HandleAttack);
		EnhancedInputComponent->BindAction(LumosAction, ETriggerEvent::Started, this, &AWeaverOfLightNShadowCharacter::HandleLumos);
	}
	else
	{
		UE_LOG(LogWeaverOfLightNShadow, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AWeaverOfLightNShadowCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AWeaverOfLightNShadowCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AWeaverOfLightNShadowCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AWeaverOfLightNShadowCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AWeaverOfLightNShadowCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AWeaverOfLightNShadowCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AWeaverOfLightNShadowCharacter::HandleToggleTorch(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Toggle Torch triggered"));
	if (AMyWand* Wand = GetWand())
	{
		Wand->ToggleLight(nullptr);
		
	}
}

void AWeaverOfLightNShadowCharacter::HandleAttack(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Warning, TEXT("Attack triggered"));
	if (AMyWand* Wand = GetWand())
	{
		Wand->AttackEnemy(nullptr);
	}
}

void AWeaverOfLightNShadowCharacter::HandleLumos(const FInputActionValue& Value)
{

	
	if (AMyWand* Wand = GetWand())
	{
		Wand->ActivateStrongLight();
		UE_LOG(LogTemp, Warning, TEXT("Lumos triggered"));
	}
}

// Helper function to locate the wand actor attached to the character
AMyWand* AWeaverOfLightNShadowCharacter::GetWand() const
{
	// Search all actors currently attached to this character
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		if (Actor && Actor->IsA(AMyWand::StaticClass()))
		{
			return Cast<AMyWand>(Actor);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GetWand(): No attached AMyWand found on %s"), *GetName());
	return nullptr;
}

void AWeaverOfLightNShadowCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("WeaverOfLightNShadowCharacter::BeginPlay called"));
}

void AWeaverOfLightNShadowCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckKillZ();
	HandleFootsteps(DeltaTime);
}

/*
* Custom death handling when the player falls out of the levle
* Uses a short delay to allow death sound to play before reloading.
*/
void AWeaverOfLightNShadowCharacter::CheckKillZ()
{
	if (bIsDead) return;
	if (bDeathTimerStarted)return;
	const UWorld* W = GetWorld();
	if (W)
	{
		const float WorldKillZ = W->GetWorldSettings()->KillZ;
		// Custom override KillZ
		if (GetActorLocation().Z < -750.f)
		{
			bDeathTimerStarted = true;
			// Play death sound
			UGameplayStatics::PlaySound2D(this, DeathSound);
			// Delay level reload
			GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AWeaverOfLightNShadowCharacter::Die, 1.0f, false);
		}
	}
}

void AWeaverOfLightNShadowCharacter::Die()
{
	// Prevent multiple death triggers
	if (bIsDead) return;
	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Player died"));

	// Reload current level
	FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, LevelName);
}

void AWeaverOfLightNShadowCharacter::HandleFootsteps(float DeltaTime)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}
	const float Speed = GetVelocity().Size();
	if (Speed == 0.0f)
	{
		return;
	}
	FootstepTimer += DeltaTime;
	if (FootstepTimer >= FootstepInterval)
	{
		// Play movement sound
		UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
		FootstepTimer = 0.0f;
	}
}

void AWeaverOfLightNShadowCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	// Play jump sound when character touches ground after a jump
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
}