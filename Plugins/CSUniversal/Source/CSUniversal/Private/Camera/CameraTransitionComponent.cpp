#include "Camera/CameraTransitionComponent.h"

UCameraTransitionComponent::UCameraTransitionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCameraTransitionComponent::UpdateTransition(
    FGlobalCameraState& CurrentState,
    const FGlobalCameraState& TargetState,
    EGlobalCameraTransitionMode TransitionMode,
    float DeltaTime
)
{
    if (DeltaTime <= 0.f)
    {
        return;
    }

    UpdatePositionState(
        CurrentState,
        TargetState,
        TransitionMode,
        DeltaTime
    );

    UpdateRotationState(
        CurrentState,
        TargetState,
        TransitionMode,
        DeltaTime
    );

    UpdateCameraSettings(
        CurrentState,
        TargetState,
        TransitionMode,
        DeltaTime
    );

    UpdateControlSettings(
        CurrentState,
        TargetState,
        TransitionMode,
        DeltaTime
    );
}

void UCameraTransitionComponent::UpdatePositionState(
    FGlobalCameraState& CurrentState,
    const FGlobalCameraState& TargetState,
    EGlobalCameraTransitionMode TransitionMode,
    float DeltaTime
)
{
    if (TransitionMode == EGlobalCameraTransitionMode::Instant)
    {
        CurrentState.Position.Position = TargetState.Position.Position;
        return;
    }

    const float Speed = FMath::Max(
        CurrentState.Movement.BlendSpeed,
        0.01f
    );

    CurrentState.Position.Position = FMath::VInterpTo(
        CurrentState.Position.Position,
        TargetState.Position.Position,
        DeltaTime,
        Speed
    );
}

void UCameraTransitionComponent::UpdateRotationState(
    FGlobalCameraState& CurrentState,
    const FGlobalCameraState& TargetState,
    EGlobalCameraTransitionMode TransitionMode,
    float DeltaTime
)
{
    if (TransitionMode == EGlobalCameraTransitionMode::Instant)
    {
        CurrentRotationSpeed = 0.f;
        CurrentState.Rotation.Rotation = TargetState.Rotation.Rotation;
        return;
    }

    const bool bFirstPersonReady =
        CurrentState.Control.ControlMode == EGlobalCameraControlMode::FirstPerson &&
        CurrentState.Position.Position.Equals(
            TargetState.Position.Position,
            0.1f
        );

    if (bFirstPersonReady)
    {
        constexpr float MaxRotationSpeed = 100.f;
        constexpr float RotationAcceleration = 250.f;

        CurrentRotationSpeed = FMath::FInterpConstantTo(
            CurrentRotationSpeed,
            MaxRotationSpeed,
            DeltaTime,
            RotationAcceleration
        );

        CurrentState.Rotation.Rotation = FMath::RInterpTo(
            CurrentState.Rotation.Rotation,
            TargetState.Rotation.Rotation,
            DeltaTime,
            FMath::Max(CurrentRotationSpeed, 0.01f)
        );

        return;
    }

    CurrentRotationSpeed = 0.f;

    const float Speed = FMath::Max(
        CurrentState.Movement.BlendSpeed,
        0.01f
    );

    CurrentState.Rotation.Rotation = FMath::RInterpTo(
        CurrentState.Rotation.Rotation,
        TargetState.Rotation.Rotation,
        DeltaTime,
        Speed
    );
}

void UCameraTransitionComponent::UpdateCameraSettings(
    FGlobalCameraState& CurrentState,
    const FGlobalCameraState& TargetState,
    EGlobalCameraTransitionMode TransitionMode,
    float DeltaTime
)
{
    const float Speed = FMath::Max(
        CurrentState.Movement.BlendSpeed,
        0.01f
    );

    auto UpdateFloat = [TransitionMode, DeltaTime, Speed](
        float& Current,
        float Target
    )
    {
        if (TransitionMode == EGlobalCameraTransitionMode::Instant)
        {
            Current = Target;
            return;
        }

        Current = FMath::FInterpTo(
            Current,
            Target,
            DeltaTime,
            Speed
        );

        if (FMath::IsNearlyEqual(Current, Target, 0.01f))
        {
            Current = Target;
        }
    };

    UpdateFloat(
        CurrentState.Movement.MovementSpeed,
        TargetState.Movement.MovementSpeed
    );

    UpdateFloat(
        CurrentState.Movement.RotationSpeed,
        TargetState.Movement.RotationSpeed
    );

    UpdateFloat(
        CurrentState.Lens.FieldOfView,
        TargetState.Lens.FieldOfView
    );

    UpdateFloat(
        CurrentState.Movement.BlendSpeed,
        TargetState.Movement.BlendSpeed
    );
}

void UCameraTransitionComponent::UpdateControlSettings(
    FGlobalCameraState& CurrentState,
    const FGlobalCameraState& TargetState,
    EGlobalCameraTransitionMode TransitionMode,
    float DeltaTime
)
{
    const float Speed = FMath::Max(
        CurrentState.Movement.BlendSpeed,
        0.01f
    );

    auto UpdateFloat = [TransitionMode, DeltaTime, Speed](
        float& Current,
        float Target
    )
    {
        if (Current == Target)
        {
            return;
        }

        if (TransitionMode == EGlobalCameraTransitionMode::Instant)
        {
            Current = Target;
            return;
        }

        Current = FMath::FInterpTo(
            Current,
            Target,
            DeltaTime,
            Speed
        );

        if (FMath::IsNearlyEqual(Current, Target, 0.01f))
        {
            Current = Target;
        }
    };

    UpdateFloat(
        CurrentState.Control.MouseSensitivity,
        TargetState.Control.MouseSensitivity
    );

    if (CurrentState.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson)
    {
        CurrentState.Orbit.OrbitYaw = TargetState.Orbit.OrbitYaw;
        CurrentState.Orbit.OrbitPitch = TargetState.Orbit.OrbitPitch;
    }
    else
    {
        UpdateFloat(
            CurrentState.Orbit.OrbitYaw,
            TargetState.Orbit.OrbitYaw
        );

        UpdateFloat(
            CurrentState.Orbit.OrbitPitch,
            TargetState.Orbit.OrbitPitch
        );
    }

    UpdateFloat(
        CurrentState.Orbit.MinPitch,
        TargetState.Orbit.MinPitch
    );

    UpdateFloat(
        CurrentState.Orbit.MaxPitch,
        TargetState.Orbit.MaxPitch
    );

    UpdateFloat(
        CurrentState.Orbit.OrbitDistance,
        TargetState.Orbit.OrbitDistance
    );

    UpdateFloat(
        CurrentState.Aim.AimDistance,
        TargetState.Aim.AimDistance
    );

    UpdateFloat(
        CurrentState.Aim.AimHeight,
        TargetState.Aim.AimHeight
    );

    UpdateFloat(
        CurrentState.Rotation.LockOnRotation,
        TargetState.Rotation.LockOnRotation
    );
}

void UCameraTransitionComponent::ResetRotationSpeed(float NewSpeed)
{
    CurrentRotationSpeed = FMath::Max(NewSpeed, 0.f);
}

float UCameraTransitionComponent::GetCurrentRotationSpeed() const
{
    return CurrentRotationSpeed;
}