#include "Camera/SystemCamera.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraStateComponent.h"
#include "Camera/CameraTargetComponent.h"
#include "Camera/CameraPositionComponent.h"
#include "Camera/CameraCollisionComponent.h"
#include "Camera/CameraRotationComponent.h"
#include "Camera/CameraTransitionComponent.h"
#include "Camera/FirstPersonCameraComponent.h"
#include "Camera/OrbitCameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

ASystemCamera::ASystemCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(Root);

	StateComponent = CreateDefaultSubobject<UCameraStateComponent>(TEXT("CameraState"));
	TargetComponent = CreateDefaultSubobject<UCameraTargetComponent>(TEXT("CameraTarget"));
	PositionComponent = CreateDefaultSubobject<UCameraPositionComponent>(TEXT("CameraPosition"));
	CollisionComponent = CreateDefaultSubobject<UCameraCollisionComponent>(TEXT("CameraCollision"));
	RotationComponent = CreateDefaultSubobject<UCameraRotationComponent>(TEXT("CameraRotation"));
	TransitionComponent = CreateDefaultSubobject<UCameraTransitionComponent>(TEXT("CameraTransition"));
	FirstPersonComponent = CreateDefaultSubobject<UFirstPersonCameraComponent>(TEXT("FirstPersonCamera"));
	OrbitComponent = CreateDefaultSubobject<UOrbitCameraComponent>(TEXT("OrbitCamera"));
}

void ASystemCamera::BeginPlay()
{
	Super::BeginPlay();

	if (TargetComponent)
	{
		if (IsValid(PlayerTarget))
		{
			TargetComponent->SetPlayerTarget(PlayerTarget);
		}
		else
		{
			PlayerTarget = TargetComponent->GetPlayerTarget();
		}

		TargetComponent->SetConnectTarget(ConnectTarget);
	}

	if (StateComponent)
	{
		StateComponent->CurrentState = CurrentState;
		StateComponent->TargetState = CurrentState;
		StateComponent->TransitionMode = TransitionMode;
	}

	if (PositionComponent)
	{
		PositionComponent->InitializePosition(
			CurrentState,
			GetPositionTarget(CurrentState.Position.PositionMode)
		);
	}

	if (CameraComponent)
	{
		CameraComponent->SetRelativeLocation(FVector::ZeroVector);
		CameraComponent->SetRelativeRotation(FRotator::ZeroRotator);
		CameraComponent->SetFieldOfView(CurrentState.Lens.FieldOfView);
	}
}

void ASystemCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PushStateToComponents();
	UpdateBlend(DeltaTime);
	UpdateCameraTransform(DeltaTime);
	PullStateFromComponents();
}

void ASystemCamera::PushStateToComponents()
{
	if (!StateComponent)
	{
		return;
	}

	StateComponent->CurrentState = CurrentState;
	StateComponent->TargetState = TargetState;
	StateComponent->TransitionMode = TransitionMode;
}

void ASystemCamera::PullStateFromComponents()
{
	if (!StateComponent)
	{
		return;
	}

	CurrentState = StateComponent->CurrentState;
	TargetState = StateComponent->TargetState;
	TransitionMode = StateComponent->TransitionMode;
}

void ASystemCamera::SetTransitionMode(EGlobalCameraTransitionMode NewMode)
{
	TransitionMode = NewMode;

	if (StateComponent)
	{
		StateComponent->SetTransitionMode(NewMode);
	}
}

void ASystemCamera::SetCameraState(
	const FGlobalCameraState& NewState,
	EGlobalCameraTransitionMode NewTransitionMode)
{
	if (!StateComponent)
	{
		return;
	}
	PushStateToComponents();
	StateComponent->SetTargetState(NewState, NewTransitionMode);
	PullStateFromComponents();

	if (TargetState.Target.FollowTarget == EGlobalCameraFollowTarget::None && PositionComponent)
	{
		PositionComponent->SetFixedWorldPosition(GetWorldPositionFromState(TargetState));
	}
}

void ASystemCamera::UpdateBlend(float DeltaTime)
{
	if (!StateComponent || !TransitionComponent)
	{
		return;
	}

	StateComponent->UpdateDiscreteSettings();

	TransitionComponent->UpdateTransition(
		StateComponent->CurrentState,
		StateComponent->TargetState,
		StateComponent->TransitionMode,
		DeltaTime
	);
}

void ASystemCamera::UpdateDiscreteSettings()
{
	if (StateComponent)
	{
		StateComponent->UpdateDiscreteSettings();
	}
}

void ASystemCamera::SetTargetPosition(FVector NewPosition, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Position.Position = NewPosition;
	SetTransitionMode(NewMode);

	if (TargetState.Target.FollowTarget == EGlobalCameraFollowTarget::None && PositionComponent)
	{
		PositionComponent->SetFixedWorldPosition(GetWorldPositionFromState(TargetState));
	}

	PushStateToComponents();
}

void ASystemCamera::SetTargetPositionMode(
	EGlobalCameraPositionMode NewMode,
	EGlobalCameraTransitionMode Transition)
{
	TargetState.Position.PositionMode = NewMode;
	SetTransitionMode(Transition);

	if (TargetState.Target.FollowTarget == EGlobalCameraFollowTarget::None && PositionComponent)
	{
		PositionComponent->SetFixedWorldPosition(GetWorldPositionFromState(TargetState));
	}

	PushStateToComponents();
}

void ASystemCamera::SetTargetFollow(
	EGlobalCameraFollowTarget NewTarget,
	EGlobalCameraTransitionMode NewMode)
{
	TargetState.Target.FollowTarget = NewTarget;
	SetTransitionMode(NewMode);

	if (NewTarget == EGlobalCameraFollowTarget::None && PositionComponent)
	{
		PositionComponent->SetFixedWorldPosition(GetWorldPositionFromState(TargetState));
	}

	PushStateToComponents();
}

void ASystemCamera::SetTargetCollision(bool bEnabled, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Position.bUseCollision = bEnabled;
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetRotation(FRotator NewRotation, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Rotation.Rotation = NewRotation;

	if (TargetState.Control.ControlMode != EGlobalCameraControlMode::ThirdPerson)
	{
		TargetState.Orbit.OrbitYaw = FRotator::NormalizeAxis(NewRotation.Yaw);
		TargetState.Orbit.OrbitPitch = FMath::Clamp(
			NewRotation.Pitch,
			TargetState.Orbit.MinPitch,
			TargetState.Orbit.MaxPitch
		);
	}

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetRotationSettings(
	bool bUseTargetRotation,
	bool bUsePathRotation,
	float NewLockOnRotation,
	EGlobalCameraTransitionMode NewMode)
{
	const bool bWasUsingTargetRotation = TargetState.Rotation.bUseTargetRotation;

	if (
		bWasUsingTargetRotation &&
		!bUseTargetRotation &&
		TargetState.Control.ControlMode != EGlobalCameraControlMode::ThirdPerson
	)
	{
		const FRotator CameraRotation = GetActorRotation();

		CurrentState.Rotation.Rotation = CameraRotation;
		TargetState.Rotation.Rotation = CameraRotation;

		CurrentState.Orbit.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
		CurrentState.Orbit.OrbitPitch = FMath::Clamp(
			CameraRotation.Pitch,
			CurrentState.Orbit.MinPitch,
			CurrentState.Orbit.MaxPitch
		);

		TargetState.Orbit.OrbitYaw = CurrentState.Orbit.OrbitYaw;
		TargetState.Orbit.OrbitPitch = CurrentState.Orbit.OrbitPitch;
	}

	TargetState.Rotation.bUseTargetRotation = bUseTargetRotation;
	TargetState.Rotation.bUsePathRotation = bUsePathRotation;
	TargetState.Rotation.LockOnRotation = NewLockOnRotation;

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetLookAt(
	bool bEnabled,
	EGlobalCameraLookMode NewLookMode,
	EGlobalCameraTransitionMode NewMode)
{
	const bool bWasLookingAtTarget = TargetState.Aim.bLookAtTarget;

	if (
		bWasLookingAtTarget &&
		!bEnabled &&
		TargetState.Control.ControlMode != EGlobalCameraControlMode::ThirdPerson
	)
	{
		const FRotator CameraRotation = GetActorRotation();

		CurrentState.Rotation.Rotation = CameraRotation;
		TargetState.Rotation.Rotation = CameraRotation;

		CurrentState.Orbit.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
		CurrentState.Orbit.OrbitPitch = FMath::Clamp(
			CameraRotation.Pitch,
			CurrentState.Orbit.MinPitch,
			CurrentState.Orbit.MaxPitch
		);

		TargetState.Orbit.OrbitYaw = CurrentState.Orbit.OrbitYaw;
		TargetState.Orbit.OrbitPitch = CurrentState.Orbit.OrbitPitch;
	}

	TargetState.Aim.bLookAtTarget = bEnabled;
	TargetState.Aim.LookMode = NewLookMode;

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetFieldOfView(float NewFieldOfView, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Lens.FieldOfView = FMath::Clamp(NewFieldOfView, 5.f, 170.f);
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetMovementSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Movement.MovementSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetRotationSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Movement.RotationSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetBlendSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Movement.BlendSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetControlMode(
	EGlobalCameraControlMode NewMode,
	EGlobalCameraTransitionMode Transition)
{
	TargetState.Control.ControlMode = NewMode;
	SetTransitionMode(Transition);
	PushStateToComponents();
}

void ASystemCamera::SetTargetMouseSensitivity(float NewSensitivity, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Control.MouseSensitivity = FMath::Max(NewSensitivity, 0.f);
	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetOrbitSettings(
	float NewYaw,
	float NewPitch,
	float NewDistance,
	float NewMinPitch,
	float NewMaxPitch,
	EGlobalCameraTransitionMode NewMode)
{
	TargetState.Orbit.OrbitYaw = FRotator::NormalizeAxis(NewYaw);
	TargetState.Orbit.MinPitch = FMath::Clamp(NewMinPitch, -89.f, 89.f);
	TargetState.Orbit.MaxPitch = FMath::Clamp(NewMaxPitch, TargetState.Orbit.MinPitch, 89.f);
	TargetState.Orbit.OrbitPitch = FMath::Clamp(
		NewPitch,
		TargetState.Orbit.MinPitch,
		TargetState.Orbit.MaxPitch
	);
	TargetState.Orbit.OrbitDistance = FMath::Max(NewDistance, 1.f);

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetAimSettings(
	float NewDistance,
	float NewHeight,
	EGlobalCameraTransitionMode NewMode)
{
	TargetState.Aim.AimDistance = FMath::Max(NewDistance, 0.f);
	TargetState.Aim.AimHeight = NewHeight;

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::SetTargetInvertThirdPersonPitch(
	bool bInvert,
	EGlobalCameraTransitionMode NewMode)
{
	TargetState.Orbit.bInvertThirdPersonPitch = bInvert;
	CurrentState.Orbit.bInvertThirdPersonPitch = bInvert;

	SetTransitionMode(NewMode);
	PushStateToComponents();
}

void ASystemCamera::UpdateCameraRotation(float LookX, float LookY)
{
	if (!StateComponent || !TargetComponent)
	{
		return;
	}

	PushStateToComponents();

	FGlobalCameraState& Current = StateComponent->CurrentState;
	FGlobalCameraState& Target = StateComponent->TargetState;

	if (
		Target.Control.ControlMode == EGlobalCameraControlMode::None ||
		Target.Rotation.bUseTargetRotation ||
		(
			Target.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson &&
			Target.Aim.bLookAtTarget &&
			IsValid(TargetComponent->GetConnectTarget())
		) ||
		(FMath::IsNearlyZero(LookX) && FMath::IsNearlyZero(LookY))
	)
	{
		return;
	}

	if (Target.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson)
	{
		if (OrbitComponent)
		{
			OrbitComponent->UpdateRotationInput(
				Target,
				Current,
				TargetComponent->GetConnectTarget(),
				LookX,
				LookY
			);
		}
	}
	else if (Target.Control.ControlMode == EGlobalCameraControlMode::FirstPerson)
	{
		if (FirstPersonComponent)
		{
			FirstPersonComponent->UpdateRotationInput(Target, LookX, LookY);
		}
	}
	PullStateFromComponents();
}

void ASystemCamera::UpdateCameraTransform(float DeltaTime)
{
	if (
		!StateComponent ||
		!TargetComponent ||
		!PositionComponent ||
		!RotationComponent
	)
	{
		return;
	}

	FGlobalCameraState& Current = StateComponent->CurrentState;
	FGlobalCameraState& Target = StateComponent->TargetState;

	AActor* FollowTarget = TargetComponent->GetFollowTarget(Current.Target.FollowTarget);
	AActor* Player = TargetComponent->GetPlayerTarget();
	AActor* Connect = TargetComponent->GetConnectTarget();

	FVector DesiredLocation = PositionComponent->GetFixedWorldPosition();
	AActor* CollisionTarget = FollowTarget;
	FVector CollisionOrigin = GetActorLocation();

	if (Current.Target.FollowTarget != EGlobalCameraFollowTarget::None)
	{
		bool bPositionCalculated = false;

		if (Current.Control.ControlMode == EGlobalCameraControlMode::FirstPerson && FirstPersonComponent)
		{
			bPositionCalculated = FirstPersonComponent->CalculateDesiredLocation(
				Current,
				FollowTarget,
				DesiredLocation
			);

			if (IsValid(FollowTarget))
			{
				CollisionOrigin = FollowTarget->GetActorLocation();
			}
		}
		else if (Current.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson && OrbitComponent)
		{
			bPositionCalculated = OrbitComponent->CalculateDesiredLocation(
				Current,
				FollowTarget,
				Player,
				Connect,
				DesiredLocation
			);

			AActor* OrbitTarget = FollowTarget;

			if (
				Current.Target.FollowTarget == EGlobalCameraFollowTarget::ConnectTarget &&
				IsValid(Player)
			)
			{
				OrbitTarget = Player;
			}

			if (IsValid(OrbitTarget))
			{
				CollisionTarget = OrbitTarget;
				CollisionOrigin = OrbitTarget->GetActorLocation()
					+ FVector::UpVector * Current.Aim.AimHeight;
			}
		}
		else if (IsValid(FollowTarget))
		{
			DesiredLocation = FollowTarget->GetActorLocation() + Current.Position.Position;
			bPositionCalculated = true;
			CollisionOrigin = FollowTarget->GetActorLocation();
		}

		if (!bPositionCalculated)
		{
			DesiredLocation = PositionComponent->GetFixedWorldPosition();
		}
	}

	if (Current.Position.bUseCollision && CollisionComponent && GetWorld())
	{
		DesiredLocation = CollisionComponent->ResolveCameraPosition(
			CollisionOrigin,
			DesiredLocation,
			CollisionTarget
		);
	}

	bool bShouldUpdateStateRotation = false;

	const FRotator DesiredRotation = RotationComponent->CalculateDesiredRotation(
		Current,
		DesiredLocation,
		FollowTarget,
		Player,
		Connect,
		bShouldUpdateStateRotation
	);

	if (bShouldUpdateStateRotation)
	{
		Current.Rotation.Rotation = DesiredRotation;
		Target.Rotation.Rotation = DesiredRotation;
	}

	const bool bUseTargetRotationSpeed =
		Current.Aim.bLookAtTarget || Current.Rotation.bUseTargetRotation;

	const float RotationSpeed = bUseTargetRotationSpeed
		? FMath::Max(Target.Movement.RotationSpeed, 2.f)
		: Current.Movement.RotationSpeed;

	if (
		Current.Control.ControlMode == EGlobalCameraControlMode::FirstPerson &&
		Current.Position.Position.Equals(Target.Position.Position, 3.f)
	)
	{
		const float MoveSpeed = FMath::Max(Current.Movement.MovementSpeed * 2.f, 40.f);
		const float PositionTolerance = 5.f;

		FVector NewLocation = FMath::VInterpTo(
			GetActorLocation(),
			DesiredLocation,
			DeltaTime,
			MoveSpeed
		);

		if (FVector::DistSquared(NewLocation, DesiredLocation) <= FMath::Square(PositionTolerance))
		{
			NewLocation = DesiredLocation;
		}

		const bool bDirectMouseControl =
			!Current.Rotation.bUseTargetRotation && !Current.Aim.bLookAtTarget;

		const FRotator NewRotation = bDirectMouseControl
			? Current.Rotation.Rotation
			: FMath::RInterpConstantTo(
				GetActorRotation(),
				DesiredRotation,
				DeltaTime,
				FMath::Max(Current.Movement.RotationSpeed, 0.01f) * 30.f
			);

		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
	else
	{
		if (
			StateComponent->TransitionMode == EGlobalCameraTransitionMode::Instant ||
			DeltaTime <= 0.f ||
			(!bUseTargetRotationSpeed && RotationSpeed <= 0.f) ||
			Current.Movement.MovementSpeed <= 0.f
		)
		{
			SetActorLocationAndRotation(DesiredLocation, DesiredRotation);
		}
		else
		{
			const FVector NewLocation = FMath::VInterpTo(
				GetActorLocation(),
				DesiredLocation,
				DeltaTime,
				FMath::Max(Current.Movement.MovementSpeed, 0.01f)
			);

			const bool bDirectMouseControl =
				!Current.Rotation.bUseTargetRotation && !Current.Aim.bLookAtTarget;

			const FRotator NewRotation = bDirectMouseControl
				? Current.Rotation.Rotation
				: FMath::RInterpConstantTo(
					GetActorRotation(),
					DesiredRotation,
					DeltaTime,
					FMath::Max(RotationSpeed, 0.01f) * 30.f
				);

			SetActorLocationAndRotation(NewLocation, NewRotation);
		}
	}

	if (CameraComponent)
	{
		CameraComponent->SetFieldOfView(Current.Lens.FieldOfView);
	}
}

AActor* ASystemCamera::GetTarget() const
{
	return TargetComponent ? TargetComponent->GetTarget() : nullptr;
}

AActor* ASystemCamera::GetPositionTarget(EGlobalCameraPositionMode PositionMode) const
{
	return TargetComponent ? TargetComponent->GetPositionTarget(PositionMode) : nullptr;
}

AActor* ASystemCamera::GetFollowTarget() const
{
	return TargetComponent
		? TargetComponent->GetFollowTarget(CurrentState.Target.FollowTarget)
		: nullptr;
}

AActor* ASystemCamera::GetRotationTarget() const
{
	return TargetComponent
		? TargetComponent->GetRotationTarget(
			CurrentState.Target.FollowTarget,
			CurrentState.Rotation.bUseTargetRotation
		)
		: nullptr;
}

void ASystemCamera::SetPlayerTarget(AActor* NewTarget)
{
	PlayerTarget = NewTarget;

	if (TargetComponent)
	{
		TargetComponent->SetPlayerTarget(NewTarget);
	}
}

void ASystemCamera::SetConnectTarget(AActor* NewTarget)
{
	ConnectTarget = NewTarget;

	if (TargetComponent)
	{
		TargetComponent->SetConnectTarget(NewTarget);
	}
}

FVector ASystemCamera::GetWorldPositionFromState(const FGlobalCameraState& State) const
{
	if (!PositionComponent)
	{
		return State.Position.Position;
	}

	return PositionComponent->GetWorldPositionFromState(
		State,
		GetPositionTarget(State.Position.PositionMode)
	);
}

FVector ASystemCamera::GetRelativePositionFromWorld(
	const FVector& WorldPosition,
	EGlobalCameraPositionMode PositionMode) const
{
	if (!PositionComponent)
	{
		return WorldPosition;
	}

	return PositionComponent->GetRelativePositionFromWorld(
		WorldPosition,
		GetPositionTarget(PositionMode)
	);
}

void ASystemCamera::ConvertPositionSpace(EGlobalCameraPositionMode NewPositionMode)
{
	if (!PositionComponent || !StateComponent)
	{
		return;
	}

	PositionComponent->ConvertPositionSpace(
		StateComponent->CurrentState,
		NewPositionMode,
		GetPositionTarget(NewPositionMode)
	);

	PullStateFromComponents();
}