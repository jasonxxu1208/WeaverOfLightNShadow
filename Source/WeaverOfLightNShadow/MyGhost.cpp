#include "MyGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaverOfLightNShadowCharacter.h"

// Sets default values
AMyGhost::AMyGhost()
{
	PrimaryActorTick.bCanEverTick = true;

	HurtTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("HurtTrigger"));
	SetRootComponent(HurtTrigger);
	HurtTrigger->InitSphereRadius(60.f);
	HurtTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HurtTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	HurtTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	HurtTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HurtTrigger->SetGenerateOverlapEvents(true);

	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	GhostMesh->SetupAttachment(HurtTrigger);
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(TEXT("Enemy"));
}

// Called when the game starts or when spawned
void AMyGhost::BeginPlay()
{
	Super::BeginPlay();

	HurtTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMyGhost::OnHurtBeginOverlap);

	StartLoc = GetActorLocation();
	//GetWorldTimerManager().SetTimer(HoverTimer, this, &AMyGhost::HoverTick, 0.016f, true);
	UE_LOG(LogTemp, Verbose, TEXT("Ghost spawned at %s. Patrol A=%s B=%s"),
		*StartLoc.ToCompactString(),
		*GetPatrolA().ToCompactString(),
		*GetPatrolB().ToCompactString());
}

void AMyGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult GroundHit;
	bOnGround = IsStandingOnWalkable(&GroundHit);

	if (!bOnGround)
	{
		Velocity.Z = FMath::Clamp(Velocity.Z + GravityZ * DeltaTime, -MaxFallSpeed, MaxFallSpeed);

	}
	else
	{
		const FVector L = GetActorLocation();
		SetActorLocation(FVector(L.X, L.Y, GroundHit.ImpactPoint.Z + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
		Velocity.Z = 0.f;
	}

	const FVector Delta(
		Velocity.X * DeltaTime,
		Velocity.Y * DeltaTime,
		bOnGround ? 0.f :Velocity.Z * DeltaTime
	);
	FHitResult SweepHit;
	AddActorWorldOffset(Delta, true, &SweepHit);
	if (SweepHit.bBlockingHit && FMath::Abs(SweepHit.ImpactNormal.Z) > 0.5f)
	{
		Velocity.Z = 0.f;
	}
	const float KillZ = GetWorld()->GetWorldSettings()->KillZ;
	const float Z = GetActorLocation().Z;
	if (Z < KillZ)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ghost %s Z=%.1f -> destroy"), *GetName(), GetActorLocation().Z, KillZ);
		Destroy();
		return;
	}
	if (bOnGround)
	{
		UpdateHover(DeltaTime);
	}
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
	return FVector(StartLoc.X, StartLoc.Y, GetActorLocation().Z);
}

FVector AMyGhost::GetPatrolB()const
{
	const FVector DestXY = StartLoc + WanderOffset;
	return FVector(DestXY.X, DestXY.Y, GetActorLocation().Z);
}

void AMyGhost::MoveTowards(const FVector& Destination, float Speed, float DeltaTime)
{
	FVector P = GetActorLocation();
	FVector To = Destination - P;

	To.Z = 0.f;
	const float Dist = To.Size();
	if (Dist < KINDA_SMALL_NUMBER) return;

	const FVector Dir = To / Dist;
	const FVector Step = Dir * Speed * DeltaTime;

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
}
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