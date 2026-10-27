#include "SystemCameraEventer.h"

#include "Components/SplineComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

#if WITH_EDITOR

#include "Misc/MessageDialog.h"
#include "UObject/Package.h"

#endif

ASystemCameraEventer::ASystemCameraEventer()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorEntered);
	OnActorEndOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorLeft);
}

void ASystemCameraEventer::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It)
	{
		GameCamera = *It;
		break;
	}

	CheckExistingOverlap();
}

void ASystemCameraEventer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PendingActor.IsValid())
	{
		if (!PendingActor->IsOverlappingActor(this))
		{
			PendingActor = nullptr;
			ActivationTimer = 0.f;
		}
		else if ((!bFireOnlyOnce || !bTriggered) && (!GameCamera /*|| !GameCamera->IsLastTriggeredEventer(this)*/))
		{
			if (ActivationDelay <= 0.f)
			{
				ActivateEventer(PendingActor.Get());
				PendingActor = nullptr;
				ActivationTimer = 0.f;
			}
			else
			{
				ActivationTimer += DeltaTime;

				if (ActivationTimer >= ActivationDelay)
				{
					ActivateEventer(PendingActor.Get());
					PendingActor = nullptr;
					ActivationTimer = 0.f;
				}
			}
		}
	}

	if (!PendingActor.IsValid() && (!bFireOnlyOnce || !bTriggered))
	{
		CheckExistingOverlap();
	}

	if (bPathActive && GameCamera /*&& GameCamera->IsLastTriggeredEventer(this)*/)
	{
		UpdatePath(DeltaTime);
	}
}

void ASystemCameraEventer::OnActorEntered(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (!OtherActor->IsA<ACharacter>())
	{
		return;
	}

	if (bFireOnlyOnce && bTriggered)
	{
		return;
	}

	/*if (GameCamera && GameCamera->IsLastTriggeredEventer(this))
	{
		return;
	}*/

	PendingActor = OtherActor;
	ActivationTimer = 0.f;

	if (ActivationDelay <= 0.f)
	{
		ActivateEventer(OtherActor);
		PendingActor = nullptr;
	}
}

void ASystemCameraEventer::ActivateEventer(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}

	if (bFireOnlyOnce && bTriggered)
	{
		return;
	}

	/*if (GameCamera && GameCamera->IsLastTriggeredEventer(this))
	{
		return;
	}*/

	GameCamera = nullptr;

	for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It)
	{
		if (It->GetOwner() == OtherActor)
		{
			GameCamera = *It;
			break;
		}
	}

	if (!GameCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("EVENTER: Camera for %s not found"), *GetNameSafe(OtherActor));
		return;
	}

	/*if (GameCamera->IsLastTriggeredEventer(this))
	{
		return;
	}*/

	bTriggered = true;

	//GameCamera->SetLastTriggeredEventer(this);

	ApplyCameraSettings();

	if (CameraPathActor)
	{
		StartPath();
	}
}

void ASystemCameraEventer::OnActorLeft(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (!OtherActor->IsA<ACharacter>())
	{
		return;
	}

	if (PendingActor.Get() == OtherActor)
	{
		PendingActor = nullptr;
		ActivationTimer = 0.f;
	}

	if (!GameCamera)
	{
		return;
	}

	//const bool bWasLastTriggeredEventer = GameCamera->IsLastTriggeredEventer(this);

	StopPath();

	/*if (bWasLastTriggeredEventer)
	{
		GameCamera->ClearLastTriggeredEventer(this);
	}*/

	if (!bRestoreOnExit)
	{
		return;
	}

	if (RestoreCameraPathActor)
	{
		CameraPathActor = RestoreCameraPathActor;
		StartDistanceThreshold = RestoreStartDistanceThreshold;
		PathOffset = RestorePathOffset;
		FollowSmoothing = RestoreFollowSmoothing;
		bPathLoop = bRestorePathLoop;

		/*if (bWasLastTriggeredEventer)
		{
			GameCamera->SetLastTriggeredEventer(this);
			StartPath();
		}*/
	}
}

void ASystemCameraEventer::CheckExistingOverlap()
{
	TArray<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || !Actor->IsA<ACharacter>())
		{
			continue;
		}

		if (bFireOnlyOnce && bTriggered)
		{
			return;
		}

		if (PendingActor.Get() == Actor)
		{
			continue;
		}

		/*if (GameCamera && GameCamera->IsLastTriggeredEventer(this))
		{
			continue;
		}*/

		OnActorEntered(this, Actor);
		return;
	}
}

void ASystemCameraEventer::ApplyTargets(AActor* NewConnectTarget)
{
	if (!GameCamera)
	{
		return;
	}

	GameCamera->SetConnectTarget(NewConnectTarget);
}

void ASystemCameraEventer::ApplyCameraSettings()
{
	if (!GameCamera)
	{
		return;
	}

	ApplyTargets(ConnectTarget);

	GameCamera->SetCameraState(TargetState, TransitionMode);
}

void ASystemCameraEventer::StartPath()
{
	if (!GameCamera || !CameraPathActor)
	{
		return;
	}

	USplineComponent* Spline = CameraPathActor->FindComponentByClass<USplineComponent>();

	if (!Spline)
	{
		return;
	}

	ActiveSpline = Spline;

	ActiveStartDistanceThreshold = StartDistanceThreshold;
	ActivePathOffset = PathOffset;
	ActiveFollowSmoothing = FollowSmoothing;
	bActivePathLoop = bPathLoop;

	float ClosestDistance = 0.f;

	if (!FindClosestPathDistance(ClosestDistance))
	{
		ClosestDistance = 0.f;
	}

	CurrentPathDistance = FMath::Clamp(ClosestDistance, 0.f, ActiveSpline->GetSplineLength());

	bPathActive = true;
	bPathFinished = false;

	ApplyPathRotation();
}

void ASystemCameraEventer::StopPath()
{
	/*const bool bIsLastTriggeredEventer = GameCamera && GameCamera->IsLastTriggeredEventer(this);*/

	bPathActive = false;
	bPathFinished = false;
	ActiveSpline = nullptr;

	/*if (!GameCamera || !bIsLastTriggeredEventer)
	{
		return;
	}*/

	//GameCamera->ClearPathRotation();

	if (!bRestoreOnExit)
	{
		return;
	}

	GameCamera->SetConnectTarget(RestoreConnectTarget);
	GameCamera->SetCameraState(RestoreTargetState, RestoreTransitionMode);
}

bool ASystemCameraEventer::FindClosestPathDistance(float& OutDistance) const
{
	OutDistance = 0.f;

	if (!ActiveSpline || !GameCamera)
	{
		return false;
	}

	AActor* Target = GameCamera->GetConnectTarget();

	if (!Target)
	{
		Target = GameCamera->GetPlayerTarget();
	}

	if (!Target)
	{
		return false;
	}

	const FVector TargetLocation = Target->GetActorLocation();
	const float InputKey = ActiveSpline->FindInputKeyClosestToWorldLocation(TargetLocation);

	OutDistance = ActiveSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

	return true;
}

void ASystemCameraEventer::UpdatePath(float DeltaTime)
{
	if (!bPathActive || !GameCamera || !ActiveSpline)
	{
		return;
	}

	/*if (!GameCamera->IsLastTriggeredEventer(this))
	{
		return;
	}*/

	AActor* Target = GameCamera->GetConnectTarget();

	if (!Target)
	{
		Target = GameCamera->GetPlayerTarget();
	}

	if (!Target)
	{
		return;
	}

	const float SplineLength = ActiveSpline->GetSplineLength();

	if (SplineLength <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector TargetWorldLocation = Target->GetActorLocation();

	const float ClosestKey = ActiveSpline->FindInputKeyClosestToWorldLocation(TargetWorldLocation);

	const float PlayerDistance = ActiveSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);

	float TargetDistance = PlayerDistance;

	if (PlayerDistance > ActiveStartDistanceThreshold)
	{
		TargetDistance = (PlayerDistance - ActiveStartDistanceThreshold) + ActivePathOffset;
	}
	else
	{
		TargetDistance = ActivePathOffset;
	}

	if (bActivePathLoop)
	{
		TargetDistance = FMath::Fmod(TargetDistance, SplineLength);

		if (TargetDistance < 0.f)
		{
			TargetDistance += SplineLength;
		}
	}
	else
	{
		TargetDistance = FMath::Clamp(TargetDistance, 0.f, SplineLength);
	}

	/*if (TransitionMode == ECameraTransitionMode::Instant)
	{
		CurrentPathDistance = TargetDistance;
	}
	else
	{*/
		CurrentPathDistance = FMath::FInterpTo(CurrentPathDistance, TargetDistance, DeltaTime, ActiveFollowSmoothing);
	//}

	const FVector WorldPosition = ActiveSpline->GetLocationAtDistanceAlongSpline(CurrentPathDistance, ESplineCoordinateSpace::World);

	FGlobalCameraState State = TargetState;

	switch (TargetState.PositionMode)
	{
	case EGlobalCameraPositionMode::World:
	{
		State.Position = WorldPosition;
		break;
	}

	case EGlobalCameraPositionMode::PlayerTarget:
	{
		AActor* PlayerTarget = GameCamera->GetPlayerTarget();

		if (PlayerTarget)
		{
			State.Position = PlayerTarget->GetActorTransform().InverseTransformPosition(WorldPosition);
		}
		else
		{
			State.Position = WorldPosition;
		}

		break;
	}

	case EGlobalCameraPositionMode::ConnectTarget:
	{
		AActor* TargetActor = GameCamera->GetConnectTarget();

		if (TargetActor)
		{
			State.Position = TargetActor->GetActorTransform().InverseTransformPosition(WorldPosition);
		}
		else
		{
			State.Position = WorldPosition;
		}

		break;
	}

	default:
	{
		State.Position = WorldPosition;
		break;
	}
	}

	/*if (TargetState.bUsePathRotation)
	{
		const FRotator PathRotation = ActiveSpline->GetRotationAtDistanceAlongSpline(CurrentPathDistance, ESplineCoordinateSpace::World);

		GameCamera->SetPathRotation(PathRotation);
	}
	else
	{
		GameCamera->ClearPathRotation();
	}*/

	GameCamera->SetCameraState(State, TransitionMode);

	if (!bActivePathLoop)
	{
		if (CurrentPathDistance >= SplineLength - 1.f)
		{
			bPathFinished = true;
		}
		else if (CurrentPathDistance <= 1.f)
		{
			bPathFinished = true;
		}
		else
		{
			bPathFinished = false;
		}
	}
}

void ASystemCameraEventer::ApplyPathRotation()
{
	if (!GameCamera || !ActiveSpline)
	{
		return;
	}

	/*if (!GameCamera->IsLastTriggeredEventer(this))
	{
		return;
	}*/

	if (!TargetState.bUsePathRotation)
	{
		//GameCamera->ClearPathRotation();
		return;
	}

	const FRotator WorldRotation = ActiveSpline->GetRotationAtDistanceAlongSpline(CurrentPathDistance, ESplineCoordinateSpace::World);

	if (TargetState.PositionMode == EGlobalCameraPositionMode::World)
	{
		//GameCamera->SetPathRotation(WorldRotation);
		return;
	}

	AActor* Target = nullptr;

	if (TargetState.PositionMode == EGlobalCameraPositionMode::PlayerTarget)
	{
		Target = GameCamera->GetPlayerTarget();
	}
	else if (TargetState.PositionMode == EGlobalCameraPositionMode::ConnectTarget)
	{
		Target = GameCamera->GetConnectTarget();
	}

	if (!Target)
	{
		//GameCamera->SetPathRotation(WorldRotation);
		return;
	}

	const FRotator RelativeRotation = Target->GetActorTransform().InverseTransformRotation(WorldRotation.Quaternion()).Rotator();

	//GameCamera->SetPathRotation(RelativeRotation);
}


#if WITH_EDITOR

	bool ASystemCameraEventer::ConfirmOverwrite() const
	{
		const EAppReturnType::Type Result = FMessageDialog::Open(
			EAppMsgType::YesNo,
			NSLOCTEXT(
				"SystemCameraEventer",
				"OverwritePreset",
				"Preset already exists. Overwrite it?"
			)
		);
		return Result == EAppReturnType::Yes;
	}

	AActor* ASystemCameraEventer::FindActorByName(const FName& ActorName) const
	{
		if (ActorName.IsNone() || !GetWorld())
		{
			return nullptr;
		}
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			if (It->GetFName() == ActorName)
			{
				return *It;
			}
		}
		return nullptr;
	}

	void ASystemCameraEventer::SavePreset()
	{
		if (!PresetData || PresetName.IsNone())
		{
			return;
		}
		FGlobalCameraEventerPreset Preset;
		
		Preset.PresetName = PresetName;
		// Camera
		Preset.CameraState = TargetState;
		Preset.ConnectTargetName = ConnectTarget.GetFName();
		Preset.TransitionMode = TransitionMode;
		// Path
		Preset.CameraPathActorName = CameraPathActor.GetFName();
		Preset.StartDistanceThreshold = StartDistanceThreshold;
		Preset.PathOffset = PathOffset;
		Preset.FollowSmoothing = FollowSmoothing;
		Preset.bPathLoop = bPathLoop;
		// Trigger
		Preset.bRestoreOnExit = bRestoreOnExit;
		Preset.RestorePresetName = RestorePresetName;
		Preset.RestoreCameraState = RestoreTargetState;
		Preset.RestoreConnectTargetName = RestoreConnectTarget.GetFName();
		Preset.RestoreTransitionMode = RestoreTransitionMode;
		// Restore Path
		Preset.RestoreCameraPathActorName = RestoreCameraPathActor.GetFName();
		Preset.RestoreStartDistanceThreshold = RestoreStartDistanceThreshold;
		Preset.RestorePathOffset = RestorePathOffset;
		Preset.RestoreFollowSmoothing = RestoreFollowSmoothing;
		Preset.bRestorePathLoop = bRestorePathLoop;
		// Trigger Settings
		Preset.bFireOnlyOnce = bFireOnlyOnce;
		Preset.ActivationDelay = ActivationDelay;
		FGlobalCameraEventerPreset ExistingPreset;
		const bool bExists = PresetData->FindPreset(PresetName, ExistingPreset);
		if (bExists && !ConfirmOverwrite())
		{
			return;
		}
		PresetData->SavePreset(PresetName, Preset, bExists);
	}

	void ASystemCameraEventer::LoadPreset()
	{
		if (!PresetData || PresetName.IsNone())
		{
			return;
		}
		FGlobalCameraEventerPreset Preset;
		if (!PresetData->FindPreset(PresetName, Preset))
		{
			return;
		}
		PresetName = Preset.PresetName;
		// Camera
		TargetState = Preset.CameraState;
		ConnectTarget = FindActorByName(Preset.ConnectTargetName);
		TransitionMode = Preset.TransitionMode;
		// Path
		CameraPathActor = FindActorByName(Preset.CameraPathActorName);
		StartDistanceThreshold = Preset.StartDistanceThreshold;
		PathOffset = Preset.PathOffset;
		FollowSmoothing = Preset.FollowSmoothing;
		bPathLoop = Preset.bPathLoop;
		// Trigger
		bRestoreOnExit = Preset.bRestoreOnExit;
		RestorePresetName = Preset.RestorePresetName;
		RestoreTargetState = Preset.RestoreCameraState;
		RestoreConnectTarget = FindActorByName(Preset.RestoreConnectTargetName);
		RestoreTransitionMode = Preset.RestoreTransitionMode;
		// Restore Path
		RestoreCameraPathActor = FindActorByName(Preset.RestoreCameraPathActorName);
		RestoreStartDistanceThreshold = Preset.RestoreStartDistanceThreshold;
		RestorePathOffset = Preset.RestorePathOffset;
		RestoreFollowSmoothing = Preset.RestoreFollowSmoothing;
		bRestorePathLoop = Preset.bRestorePathLoop;
		// Trigger Settings
		bFireOnlyOnce = Preset.bFireOnlyOnce;
		ActivationDelay = Preset.ActivationDelay;
	}

	void ASystemCameraEventer::DeletePreset()
	{
		if (!PresetData || PresetName.IsNone())
		{
			return;
		}
		PresetData->DeletePreset(PresetName);
	}

#endif
