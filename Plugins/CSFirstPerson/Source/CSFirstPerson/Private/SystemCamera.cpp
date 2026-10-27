// _________________________________________
// includes
#include "SystemCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
// _________________________________________
// Base
ASystemCamera::ASystemCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(Root);
}
void ASystemCamera::BeginPlay()
{
	Super::BeginPlay();
	if (!PlayerTarget)
	{
		PlayerTarget = GetOwner();
	}
	TargetState = CurrentState;
	FixedWorldPosition = GetWorldPositionFromState(CurrentState);
	if (CameraComponent)
	{
		CameraComponent->SetRelativeLocation(FVector::ZeroVector);
		CameraComponent->SetRelativeRotation(FRotator::ZeroRotator);
		CameraComponent->SetFieldOfView(CurrentState.FieldOfView);
	}
}
void ASystemCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateBlend(DeltaTime);
	UpdateCameraTransform(DeltaTime);
}
// _________________________________________
// Target state setters
void ASystemCamera::SetTransitionMode(EGlobalCameraTransitionMode NewMode)
{
	TransitionMode = NewMode;
}
void ASystemCamera::SetCameraState(const FGlobalCameraState& NewState, EGlobalCameraTransitionMode NewTransitionMode)
{ // Set the target state and transition mode
	TargetState = NewState;
	SetTransitionMode(NewTransitionMode);
	if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None)
	{
		FixedWorldPosition = GetWorldPositionFromState(TargetState);
	}
}
void ASystemCamera::SetTargetPosition(FVector NewPosition, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Position = NewPosition;
	SetTransitionMode(NewMode);
	if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None)
	{
		FixedWorldPosition = GetWorldPositionFromState(TargetState);
	}
}
void ASystemCamera::SetTargetPositionMode(EGlobalCameraPositionMode NewMode, EGlobalCameraTransitionMode Transition)
{
	TargetState.PositionMode = NewMode;
	SetTransitionMode(Transition);
	if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None)
	{
		FixedWorldPosition = GetWorldPositionFromState(TargetState);
	}
}
void ASystemCamera::SetTargetFollow(EGlobalCameraFollowTarget NewTarget, EGlobalCameraTransitionMode NewMode)
{
	TargetState.FollowTarget = NewTarget;
	SetTransitionMode(NewMode);
	if (NewTarget == EGlobalCameraFollowTarget::None)
	{
		FixedWorldPosition = GetWorldPositionFromState(TargetState);
	}
}
void ASystemCamera::SetTargetCollision(bool bEnabled, EGlobalCameraTransitionMode NewMode)
{
	TargetState.bUseCollision = bEnabled;
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotation(FRotator NewRotation, EGlobalCameraTransitionMode NewMode)
{
	TargetState.Rotation = NewRotation;
	TargetState.OrbitYaw = NewRotation.Yaw;
	TargetState.OrbitPitch = NewRotation.Pitch;
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotationSettings(bool bUseTargetRotation, bool bUsePathRotation, float NewLockOnRotation, EGlobalCameraTransitionMode NewMode)
{
	TargetState.bUseTargetRotation = bUseTargetRotation;
	TargetState.bUsePathRotation = bUsePathRotation;
	TargetState.LockOnRotation = NewLockOnRotation;
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetLookAt(bool bEnabled, EGlobalCameraLookMode NewLookMode, EGlobalCameraTransitionMode NewMode)
{
	TargetState.bLookAtTarget = bEnabled;
	TargetState.LookMode = NewLookMode;
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetFieldOfView(float NewFieldOfView, EGlobalCameraTransitionMode NewMode)
{
	TargetState.FieldOfView = FMath::Clamp(NewFieldOfView, 5.f, 170.f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetMovementSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.MovementSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotationSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.RotationSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetBlendSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode)
{
	TargetState.BlendSpeed = FMath::Max(NewSpeed, 0.01f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetControlMode(EGlobalCameraControlMode NewMode, EGlobalCameraTransitionMode Transition)
{
	TargetState.ControlMode = NewMode;
	SetTransitionMode(Transition);
}
void ASystemCamera::SetTargetMouseSensitivity(float NewSensitivity, EGlobalCameraTransitionMode NewMode)
{
	TargetState.MouseSensitivity = FMath::Max(NewSensitivity, 0.f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetOrbitSettings(float NewYaw, float NewPitch, float NewDistance, float NewMinPitch, float NewMaxPitch, EGlobalCameraTransitionMode NewMode)
{
	TargetState.OrbitYaw = FRotator::NormalizeAxis(NewYaw);
	TargetState.MinPitch = FMath::Clamp(NewMinPitch, -89.f, 89.f);
	TargetState.MaxPitch = FMath::Clamp(NewMaxPitch, TargetState.MinPitch, 89.f);
	TargetState.OrbitPitch = FMath::Clamp(NewPitch, TargetState.MinPitch, TargetState.MaxPitch);
	TargetState.OrbitDistance = FMath::Max(NewDistance, 1.f);
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetAimSettings(float NewDistance, float NewHeight, EGlobalCameraTransitionMode NewMode)
{
	TargetState.AimDistance = FMath::Max(NewDistance, 0.f);
	TargetState.AimHeight = NewHeight;
	SetTransitionMode(NewMode);
}
void ASystemCamera::SetConnectTarget(AActor* NewTarget)
{
	ConnectTarget = NewTarget;
}
// _________________________________________
// Update functions
void ASystemCamera::UpdateBlend(float DeltaTime)
{
	UpdateDiscreteSettings();
	UpdatePositionState(DeltaTime);
	UpdateRotationState(DeltaTime);
	UpdateCameraSettings(DeltaTime);
	UpdateControlSettings(DeltaTime);
}
void ASystemCamera::UpdateDiscreteSettings()
{
	CurrentState.PositionMode = TargetState.PositionMode;
	CurrentState.FollowTarget = TargetState.FollowTarget;
	CurrentState.bUseCollision = TargetState.bUseCollision;
	CurrentState.bUseTargetRotation = TargetState.bUseTargetRotation;
	CurrentState.bUsePathRotation = TargetState.bUsePathRotation;
	CurrentState.bLookAtTarget = TargetState.bLookAtTarget;
	CurrentState.ControlMode = TargetState.ControlMode;
	CurrentState.LookMode = TargetState.LookMode;
}
void ASystemCamera::UpdatePositionState(float DeltaTime)
{
	if (TransitionMode == EGlobalCameraTransitionMode::Instant)
	{
		CurrentState.Position = TargetState.Position;
		return;
	}
	const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
	CurrentState.Position = FMath::VInterpTo(CurrentState.Position, TargetState.Position, DeltaTime, Speed);
	/*if (CurrentState.Position.Equals(TargetState.Position, 0.01f))
	{
		CurrentState.Position = TargetState.Position;
	}*/
}
void ASystemCamera::UpdateRotationState(float DeltaTime)
{
	if (TransitionMode == EGlobalCameraTransitionMode::Instant)
	{
		CurrentState.Rotation = TargetState.Rotation;
		return;
	}
	if (CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson && CurrentState.Position.Equals(TargetState.Position, 0.1f))
	{
		//CurrentState.Rotation = TargetState.Rotation;
		const float Speed = FMath::Max(CurrentState.RotationSpeed * 3.f, 20.f);
		CurrentState.Rotation = FMath::RInterpTo(CurrentState.Rotation, TargetState.Rotation, DeltaTime, Speed);
		return;
	}
	const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
	CurrentState.Rotation = FMath::RInterpTo(CurrentState.Rotation, TargetState.Rotation, DeltaTime, Speed);
	/*if (CurrentState.Rotation.Equals(TargetState.Rotation, 0.01f))
	{
		CurrentState.Rotation = TargetState.Rotation;
	}*/
}
void ASystemCamera::UpdateCameraSettings(float DeltaTime)
{
	const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
	auto UpdateFloat = [this, DeltaTime, Speed](float& Current, float Target)
		{
			if (TransitionMode == EGlobalCameraTransitionMode::Instant)
			{
				Current = Target;
				return;
			}
			Current = FMath::FInterpTo(Current, Target, DeltaTime, Speed);
			if (FMath::IsNearlyEqual(Current, Target, 0.01f))
			{
				Current = Target;
			}
		};
	UpdateFloat(CurrentState.MovementSpeed, TargetState.MovementSpeed);
	UpdateFloat(CurrentState.RotationSpeed, TargetState.RotationSpeed);
	UpdateFloat(CurrentState.FieldOfView, TargetState.FieldOfView);
	UpdateFloat(CurrentState.BlendSpeed, TargetState.BlendSpeed);
}
void ASystemCamera::UpdateControlSettings(float DeltaTime)
{
	const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
	auto UpdateFloat = [this, DeltaTime, Speed](float& Current, float Target)
		{
			if (Current == Target)
			{
				return;
			}
			/*if (((Current == CurrentState.OrbitYaw || Current == CurrentState.OrbitPitch) && (Target == TargetState.OrbitYaw || Target == TargetState.OrbitPitch) ) && CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson && CurrentState.Rotation.Equals(TargetState.Rotation, 5.f))
			{
				Current = Target;
				return;
			}*/
			if (TransitionMode == EGlobalCameraTransitionMode::Instant)
			{
				Current = Target;
				return;
			}
			Current = FMath::FInterpTo(Current, Target, DeltaTime, Speed);
			if (FMath::IsNearlyEqual(Current, Target, 0.01f))
			{
				Current = Target;
			}
		};
	UpdateFloat(CurrentState.MouseSensitivity, TargetState.MouseSensitivity);
	UpdateFloat(CurrentState.OrbitYaw, TargetState.OrbitYaw);
	UpdateFloat(CurrentState.OrbitPitch, TargetState.OrbitPitch);
	UpdateFloat(CurrentState.MinPitch, TargetState.MinPitch);
	UpdateFloat(CurrentState.MaxPitch, TargetState.MaxPitch);
	UpdateFloat(CurrentState.OrbitDistance, TargetState.OrbitDistance);
	UpdateFloat(CurrentState.AimDistance, TargetState.AimDistance);
	UpdateFloat(CurrentState.AimHeight, TargetState.AimHeight);
	UpdateFloat(CurrentState.LockOnRotation, TargetState.LockOnRotation);
}
void ASystemCamera::UpdateCameraRotation(float LookX, float LookY)
{
	if (TargetState.bUseTargetRotation)
	{
		if (AActor* RotationTarget = GetRotationTarget())
		{
			TargetState.Rotation = RotationTarget->GetActorRotation();
			TargetState.Rotation.Yaw += TargetState.LockOnRotation;
		}
		return;
	}
	if (CurrentState.ControlMode == EGlobalCameraControlMode::None) // || !(CurrentState.Rotation.Equals(TargetState.Rotation, 0.1f) && CurrentState.Position.Equals(TargetState.Position, 0.1f))
	{
		return;
	}
	if (FMath::IsNearlyZero(LookX) && FMath::IsNearlyZero(LookY))
	{
		return;
	}
	
	TargetState.OrbitYaw = FRotator::NormalizeAxis(TargetState.OrbitYaw + LookX * TargetState.MouseSensitivity);
	TargetState.OrbitPitch = FMath::Clamp(
		TargetState.OrbitPitch + LookY * TargetState.MouseSensitivity,
		TargetState.MinPitch,
		TargetState.MaxPitch
	);
	TargetState.Rotation.Yaw = TargetState.OrbitYaw;
	TargetState.Rotation.Pitch = TargetState.OrbitPitch;
	
	//TargetState.bLookAtTarget = false;
	//TargetState.bUseTargetRotation = false;
}
void ASystemCamera::UpdateCameraTransform(float DeltaTime)
{
	const FVector DesiredLocation = CalculateDesiredLocation();
	FRotator DesiredRotation = CurrentState.Rotation;

	if (CurrentState.bUseTargetRotation)
	{
		if (AActor* RotationTarget = GetRotationTarget())
		{
			DesiredRotation = RotationTarget->GetActorRotation();
			DesiredRotation.Yaw += CurrentState.LockOnRotation;
		}
	}
	else if (CurrentState.bLookAtTarget && PlayerTarget)
	{
		DesiredRotation = UKismetMathLibrary::FindLookAtRotation(
			DesiredLocation,
			PlayerTarget->GetActorLocation()
		);
	}
	if (CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson && CurrentState.Position.Equals(TargetState.Position, 3.f))
	{
		
		const float MoveSpeed = FMath::Max(CurrentState.MovementSpeed * 2.f, 40.f);
		const float PositionTolerance = 5.0f;

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
		SetActorLocationAndRotation(NewLocation, DesiredRotation);
	}
	else
	{
		if (TransitionMode == EGlobalCameraTransitionMode::Instant || DeltaTime <= 0.f || (CurrentState.RotationSpeed <= 0.f || CurrentState.MovementSpeed <= 0.f))
		{
			SetActorLocationAndRotation(DesiredLocation, DesiredRotation);
		}
		else
		{
			const FVector NewLocation = FMath::VInterpTo(
				GetActorLocation(),
				DesiredLocation,
				DeltaTime,
				FMath::Max(CurrentState.MovementSpeed, 0.01f)
			);
			const bool bDirectMouseControl = !CurrentState.bUseTargetRotation && !CurrentState.bLookAtTarget;
			/*const FRotator NewRotation = bDirectMouseControl
				? CurrentState.Rotation
				: FMath::RInterpTo(
					GetActorRotation(),
					DesiredRotation,
					DeltaTime,
					FMath::Max(CurrentState.RotationSpeed, 0.1f)
				);*/
			const FRotator NewRotation = bDirectMouseControl
				? CurrentState.Rotation
				: FMath::RInterpConstantTo(
					GetActorRotation(),
					DesiredRotation,
					DeltaTime,
					FMath::Max(CurrentState.RotationSpeed, 0.01f) * 30.f
				);
			
			SetActorLocationAndRotation(NewLocation, NewRotation);
		}
	}

	if (CameraComponent)
	{
		CameraComponent->SetFieldOfView(CurrentState.FieldOfView);
	}
}
// _________________________________________
// Target getters
AActor* ASystemCamera::GetTarget() const
{
	return PlayerTarget ? PlayerTarget : ConnectTarget;
}
AActor* ASystemCamera::GetPositionTarget(EGlobalCameraPositionMode PositionMode) const
{
	switch (PositionMode)
	{
	case EGlobalCameraPositionMode::PlayerTarget:
		return PlayerTarget;
	case EGlobalCameraPositionMode::ConnectTarget:
		return ConnectTarget;
	default:
		return nullptr;
	}
}
AActor* ASystemCamera::GetFollowTarget() const
{
	switch (CurrentState.FollowTarget)
	{
	case EGlobalCameraFollowTarget::PlayerTarget:
		return PlayerTarget;
	case EGlobalCameraFollowTarget::ConnectTarget:
		return ConnectTarget;
	default:
		return nullptr;
	}
}
AActor* ASystemCamera::GetRotationTarget() const
{
	return CurrentState.bUseTargetRotation ? GetFollowTarget() : nullptr;
}
void ASystemCamera::SetPlayerTarget(AActor* NewTarget)
{
	PlayerTarget = NewTarget;
}
FVector ASystemCamera::GetWorldPositionFromState(const FGlobalCameraState& State) const
{
	if (State.PositionMode == EGlobalCameraPositionMode::World)
	{
		return State.Position;
	}
	if (AActor* Target = GetPositionTarget(State.PositionMode))
	{
		return Target->GetActorTransform().TransformPosition(State.Position);
	}
	return State.Position;
}
FVector ASystemCamera::GetRelativePositionFromWorld(const FVector& WorldPosition, EGlobalCameraPositionMode PositionMode) const
{
	if (AActor* Target = GetPositionTarget(PositionMode))
	{
		return Target->GetActorTransform().InverseTransformPosition(WorldPosition);
	}
	return WorldPosition;
}
// _________________________________________
// Position space conversion
void ASystemCamera::ConvertPositionSpace(EGlobalCameraPositionMode NewPositionMode)
{
	const FVector WorldPosition = GetActorLocation();
	CurrentState.Position = GetRelativePositionFromWorld(WorldPosition, NewPositionMode);
	CurrentState.Rotation = GetActorRotation();
	if (CurrentState.FollowTarget == EGlobalCameraFollowTarget::None)
	{
		FixedWorldPosition = WorldPosition;
	}
}
// _________________________________________
// Desired location calculation
FVector ASystemCamera::CalculateDesiredLocation() const
{
	if (CurrentState.FollowTarget == EGlobalCameraFollowTarget::None)
	{
		return FixedWorldPosition;
	}

	AActor* FollowTarget = GetFollowTarget();
	if (!FollowTarget)
	{
		return FixedWorldPosition;
	}

	const FVector TargetLocation = FollowTarget->GetActorLocation();
	const FVector DesiredLocation = TargetLocation + CurrentState.Position;

	if (CurrentState.PositionMode != EGlobalCameraPositionMode::PlayerTarget ||
		CurrentState.FollowTarget != EGlobalCameraFollowTarget::PlayerTarget ||
		!CurrentState.bUseCollision)
	{
		return DesiredLocation;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SystemCameraCollision), false);
	Params.AddIgnoredActor(FollowTarget);
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TargetLocation,
		DesiredLocation,
		FQuat::Identity,
		ECC_Camera,
		FCollisionShape::MakeSphere(15.f),
		Params
	);

	return bHit ? Hit.Location + Hit.Normal * 15.f : DesiredLocation;
}
