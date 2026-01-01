#include "MyGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaverOfLightNShadowCharacter.h"

// Sets default values
AMyGhost::AMyGhost()
{
	PrimaryActorTick.bCanEverTick = true;

	// Sphere trigger used to detect player contact
	HurtTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("HurtTrigger"));
	SetRootComponent(HurtTrigger);

	HurtTrigger->InitSphereRadius(60.f);
	HurtTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HurtTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	HurtTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	HurtTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HurtTrigger->SetGenerateOverlapEvents(true);

	// Visual mesh
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	GhostMesh->SetupAttachment(HurtTrigger);
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Used by wand attack logic
	Tags.Add(TEXT("Enemy"));
}

void AMyGhost::BeginPlay()
{
	Super::BeginPlay();

	// Overlap to kill player
	HurtTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMyGhost::OnHurtBeginOverlap);

	// Cache starting location for patrol and hover calculation
	StartLoc = GetActorLocation();

	UE_LOG(LogTemp, Verbose, TEXT("Ghost spawned at %s. Patrol A=%s B=%s"),
		*StartLoc.ToCompactString(),
		*GetPatrolA().ToCompactString(),
		*GetPatrolB().ToCompactString());
}

void AMyGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* 
	* Horizontal movement handled manually
	* Vertical movement uses gravity + walkable surface checks
	* Ground check using downward line trace
	*/
	FHitResult GroundHit;
	bOnGround = IsStandingOnWalkable(&GroundHit);

	if (!bOnGround)
	{
		// Apply gravity while falling
		Velocity.Z = FMath::Clamp(Velocity.Z + GravityZ * DeltaTime, -MaxFallSpeed, MaxFallSpeed);

	}
	else
	{
		// Snap ghost slightly above walkable surface
		const FVector L = GetActorLocation();
		SetActorLocation(FVector(L.X, L.Y, GroundHit.ImpactPoint.Z + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
		Velocity.Z = 0.f;
	}
	// Apply movement
	const FVector Delta(
		Velocity.X * DeltaTime,
		Velocity.Y * DeltaTime,
		bOnGround ? 0.f :Velocity.Z * DeltaTime
	);
	FHitResult SweepHit;
	AddActorWorldOffset(Delta, true, &SweepHit);

	// Stop vertical movement if hitting a walkable surface
	if (SweepHit.bBlockingHit && FMath::Abs(SweepHit.ImpactNormal.Z) > 0.5f)
	{
		Velocity.Z = 0.f;
	}

	// Destroy ghost if it falls below world KillZ
	const float KillZ = GetWorld()->GetWorldSettings()->KillZ;
	const float Z = GetActorLocation().Z;
	if (Z < KillZ)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ghost %s Z=%.1f -> destroy"), *GetName(), GetActorLocation().Z, KillZ);
		Destroy();
		return;
	}
	// Apply hovering only when grounded
	if (bOnGround)
	{
		UpdateHover(DeltaTime);
	}
	// Update AI state and movement behavior
	UpdateState(DeltaTime);
}

void AMyGhost::UpdateHover(float DeltaTime)
{
	if (HoverAmplitude <= 0.f || HoverSpeed <= 0.f) return;

	HoverTime += DeltaTime * HoverSpeed;
	const float Offset = FMath::Sin(HoverTime) * HoverAmplitude;

	FVector P = GetActorLocation();
	P.Z = StartLoc.Z + Offset;
	SetActorLocation(P, false);
}

FVector AMyGhost::GetPatrolA()const
{
	// Patrol start point
	return FVector(StartLoc.X, StartLoc.Y, GetActorLocation().Z);
}

FVector AMyGhost::GetPatrolB()const
{
	// Patrol end point
	const FVector DestXY = StartLoc + WanderOffset;
	return FVector(DestXY.X, DestXY.Y, GetActorLocation().Z);
}

void AMyGhost::MoveTowards(const FVector& Destination, float Speed, float DeltaTime)
{
	// Constant speed movement toward destionation
	FVector P = GetActorLocation();
	FVector To = Destination - P;

	To.Z = 0.f;
	const float Dist = To.Size();
	if (Dist < KINDA_SMALL_NUMBER) return;

	const FVector Dir = To / Dist;
	const FVector Step = Dir * Speed * DeltaTime;

	// Prevent overshooting
	if (Step.SizeSquared() > Dist * Dist)
	{
		P = Destination;
	}
	else
	{
		P += Step;
	}
	SetActorLocation(P, false);

	if (!Dir.IsNearlyZero())
	{
		const FRotator Face = Dir.Rotation();
		SetActorRotation(FRotator(0.f, Face.Yaw, 0.f));
	}
	
	// Movement sound interval scales with the current AI state
	float interval = MoveInterval_Patrol;
	if (CurrentState == EGhostState::Chase)
	{
		interval = MoveInterval_Chase;
	}
	MoveTimer += DeltaTime;
	if (MoveTimer >= interval)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MoveSound, GetActorLocation());
		MoveTimer = 0.f;
	}
}

/*
* Simple state AI
* - Patrol: Move back and forth between two points
* - Chase: Move toward the player when within detect radius
*/
void AMyGhost::UpdateState(float DeltaTime)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		const FVector GhostXY(GetActorLocation().X, GetActorLocation().Y, 0.f);
		const FVector PlayerXY(PlayerPawn->GetActorLocation().X, PlayerPawn->GetActorLocation().Y, 0.f);
		const float Dist = FVector::Dist(GhostXY, PlayerXY);

		if (CurrentState == EGhostState::Patrol && Dist <= DetectRadius)
		{
			CurrentState = EGhostState::Chase;
			UGameplayStatics::PlaySound2D(this, ChaseSound);
			UE_LOG(LogTemp, Verbose, TEXT("Ghost: Chase(%.1f)"), Dist);
		}
		else if (CurrentState == EGhostState::Chase && Dist >= LoseRadius)
		{
			CurrentState = EGhostState::Patrol;
			UE_LOG(LogTemp, Verbose, TEXT("Ghost: Patrol(%.1f)"), Dist);
		}

		if (CurrentState == EGhostState::Chase)
		{
			FVector Destination = PlayerPawn->GetActorLocation();
			Destination.Z = GetActorLocation().Z;
			MoveTowards(Destination, ChaseSpeed, DeltaTime);
			return;
		}
	}
	// Patrol behavior, move back and forth between A and B
	const FVector A = GetPatrolA();
	const FVector B = GetPatrolB();
	const FVector Target = bGoingToB ? B : A;
	MoveTowards(Target, PatrolSpeed, DeltaTime);

	const float d = FVector::Dist2D(GetActorLocation(), Target);
	if (d <= PatrolSwitchDistance)
	{
		bGoingToB = !bGoingToB;
	}
}

void AMyGhost::OnHurtBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32, bool, const FHitResult&)
{
	if (!OtherActor || OtherActor == this) return;
	if (Cast<APawn>(OtherActor) == nullptr) return;
	if (!OtherActor->ActorHasTag(TEXT("Player"))) return;
	// Kill palyer on contack
	KillPlayer(OtherActor);
}

void AMyGhost::KillPlayer_Implementation(AActor* Victim)
{
	if (AWeaverOfLightNShadowCharacter* Player = Cast<AWeaverOfLightNShadowCharacter>(Victim))
	{
		UE_LOG(LogTemp, Warning, TEXT("Ghost %s touched %s -> PLAYER DIED"), *GetName(), *Victim->GetName());
		Player->Die();
	}
	
}

bool AMyGhost::IsStandingOnWalkable(FHitResult* OutHit)const
{
	// Line trace downward to detect walkable tagged surfaces
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0, 0, -50.f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GhostGround), false, this);
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		return false;

	if (AActor* A = Hit.GetActor())
	{
		if (A->ActorHasTag(TEXT("Walkable")))
		{
			if (OutHit) *OutHit = Hit;
			return true;
		}
	}
	return false;
}