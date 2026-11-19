#include "Camera/FirstPersonCameraComponent.h"
#include "GameFramework/Actor.h"

UFirstPersonCameraComponent::UFirstPersonCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFirstPersonCameraComponent::UpdateRotationInput(FGlobalCameraState& TargetState, float LookX, float LookY) const
{
	if (TargetState.Control.ControlMode != EGlobalCameraControlMode::FirstPerson)
	{
		return;
	}

	if (TargetState.Rotation.bUseTargetRotation || TargetState.Aim.bLookAtTarget)
	{
		return;
	}

	if (FMath::IsNearlyZero(LookX) && FMath::IsNearlyZero(LookY))
	{
		return;
	}

	const float Sensitivity = TargetState.Control.MouseSensitivity;

	TargetState.Orbit.OrbitYaw = FRotator::NormalizeAxis(
	TargetState.Orbit.OrbitYaw + LookX * Sensitivity
	);
	TargetState.Orbit.OrbitPitch = FMath::Clamp(
		TargetState.Orbit.OrbitPitch + LookY * Sensitivity,
		TargetState.Orbit.MinPitch,
		TargetState.Orbit.MaxPitch
	);

	TargetState.Rotation.Rotation.Yaw = TargetState.Orbit.OrbitYaw;
	TargetState.Rotation.Rotation.Pitch = TargetState.Orbit.OrbitPitch;
}

bool UFirstPersonCameraComponent::CalculateDesiredLocation(
	const FGlobalCameraState& State,
	AActor* FollowTarget,
	FVector& OutDesiredLocation) const
{
	if (!IsValid(FollowTarget))
	{
		return false;
	}

	OutDesiredLocation = FollowTarget->GetActorLocation() + State.Position.Position;
	return true;
}