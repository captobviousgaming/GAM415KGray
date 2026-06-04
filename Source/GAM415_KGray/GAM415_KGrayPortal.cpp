// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_KGrayPortal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AGAM415_KGrayPortal::AGAM415_KGrayPortal()
{
	PrimaryActorTick.bCanEverTick = true;

	// [Week 5] Initialize the root and portal mesh.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	PortalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// [Week 5] Initialize the teleport volume and bind the overlap event.
	TeleportVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TeleportVolume"));
	TeleportVolume->SetupAttachment(RootComponent);
	TeleportVolume->SetBoxExtent(FVector(10.0f, 100.0f, 100.0f));
	TeleportVolume->OnComponentBeginOverlap.AddDynamic(this, &AGAM415_KGrayPortal::OnOverlapBegin);

	// [Week 5] Create a scene capture component to simulate the live feed.
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootComponent);

	bIsTeleporting = false;
}

void AGAM415_KGrayPortal::BeginPlay()
{
	Super::BeginPlay();

	// [Week 5] Assign the render target to the scene capture live feed upon starting the game.
	if (RenderTarget)
	{
		SceneCapture->TextureTarget = RenderTarget;
	}
}

void AGAM415_KGrayPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSceneCapture();
}

void AGAM415_KGrayPortal::UpdateSceneCapture()
{
	if (!LinkedPortal) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager) return;

	// [Week 5] Implement scene capture component logic to calculate distance and camera rotation to simulate depth.
	FTransform CameraTransform = CameraManager->GetTransform();
	FTransform RelativeTransform = CameraTransform.GetRelativeTransform(GetActorTransform());

	// [Week 5] Rotate the relative transform by 180 degrees so the camera faces out of the linked portal correctly.
	FVector AdjustedLocation = RelativeTransform.GetLocation().RotateAngleAxis(180.0f, FVector(0, 0, 1));
	FRotator AdjustedRotation = RelativeTransform.GetRotation().Rotator();
	AdjustedRotation.Yaw += 180.0f;

	FTransform AdjustedRelativeTransform(AdjustedRotation, AdjustedLocation, RelativeTransform.GetScale3D());

	// Apply the simulated depth and rotation to the linked portal's scene capture camera.
	FTransform NewCaptureTransform = AdjustedRelativeTransform * LinkedPortal->GetActorTransform();
	LinkedPortal->SceneCapture->SetWorldTransform(NewCaptureTransform);
}

void AGAM415_KGrayPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// [Week 5] Implement portal functionality, linking one portal to another and teleporting the player.
	if (LinkedPortal && OtherActor && OtherActor->IsA(ACharacter::StaticClass()) && !bIsTeleporting)
	{
		// Disable teleporting on the destination portal to prevent an infinite bouncing loop.
		LinkedPortal->bIsTeleporting = true;

		// Calculate the relative entry distance and angle of the player.
		FTransform ActorTransform = OtherActor->GetActorTransform();
		FTransform RelativeTransform = ActorTransform.GetRelativeTransform(GetActorTransform());

		// [Week 5] After teleportation, face the player in the forward direction relative to the exit portal.
		FVector AdjustedLocation = RelativeTransform.GetLocation().RotateAngleAxis(180.0f, FVector(0, 0, 1));
		FRotator AdjustedRotation = RelativeTransform.GetRotation().Rotator();
		AdjustedRotation.Yaw += 180.0f;

		FTransform AdjustedRelativeTransform(AdjustedRotation, AdjustedLocation, RelativeTransform.GetScale3D());
		FTransform NewActorTransform = AdjustedRelativeTransform * LinkedPortal->GetActorTransform();

		// Execute the teleport.
		ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->TeleportTo(NewActorTransform.GetLocation(), NewActorTransform.GetRotation().Rotator(), false, true);
			PlayerCharacter->GetController()->SetControlRotation(NewActorTransform.GetRotation().Rotator());
		}

		// Re-enable the portal after a split second.
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (LinkedPortal) LinkedPortal->bIsTeleporting = false;
			}, 0.1f, false);
	}
}