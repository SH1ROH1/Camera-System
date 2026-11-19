#include "Camera/Eventer/CameraEventerComponent.h"
#include "Camera/Eventer/CameraPathComponent.h"
#include "Camera/SystemCamera.h"
#include "EngineUtils.h"

UCameraEventerComponent::UCameraEventerComponent() {
  PrimaryComponentTick.bCanEverTick = false;
}

void UCameraEventerComponent::BeginPlay() {
  Super::BeginPlay();
  InitializeCamera();
}

void UCameraEventerComponent::InitializeCamera() {
  if (IsValid(Camera) || !GetWorld()) {
    return;
  }

  for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It) {
    Camera = *It;
    break;
  }
}

void UCameraEventerComponent::SetCamera(ASystemCamera *NewCamera) {
  Camera = NewCamera;
}

void UCameraEventerComponent::ApplyAction(
    const FCameraEventerActionSettings &Action) {
  if (!IsValid(Camera)) {
    InitializeCamera();
  }

  if (!IsValid(Camera)) {
    return;
  }

  Camera->SetConnectTarget(Action.ConnectTarget);
  Camera->SetCameraState(Action.CameraState, Action.TransitionMode);

  UCameraPathComponent *PathComponent =
      GetOwner()->FindComponentByClass<UCameraPathComponent>();

  if (IsValid(Action.CameraPathActor)) {
    if (IsValid(PathComponent)) {
      PathComponent->StartPath(Camera, Action);
    }
  } else if (IsValid(PathComponent)) {
    PathComponent->StopPath();
  }
}

void UCameraEventerComponent::ActivateEventer(
    const FCameraEventerSettings &EventerSettings) {
  ApplyAction(EventerSettings.Activation);
}

void UCameraEventerComponent::DeactivateEventer(
    const FCameraEventerSettings &EventerSettings) {
  if (!EventerSettings.bRestoreOnExit) {
    return;
  }

  ApplyAction(EventerSettings.Restore);
}

ASystemCamera *UCameraEventerComponent::GetCamera() const { return Camera; }
