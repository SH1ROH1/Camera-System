#include "Camera/CameraStateComponent.h"
#include "GameFramework/Actor.h"

UCameraStateComponent::UCameraStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCameraStateComponent::SetTargetState(
    const FGlobalCameraState& NewState,
    EGlobalCameraTransitionMode NewTransitionMode
)
{
    const bool bWasLookingAtTarget =
        TargetState.Aim.bLookAtTarget && !NewState.Aim.bLookAtTarget;

    const bool bSameControlMode =
        CurrentState.Control.ControlMode == NewState.Control.ControlMode;

    FGlobalCameraState StateToApply = NewState;

    if (!bSameControlMode)
    {
        const FRotator CameraRotation = GetOwner()->GetActorRotation();

        StateToApply.Rotation.Rotation = CameraRotation;
        StateToApply.Orbit.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
        StateToApply.Orbit.OrbitPitch = FMath::Clamp(
            CameraRotation.Pitch,
            StateToApply.Orbit.MinPitch,
            StateToApply.Orbit.MaxPitch
        );
    }
    else if (NewState.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson)
    {
        StateToApply.Orbit.OrbitYaw = TargetState.Orbit.OrbitYaw;
        StateToApply.Orbit.OrbitPitch = TargetState.Orbit.OrbitPitch;
    }

    if (
        bWasLookingAtTarget &&
        NewState.Control.ControlMode == EGlobalCameraControlMode::FirstPerson
    )
    {
        const FRotator CameraRotation = GetOwner()->GetActorRotation();

        StateToApply.Rotation.Rotation = CameraRotation;
        StateToApply.Orbit.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
        StateToApply.Orbit.OrbitPitch = FMath::Clamp(
            CameraRotation.Pitch,
            StateToApply.Orbit.MinPitch,
            StateToApply.Orbit.MaxPitch
        );
    }

    TargetState = StateToApply;
    CurrentState.Orbit.bInvertThirdPersonPitch =
        StateToApply.Orbit.bInvertThirdPersonPitch;

    SetTransitionMode(NewTransitionMode);
}

void UCameraStateComponent::SetTransitionMode(EGlobalCameraTransitionMode NewMode)
{
    TransitionMode = NewMode;
}

void UCameraStateComponent::UpdateDiscreteSettings()
{
    CurrentState.Position.PositionMode = TargetState.Position.PositionMode;
    CurrentState.Target.FollowTarget = TargetState.Target.FollowTarget;
    CurrentState.Position.bUseCollision = TargetState.Position.bUseCollision;

    CurrentState.Rotation.bUseTargetRotation =
        TargetState.Rotation.bUseTargetRotation;

    CurrentState.Rotation.bUsePathRotation =
        TargetState.Rotation.bUsePathRotation;

    CurrentState.Aim.bLookAtTarget = TargetState.Aim.bLookAtTarget;
    CurrentState.Control.ControlMode = TargetState.Control.ControlMode;
    CurrentState.Aim.LookMode = TargetState.Aim.LookMode;

    CurrentState.Orbit.bInvertThirdPersonPitch =
        TargetState.Orbit.bInvertThirdPersonPitch;
}