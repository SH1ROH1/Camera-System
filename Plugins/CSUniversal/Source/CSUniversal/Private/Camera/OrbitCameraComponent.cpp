#include "Camera/OrbitCameraComponent.h"
#include "GameFramework/Actor.h"

UOrbitCameraComponent::UOrbitCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOrbitCameraComponent::UpdateRotationInput(
	FGlobalCameraState& TargetState,
	const FGlobalCameraState& CurrentState,
	AActor* ConnectTarget,
	float LookX,
	float LookY) const
{
	if (TargetState.Control.ControlMode != EGlobalCameraControlMode::ThirdPerson)
	{
		return;
	}

	if (TargetState.Rotation.bUseTargetRotation)
	{
		return;
	}

	if (TargetState.Aim.bLookAtTarget && IsValid(ConnectTarget))
	{
		return;
	}

	if (FMath::IsNearlyZero(LookX) && FMath::IsNearlyZero(LookY))
	{
		return;
	}

	const float Sensitivity = TargetState.Control.MouseSensitivity;
	const float PitchDirection = CurrentState.Orbit.bInvertThirdPersonPitch ? -1.f : 1.f;
	
	TargetState.Orbit.OrbitYaw += LookX * Sensitivity;
	TargetState.Orbit.OrbitPitch = FMath::Clamp(
		TargetState.Orbit.OrbitPitch + LookY * Sensitivity * PitchDirection,
		TargetState.Orbit.MinPitch,
		TargetState.Orbit.MaxPitch
	);
}

float UOrbitCameraComponent::CalculateOrbitYaw(
	const FGlobalCameraState& State,
	AActor* PlayerTarget,
	AActor* ConnectTarget) const
{
	if (!IsValid(PlayerTarget) || !IsValid(ConnectTarget))
	{
		return State.Orbit.OrbitYaw;
	}

	FVector Direction = ConnectTarget->GetActorLocation() - PlayerTarget->GetActorLocation();
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero())
	{
		return State.Orbit.OrbitYaw;
	}

	return Direction.Rotation().Yaw;
}

bool UOrbitCameraComponent::CalculateDesiredLocation(
	const FGlobalCameraState& State,
	AActor* FollowTarget,
	AActor* PlayerTarget,
	AActor* ConnectTarget,
	FVector& OutDesiredLocation) const
{
	AActor* OrbitTarget = FollowTarget;

	if (State.Target.FollowTarget == EGlobalCameraFollowTarget::ConnectTarget && IsValid(PlayerTarget))
	{
		OrbitTarget = PlayerTarget;
	}

	if (!IsValid(OrbitTarget))
	{
		return false;
	}

	const FVector OrbitCenter = OrbitTarget->GetActorLocation()
		+ FVector::UpVector * State.Aim.AimHeight;

	const float OrbitYaw = CalculateOrbitYaw(State, PlayerTarget, ConnectTarget);
	const FRotator OrbitRotation(State.Orbit.OrbitPitch, OrbitYaw, 0.f);
	const float OrbitDistance = FMath::Max(State.Orbit.OrbitDistance, 1.f);

	OutDesiredLocation = OrbitCenter - OrbitRotation.Vector() * OrbitDistance;

	return true;
}