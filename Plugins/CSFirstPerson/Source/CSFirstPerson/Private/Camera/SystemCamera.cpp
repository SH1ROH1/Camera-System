// _________________________________________
// includes
#include "Camera/SystemCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
// _________________________________________
// Base
ASystemCamera::ASystemCamera() {
  PrimaryActorTick.bCanEverTick = true;
  Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  SetRootComponent(Root);
  CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  CameraComponent->SetupAttachment(Root);
}
void ASystemCamera::BeginPlay() {
  Super::BeginPlay();
  if (!PlayerTarget) {
    PlayerTarget = GetOwner();
  }
  TargetState = CurrentState;
  FixedWorldPosition = GetWorldPositionFromState(CurrentState);
  if (CameraComponent) {
    CameraComponent->SetRelativeLocation(FVector::ZeroVector);
    CameraComponent->SetRelativeRotation(FRotator::ZeroRotator);
    CameraComponent->SetFieldOfView(CurrentState.FieldOfView);
  }
}
void ASystemCamera::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  UpdateBlend(DeltaTime);
  UpdateCameraTransform(DeltaTime);
}
// _________________________________________
// Target state setters
void ASystemCamera::SetTransitionMode(EGlobalCameraTransitionMode NewMode) {
  TransitionMode = NewMode;
}

void ASystemCamera::SetCameraState(
    const FGlobalCameraState &NewState,
    EGlobalCameraTransitionMode NewTransitionMode) {
  const bool bWasLookingAtTarget =
      TargetState.bLookAtTarget && !NewState.bLookAtTarget;
  const bool bSameControlMode =
      CurrentState.ControlMode == NewState.ControlMode;

  FGlobalCameraState StateToApply = NewState;

  if (!bSameControlMode) {
    const FRotator CameraRotation = GetActorRotation();
    StateToApply.Rotation = CameraRotation;
    StateToApply.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
    StateToApply.OrbitPitch = FMath::Clamp(
        CameraRotation.Pitch, StateToApply.MinPitch, StateToApply.MaxPitch);
  } else if (NewState.ControlMode == EGlobalCameraControlMode::ThirdPerson) {
    StateToApply.OrbitYaw = TargetState.OrbitYaw;
    StateToApply.OrbitPitch = TargetState.OrbitPitch;
  }

  // Сохраняем фактический поворот камеры при выходе из LookAt.
  if (bWasLookingAtTarget &&
      NewState.ControlMode == EGlobalCameraControlMode::FirstPerson) {
    const FRotator CameraRotation = GetActorRotation();
    StateToApply.Rotation = CameraRotation;
    StateToApply.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);
    StateToApply.OrbitPitch = FMath::Clamp(
        CameraRotation.Pitch, StateToApply.MinPitch, StateToApply.MaxPitch);
  }

  TargetState = StateToApply;
  CurrentState.bInvertThirdPersonPitch = StateToApply.bInvertThirdPersonPitch;

  SetTransitionMode(NewTransitionMode);

  if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None) {
    FixedWorldPosition = GetWorldPositionFromState(TargetState);
  }
}

void ASystemCamera::SetTargetPosition(FVector NewPosition,
                                      EGlobalCameraTransitionMode NewMode) {
  TargetState.Position = NewPosition;
  SetTransitionMode(NewMode);
  if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None) {
    FixedWorldPosition = GetWorldPositionFromState(TargetState);
  }
}
void ASystemCamera::SetTargetPositionMode(
    EGlobalCameraPositionMode NewMode, EGlobalCameraTransitionMode Transition) {
  TargetState.PositionMode = NewMode;
  SetTransitionMode(Transition);
  if (TargetState.FollowTarget == EGlobalCameraFollowTarget::None) {
    FixedWorldPosition = GetWorldPositionFromState(TargetState);
  }
}
void ASystemCamera::SetTargetFollow(EGlobalCameraFollowTarget NewTarget,
                                    EGlobalCameraTransitionMode NewMode) {
  TargetState.FollowTarget = NewTarget;
  SetTransitionMode(NewMode);
  if (NewTarget == EGlobalCameraFollowTarget::None) {
    FixedWorldPosition = GetWorldPositionFromState(TargetState);
  }
}
void ASystemCamera::SetTargetCollision(bool bEnabled,
                                       EGlobalCameraTransitionMode NewMode) {
  TargetState.bUseCollision = bEnabled;
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotation(FRotator NewRotation,
                                      EGlobalCameraTransitionMode NewMode) {
  TargetState.Rotation = NewRotation;

  if (TargetState.ControlMode != EGlobalCameraControlMode::ThirdPerson) {
    TargetState.OrbitYaw = FRotator::NormalizeAxis(NewRotation.Yaw);

    TargetState.OrbitPitch = FMath::Clamp(
        NewRotation.Pitch, TargetState.MinPitch, TargetState.MaxPitch);
  }

  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotationSettings(
    bool bUseTargetRotation, bool bUsePathRotation, float NewLockOnRotation,
    EGlobalCameraTransitionMode NewMode) {
  const bool bWasUsingTargetRotation = TargetState.bUseTargetRotation;

  // Переход от поворота актора к ручному управлению камерой.
  if (bWasUsingTargetRotation && !bUseTargetRotation &&
      TargetState.ControlMode != EGlobalCameraControlMode::ThirdPerson) {
    const FRotator CameraRotation = GetActorRotation();

    CurrentState.Rotation = CameraRotation;
    TargetState.Rotation = CameraRotation;

    CurrentState.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);

    CurrentState.OrbitPitch = FMath::Clamp(
        CameraRotation.Pitch, CurrentState.MinPitch, CurrentState.MaxPitch);

    TargetState.OrbitYaw = CurrentState.OrbitYaw;
    TargetState.OrbitPitch = CurrentState.OrbitPitch;
  }

  TargetState.bUseTargetRotation = bUseTargetRotation;
  TargetState.bUsePathRotation = bUsePathRotation;
  TargetState.LockOnRotation = NewLockOnRotation;

  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetLookAt(bool bEnabled,
                                    EGlobalCameraLookMode NewLookMode,
                                    EGlobalCameraTransitionMode NewMode) {
  const bool bWasLookingAtTarget = TargetState.bLookAtTarget;

  if (bWasLookingAtTarget && !bEnabled &&
      TargetState.ControlMode != EGlobalCameraControlMode::ThirdPerson) {
    const FRotator CameraRotation = GetActorRotation();

    CurrentState.Rotation = CameraRotation;
    TargetState.Rotation = CameraRotation;

    CurrentState.OrbitYaw = FRotator::NormalizeAxis(CameraRotation.Yaw);

    CurrentState.OrbitPitch = FMath::Clamp(
        CameraRotation.Pitch, CurrentState.MinPitch, CurrentState.MaxPitch);

    TargetState.OrbitYaw = CurrentState.OrbitYaw;
    TargetState.OrbitPitch = CurrentState.OrbitPitch;
  }

  TargetState.bLookAtTarget = bEnabled;
  TargetState.LookMode = NewLookMode;

  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetFieldOfView(float NewFieldOfView,
                                         EGlobalCameraTransitionMode NewMode) {
  TargetState.FieldOfView = FMath::Clamp(NewFieldOfView, 5.f, 170.f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetMovementSpeed(
    float NewSpeed, EGlobalCameraTransitionMode NewMode) {
  TargetState.MovementSpeed = FMath::Max(NewSpeed, 0.01f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetRotationSpeed(
    float NewSpeed, EGlobalCameraTransitionMode NewMode) {
  TargetState.RotationSpeed = FMath::Max(NewSpeed, 0.01f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetBlendSpeed(float NewSpeed,
                                        EGlobalCameraTransitionMode NewMode) {
  TargetState.BlendSpeed = FMath::Max(NewSpeed, 0.01f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetControlMode(
    EGlobalCameraControlMode NewMode, EGlobalCameraTransitionMode Transition) {
  TargetState.ControlMode = NewMode;
  SetTransitionMode(Transition);
}
void ASystemCamera::SetTargetMouseSensitivity(
    float NewSensitivity, EGlobalCameraTransitionMode NewMode) {
  TargetState.MouseSensitivity = FMath::Max(NewSensitivity, 0.f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetOrbitSettings(
    float NewYaw, float NewPitch, float NewDistance, float NewMinPitch,
    float NewMaxPitch, EGlobalCameraTransitionMode NewMode) {
  TargetState.OrbitYaw = FRotator::NormalizeAxis(NewYaw);
  TargetState.MinPitch = FMath::Clamp(NewMinPitch, -89.f, 89.f);
  TargetState.MaxPitch = FMath::Clamp(NewMaxPitch, TargetState.MinPitch, 89.f);
  TargetState.OrbitPitch =
      FMath::Clamp(NewPitch, TargetState.MinPitch, TargetState.MaxPitch);
  TargetState.OrbitDistance = FMath::Max(NewDistance, 1.f);
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetAimSettings(float NewDistance, float NewHeight,
                                         EGlobalCameraTransitionMode NewMode) {
  TargetState.AimDistance = FMath::Max(NewDistance, 0.f);
  TargetState.AimHeight = NewHeight;
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetTargetInvertThirdPersonPitch(
    bool bInvert, EGlobalCameraTransitionMode NewMode) {
  TargetState.bInvertThirdPersonPitch = bInvert;
  CurrentState.bInvertThirdPersonPitch = bInvert;
  SetTransitionMode(NewMode);
}
void ASystemCamera::SetConnectTarget(AActor *NewTarget) {
  ConnectTarget = NewTarget;
}
// _________________________________________
// Update functions
void ASystemCamera::UpdateBlend(float DeltaTime) {
  UpdateDiscreteSettings();
  UpdatePositionState(DeltaTime);
  UpdateRotationState(DeltaTime);
  UpdateCameraSettings(DeltaTime);
  UpdateControlSettings(DeltaTime);
}
void ASystemCamera::UpdateDiscreteSettings() {
  CurrentState.PositionMode = TargetState.PositionMode;
  CurrentState.FollowTarget = TargetState.FollowTarget;
  CurrentState.bUseCollision = TargetState.bUseCollision;
  CurrentState.bUseTargetRotation = TargetState.bUseTargetRotation;
  CurrentState.bUsePathRotation = TargetState.bUsePathRotation;
  CurrentState.bLookAtTarget = TargetState.bLookAtTarget;
  CurrentState.ControlMode = TargetState.ControlMode;
  CurrentState.LookMode = TargetState.LookMode;
  CurrentState.bInvertThirdPersonPitch = TargetState.bInvertThirdPersonPitch;
}
void ASystemCamera::UpdatePositionState(float DeltaTime) {
  if (TransitionMode == EGlobalCameraTransitionMode::Instant) {
    CurrentState.Position = TargetState.Position;
    return;
  }
  const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
  CurrentState.Position = FMath::VInterpTo(
      CurrentState.Position, TargetState.Position, DeltaTime, Speed);
  /*if (CurrentState.Position.Equals(TargetState.Position, 0.01f))
  {
          CurrentState.Position = TargetState.Position;
  }*/
}
void ASystemCamera::UpdateRotationState(float DeltaTime) {
  if (TransitionMode == EGlobalCameraTransitionMode::Instant) {
    CurrentRotationSpeed = 0.f;
    CurrentState.Rotation = TargetState.Rotation;
    return;
  }

  const bool bFirstPersonReady =
      CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson &&
      CurrentState.Position.Equals(TargetState.Position, 0.1f);

  if (bFirstPersonReady) {
    const float MaxRotationSpeed = 100.f;
    const float RotationAcceleration = 250.f;

    CurrentRotationSpeed =
        FMath::FInterpConstantTo(CurrentRotationSpeed, MaxRotationSpeed,
                                 DeltaTime, RotationAcceleration);

    CurrentState.Rotation =
        FMath::RInterpTo(CurrentState.Rotation, TargetState.Rotation, DeltaTime,
                         FMath::Max(CurrentRotationSpeed, 0.01f));
    return;
  }

  CurrentRotationSpeed = 0.f;

  const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);

  CurrentState.Rotation = FMath::RInterpTo(
      CurrentState.Rotation, TargetState.Rotation, DeltaTime, Speed);
}
void ASystemCamera::UpdateCameraSettings(float DeltaTime) {
  const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
  auto UpdateFloat = [this, DeltaTime, Speed](float &Current, float Target) {
    if (TransitionMode == EGlobalCameraTransitionMode::Instant) {
      Current = Target;
      return;
    }
    Current = FMath::FInterpTo(Current, Target, DeltaTime, Speed);
    if (FMath::IsNearlyEqual(Current, Target, 0.01f)) {
      Current = Target;
    }
  };
  UpdateFloat(CurrentState.MovementSpeed, TargetState.MovementSpeed);
  UpdateFloat(CurrentState.RotationSpeed, TargetState.RotationSpeed);
  UpdateFloat(CurrentState.FieldOfView, TargetState.FieldOfView);
  UpdateFloat(CurrentState.BlendSpeed, TargetState.BlendSpeed);
}
void ASystemCamera::UpdateControlSettings(float DeltaTime) {
  const float Speed = FMath::Max(CurrentState.BlendSpeed, 0.01f);
  auto UpdateFloat = [this, DeltaTime, Speed](float &Current, float Target) {
    if (Current == Target) {
      return;
    }
    if (TransitionMode == EGlobalCameraTransitionMode::Instant) {
      Current = Target;
      return;
    }
    Current = FMath::FInterpTo(Current, Target, DeltaTime, Speed);
    if (FMath::IsNearlyEqual(Current, Target, 0.01f)) {
      Current = Target;
    }
  };

  UpdateFloat(CurrentState.MouseSensitivity, TargetState.MouseSensitivity);

  // Для третьего лица орбита изменяется мгновенно.
  // Это обеспечивает мгновенное реагирование камеры на мышь.
  if (CurrentState.ControlMode == EGlobalCameraControlMode::ThirdPerson) {
    CurrentState.OrbitYaw = TargetState.OrbitYaw;
    CurrentState.OrbitPitch = TargetState.OrbitPitch;
  } else {
    // Для первого лица орбита интерполируется плавно.
    UpdateFloat(CurrentState.OrbitYaw, TargetState.OrbitYaw);
    UpdateFloat(CurrentState.OrbitPitch, TargetState.OrbitPitch);
  }

  UpdateFloat(CurrentState.MinPitch, TargetState.MinPitch);
  UpdateFloat(CurrentState.MaxPitch, TargetState.MaxPitch);
  UpdateFloat(CurrentState.OrbitDistance, TargetState.OrbitDistance);
  UpdateFloat(CurrentState.AimDistance, TargetState.AimDistance);
  UpdateFloat(CurrentState.AimHeight, TargetState.AimHeight);
  UpdateFloat(CurrentState.LockOnRotation, TargetState.LockOnRotation);
}

void ASystemCamera::UpdateCameraRotation(float LookX, float LookY) {
  if (TargetState.ControlMode == EGlobalCameraControlMode::None ||
      TargetState.bUseTargetRotation ||
      (TargetState.ControlMode == EGlobalCameraControlMode::ThirdPerson &&
       TargetState.bLookAtTarget && IsValid(ConnectTarget)) ||
      (FMath::IsNearlyZero(LookX) && FMath::IsNearlyZero(LookY))) {
    return;
  }

  if (TargetState.ControlMode == EGlobalCameraControlMode::ThirdPerson) {
    TargetState.OrbitYaw = FRotator::NormalizeAxis(
        TargetState.OrbitYaw + LookX * TargetState.MouseSensitivity);

    const float VerticalDirection =
        CurrentState.bInvertThirdPersonPitch ? -1.f : 1.f;

    TargetState.OrbitPitch =
        FMath::Clamp(TargetState.OrbitPitch + VerticalDirection * LookY *
                                                  TargetState.MouseSensitivity,
                     TargetState.MinPitch, TargetState.MaxPitch);
    return;
  }

  if (TargetState.bLookAtTarget) {
    return;
  }

  TargetState.OrbitYaw = FRotator::NormalizeAxis(
      TargetState.OrbitYaw + LookX * TargetState.MouseSensitivity);

  TargetState.OrbitPitch = FMath::Clamp(
      TargetState.OrbitPitch + LookY * TargetState.MouseSensitivity,
      TargetState.MinPitch, TargetState.MaxPitch);

  TargetState.Rotation.Yaw = TargetState.OrbitYaw;
  TargetState.Rotation.Pitch = TargetState.OrbitPitch;
}

void ASystemCamera::UpdateCameraTransform(float DeltaTime) {
  const FVector DesiredLocation = CalculateDesiredLocation();
  FRotator DesiredRotation = CurrentState.Rotation;
  const bool bUseTargetRotationSpeed =
      CurrentState.bLookAtTarget || CurrentState.bUseTargetRotation;

  const float RotationSpeed = bUseTargetRotationSpeed
                                  ? FMath::Max(TargetState.RotationSpeed, 2.f)
                                  : CurrentState.RotationSpeed;
  if (CurrentState.bUseTargetRotation) {
    if (AActor *RotationTarget = GetRotationTarget()) {
      DesiredRotation = RotationTarget->GetActorRotation();
      DesiredRotation.Yaw += CurrentState.LockOnRotation;
    }

  } else if (CurrentState.bLookAtTarget) {
    FVector LookAtLocation;
    bool bHasLookAtLocation = false;

    if (CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson &&
        IsValid(ConnectTarget)) {
      LookAtLocation = ConnectTarget->GetActorLocation() +
                       FVector::UpVector * CurrentState.AimHeight;
      bHasLookAtLocation = true;
    } else if (CurrentState.ControlMode ==
                   EGlobalCameraControlMode::ThirdPerson &&
               IsValid(PlayerTarget) && IsValid(ConnectTarget)) {
      LookAtLocation = (PlayerTarget->GetActorLocation() +
                        ConnectTarget->GetActorLocation()) *
                           0.5f +
                       FVector::UpVector * CurrentState.AimHeight;
      bHasLookAtLocation = true;
    } else {
      AActor *LookAtTarget = GetFollowTarget();

      if (!IsValid(LookAtTarget)) {
        LookAtTarget = PlayerTarget;
      }

      if (IsValid(LookAtTarget)) {
        if (CurrentState.LookMode == EGlobalCameraLookMode::LookAtTarget) {
          LookAtLocation = LookAtTarget->GetActorLocation() +
                           FVector::UpVector * CurrentState.AimHeight;
        } else {
          const FVector TargetLocation = LookAtTarget->GetActorLocation();
          const FVector TargetRight = LookAtTarget->GetActorRightVector();

          const float ShoulderOffset =
              FMath::Clamp(CurrentState.AimDistance * 0.05f, 10.f, 100.f);

          LookAtLocation = TargetLocation + TargetRight * ShoulderOffset +
                           FVector::UpVector * CurrentState.AimHeight;
        }

        bHasLookAtLocation = true;
      }
    }

    if (bHasLookAtLocation) {
      DesiredRotation = UKismetMathLibrary::FindLookAtRotation(DesiredLocation,
                                                               LookAtLocation);

      CurrentState.Rotation = DesiredRotation;
      TargetState.Rotation = DesiredRotation;
    }
  }

  if (CurrentState.ControlMode == EGlobalCameraControlMode::FirstPerson &&
      CurrentState.Position.Equals(TargetState.Position, 3.f)) {
    const float MoveSpeed = FMath::Max(CurrentState.MovementSpeed * 2.f, 40.f);

    const float PositionTolerance = 5.f;

    FVector NewLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation,
                                           DeltaTime, MoveSpeed);

    if (FVector::DistSquared(NewLocation, DesiredLocation) <=
        FMath::Square(PositionTolerance)) {
      NewLocation = DesiredLocation;
    }

    const bool bDirectMouseControl =
        !CurrentState.bUseTargetRotation && !CurrentState.bLookAtTarget;

    const FRotator NewRotation =
        bDirectMouseControl
            ? CurrentState.Rotation
            : FMath::RInterpConstantTo(
                  GetActorRotation(), DesiredRotation, DeltaTime,
                  FMath::Max(CurrentState.RotationSpeed, 0.01f) * 30.f);

    SetActorLocationAndRotation(NewLocation, NewRotation);
  } else {
    if (TransitionMode == EGlobalCameraTransitionMode::Instant ||
        DeltaTime <= 0.f ||
        (!bUseTargetRotationSpeed && RotationSpeed <= 0.f) ||
        CurrentState.MovementSpeed <= 0.f) {
      SetActorLocationAndRotation(DesiredLocation, DesiredRotation);
    } else {
      const FVector NewLocation =
          FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime,
                           FMath::Max(CurrentState.MovementSpeed, 0.01f));

      const bool bDirectMouseControl =
          !CurrentState.bUseTargetRotation && !CurrentState.bLookAtTarget;

      const FRotator NewRotation =
          bDirectMouseControl
              ? CurrentState.Rotation
              : FMath::RInterpConstantTo(
                    GetActorRotation(), DesiredRotation, DeltaTime,
                    FMath::Max(RotationSpeed, 0.01f) * 30.f);

      SetActorLocationAndRotation(NewLocation, NewRotation);
    }
  }

  if (CameraComponent) {
    CameraComponent->SetFieldOfView(CurrentState.FieldOfView);
  }
}
// _________________________________________
// Target getters
AActor *ASystemCamera::GetTarget() const {
  return PlayerTarget ? PlayerTarget : ConnectTarget;
}
AActor *
ASystemCamera::GetPositionTarget(EGlobalCameraPositionMode PositionMode) const {
  switch (PositionMode) {
  case EGlobalCameraPositionMode::PlayerTarget:
    return PlayerTarget;
  case EGlobalCameraPositionMode::ConnectTarget:
    return ConnectTarget;
  default:
    return nullptr;
  }
}
AActor *ASystemCamera::GetFollowTarget() const {
  switch (CurrentState.FollowTarget) {
  case EGlobalCameraFollowTarget::PlayerTarget:
    return PlayerTarget;
  case EGlobalCameraFollowTarget::ConnectTarget:
    return ConnectTarget;
  default:
    return nullptr;
  }
}
AActor *ASystemCamera::GetRotationTarget() const {
  return CurrentState.bUseTargetRotation ? GetFollowTarget() : nullptr;
}
void ASystemCamera::SetPlayerTarget(AActor *NewTarget) {
  PlayerTarget = NewTarget;
}
FVector ASystemCamera::GetWorldPositionFromState(
    const FGlobalCameraState &State) const {
  if (State.PositionMode == EGlobalCameraPositionMode::World) {
    return State.Position;
  }
  if (AActor *Target = GetPositionTarget(State.PositionMode)) {
    return Target->GetActorTransform().TransformPosition(State.Position);
  }
  return State.Position;
}
FVector ASystemCamera::GetRelativePositionFromWorld(
    const FVector &WorldPosition,
    EGlobalCameraPositionMode PositionMode) const {
  if (AActor *Target = GetPositionTarget(PositionMode)) {
    return Target->GetActorTransform().InverseTransformPosition(WorldPosition);
  }
  return WorldPosition;
}
// _________________________________________
// Position space conversion
void ASystemCamera::ConvertPositionSpace(
    EGlobalCameraPositionMode NewPositionMode) {
  const FVector WorldPosition = GetActorLocation();
  CurrentState.Position =
      GetRelativePositionFromWorld(WorldPosition, NewPositionMode);
  CurrentState.Rotation = GetActorRotation();
  if (CurrentState.FollowTarget == EGlobalCameraFollowTarget::None) {
    FixedWorldPosition = WorldPosition;
  }
}
// _________________________________________
// Desired location calculation
FVector ASystemCamera::CalculateDesiredLocation() const {
  if (CurrentState.FollowTarget == EGlobalCameraFollowTarget::None) {
    return FixedWorldPosition;
  }

  AActor *FollowTarget = GetFollowTarget();

  if (CurrentState.ControlMode == EGlobalCameraControlMode::ThirdPerson &&
      CurrentState.FollowTarget == EGlobalCameraFollowTarget::ConnectTarget &&
      IsValid(PlayerTarget)) {
    FollowTarget = PlayerTarget;
  }

  if (!IsValid(FollowTarget)) {
    return FixedWorldPosition;
  }

  const FVector TargetLocation = FollowTarget->GetActorLocation();

  if (CurrentState.ControlMode != EGlobalCameraControlMode::ThirdPerson) {
    return TargetLocation + CurrentState.Position;
  }

  float OrbitYaw = CurrentState.OrbitYaw;

  if (IsValid(PlayerTarget) && IsValid(ConnectTarget)) {
    FVector DirectionToConnectTarget =
        ConnectTarget->GetActorLocation() - PlayerTarget->GetActorLocation();

    // Высота цели не должна менять орбиту камеры.
    DirectionToConnectTarget.Z = 0.f;

    if (!DirectionToConnectTarget.IsNearlyZero()) {
      OrbitYaw = DirectionToConnectTarget.Rotation().Yaw;
    }
  }

  const FVector OrbitCenter =
      TargetLocation + FVector::UpVector * CurrentState.AimHeight;

  const FRotator OrbitRotation(CurrentState.OrbitPitch, OrbitYaw, 0.f);

  const float OrbitDistance = FMath::Max(CurrentState.OrbitDistance, 1.f);

  const FVector DesiredLocation =
      OrbitCenter - OrbitRotation.Vector() * OrbitDistance;

  if (!CurrentState.bUseCollision || !GetWorld()) {
    return DesiredLocation;
  }

  FHitResult Hit;
  FCollisionQueryParams Params(SCENE_QUERY_STAT(SystemCameraCollision), false);

  Params.AddIgnoredActor(FollowTarget);
  Params.AddIgnoredActor(this);

  const bool bHit = GetWorld()->SweepSingleByChannel(
      Hit, OrbitCenter, DesiredLocation, FQuat::Identity, ECC_Camera,
      FCollisionShape::MakeSphere(15.f), Params);

  return bHit ? Hit.Location + Hit.Normal * 15.f : DesiredLocation;
}