#include "Camera/Eventer/CameraPathComponent.h"
#include "Camera/SystemCamera.h"
#include "Components/SplineComponent.h"

namespace
{
AActor* GetPathTarget(
    ASystemCamera* Camera,
    EGlobalCameraFollowTarget FollowTarget)
{
  if (!IsValid(Camera))
  {
    return nullptr;
  }

  if (FollowTarget == EGlobalCameraFollowTarget::ConnectTarget)
  {
    if (AActor* ConnectTarget = Camera->GetConnectTarget())
    {
      return ConnectTarget;
    }

    return Camera->GetPlayerTarget();
  }

  if (AActor* PlayerTarget = Camera->GetPlayerTarget())
  {
    return PlayerTarget;
  }

  return Camera->GetConnectTarget();
}

float GetPathDistanceBeforeTarget(
    float TargetDistance,
    float StartDistanceThreshold,
    float PathOffset,
    float SplineLength,
    bool bLoop)
{
  float Distance = TargetDistance - StartDistanceThreshold + PathOffset;

  if (bLoop)
  {
    Distance = FMath::Fmod(Distance, SplineLength);

    if (Distance < 0.f)
    {
      Distance += SplineLength;
    }

    return Distance;
  }

  return FMath::Clamp(Distance, 0.f, SplineLength);
}
}

UCameraPathComponent::UCameraPathComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCameraPathComponent::StartPath(
    ASystemCamera* InCamera,
    const FCameraEventerActionSettings& Action)
{
  StopPath();

  if (!IsValid(InCamera) || !IsValid(Action.CameraPathActor))
  {
    return;
  }

  USplineComponent* Spline =
      Action.CameraPathActor->FindComponentByClass<USplineComponent>();

  if (!IsValid(Spline) || Spline->GetSplineLength() <= KINDA_SMALL_NUMBER)
  {
    return;
  }

  Camera = InCamera;
  ActiveSpline = Spline;
  CameraState = Action.CameraState;
  TransitionMode = Action.TransitionMode;

  StartDistanceThreshold =
      FMath::Max(Action.PathSettings.StartDistanceThreshold, 0.f);
  PathOffset = Action.PathSettings.PathOffset;
  FollowSmoothing =
      FMath::Max(Action.PathSettings.FollowSmoothing, KINDA_SMALL_NUMBER);
  bLoop = Action.PathSettings.bLoop;

  float ClosestDistance = 0.f;

  if (!FindClosestPathDistance(ClosestDistance))
  {
    StopPath();
    return;
  }

  const float SplineLength = ActiveSpline->GetSplineLength();

  CurrentPathDistance = GetPathDistanceBeforeTarget(
      ClosestDistance,
      StartDistanceThreshold,
      PathOffset,
      SplineLength,
      bLoop);

  bPathActive = true;
  SetComponentTickEnabled(true);

  // Применяем позицию сразу, не оставляя камере исходный (в том числе
  // нулевой) CameraState до первого тика компонента.
  UpdatePath(0.f);
}

void UCameraPathComponent::StopPath()
{
  bPathActive = false;
  SetComponentTickEnabled(false);
  Camera = nullptr;
  ActiveSpline = nullptr;
  CurrentPathDistance = 0.f;
}

bool UCameraPathComponent::IsPathActive() const
{
  return bPathActive;
}

void UCameraPathComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  UpdatePath(DeltaTime);
}

bool UCameraPathComponent::FindClosestPathDistance(float& OutDistance) const
{
  OutDistance = 0.f;

  if (!IsValid(ActiveSpline) || !IsValid(Camera))
  {
    return false;
  }

  AActor* Target = GetPathTarget(Camera, CameraState.Target.FollowTarget);

  if (!IsValid(Target))
  {
    return false;
  }

  const float InputKey =
      ActiveSpline->FindInputKeyClosestToWorldLocation(Target->GetActorLocation());

  OutDistance =
      ActiveSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

  return true;
}

void UCameraPathComponent::UpdatePath(float DeltaTime)
{
  if (!bPathActive || !IsValid(Camera) || !IsValid(ActiveSpline))
  {
    StopPath();
    return;
  }

  AActor* Target = GetPathTarget(Camera, CameraState.Target.FollowTarget);

  if (!IsValid(Target))
  {
    StopPath();
    return;
  }

  const float SplineLength = ActiveSpline->GetSplineLength();

  if (SplineLength <= KINDA_SMALL_NUMBER)
  {
    StopPath();
    return;
  }

  const float ClosestKey =
      ActiveSpline->FindInputKeyClosestToWorldLocation(Target->GetActorLocation());

  const float TargetDistanceOnSpline =
      ActiveSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);

  const float TargetPathDistance = GetPathDistanceBeforeTarget(
      TargetDistanceOnSpline,
      StartDistanceThreshold,
      PathOffset,
      SplineLength,
      bLoop);

  float InterpolationTarget = TargetPathDistance;

  if (bLoop)
  {
    const float HalfSplineLength = SplineLength * 0.5f;
    const float DistanceDelta = InterpolationTarget - CurrentPathDistance;

    if (DistanceDelta > HalfSplineLength)
    {
      InterpolationTarget -= SplineLength;
    }
    else if (DistanceDelta < -HalfSplineLength)
    {
      InterpolationTarget += SplineLength;
    }
  }

  CurrentPathDistance = FMath::FInterpTo(
      CurrentPathDistance,
      InterpolationTarget,
      DeltaTime,
      FollowSmoothing);

  if (bLoop)
  {
    CurrentPathDistance = FMath::Fmod(CurrentPathDistance, SplineLength);

    if (CurrentPathDistance < 0.f)
    {
      CurrentPathDistance += SplineLength;
    }
  }
  else
  {
    CurrentPathDistance =
        FMath::Clamp(CurrentPathDistance, 0.f, SplineLength);
  }

  FGlobalCameraState PathState = CameraState;

  // Путь задаёт абсолютную мировую позицию. Иначе ThirdPerson использует
  // орбиту, а не позицию, вычисленную по сплайну.
  PathState.Position.PositionMode = EGlobalCameraPositionMode::World;
  PathState.Target.FollowTarget = EGlobalCameraFollowTarget::None;
  PathState.Position.Position = ActiveSpline->GetLocationAtDistanceAlongSpline(
      CurrentPathDistance,
      ESplineCoordinateSpace::World);

  if (PathState.Rotation.bUsePathRotation)
  {
    const FVector Tangent = ActiveSpline->GetTangentAtDistanceAlongSpline(
        CurrentPathDistance,
        ESplineCoordinateSpace::World);

    if (!Tangent.IsNearlyZero())
    {
      PathState.Rotation.Rotation = Tangent.Rotation();
      PathState.Rotation.bUseTargetRotation = false;
      PathState.Aim.bLookAtTarget = false;
    }
  }

  Camera->SetCameraState(PathState, TransitionMode);

  if (!bLoop)
  {
    const bool bAtStart =
        TargetPathDistance <= 1.f && CurrentPathDistance <= 1.f;
    const bool bAtEnd =
        TargetPathDistance >= SplineLength - 1.f &&
        CurrentPathDistance >= SplineLength - 1.f;

    if (bAtStart || bAtEnd)
    {
      StopPath();
    }
  }
}
