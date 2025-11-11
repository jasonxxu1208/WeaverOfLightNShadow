#include "MyGhost.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyGhost::AMyGhost()
{
	PrimaryActorTick.bCanEverTick = false;

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
	GetWorldTimerManager().SetTimer(HoverTimer, this, &AMyGhost::HoverTick, 0.016f, true);
}

void AMyGhost::OnHurtBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32, bool, const FHitResult&)
{
	if (!OtherActor || OtherActor == this) return;

	KillPlayer(OtherActor);
}

void AMyGhost::KillPlayer_Implementation(AActor* Victim)
{
	UE_LOG(LogTemp, Warning, TEXT("Ghost %s touched %s -> PLAYER DIED"), *GetName(), *Victim->GetName());

	if (bRespawnOnKill)
	{
		const FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
		UGameplayStatics::OpenLevel(this, LevelName);
	}
}

void AMyGhost::HoverTick()
{
	if (HoverAmplitude <= 0.f || HoverSpeed <= 0.f) return;

	const float T = GetWorld()->GetTimeSeconds() * HoverSpeed;
	const float Offset = FMath::Sin(T) * HoverAmplitude;
	FVector P = StartLoc;
	P.Z += Offset;
	SetActorLocation(P, false, nullptr, ETeleportType::TeleportPhysics);
}
