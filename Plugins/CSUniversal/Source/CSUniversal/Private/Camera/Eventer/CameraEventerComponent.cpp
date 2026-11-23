#include "Camera/Eventer/CameraEventerComponent.h"
#include "Camera/Eventer/CameraPathComponent.h"
#include "Camera/SystemCamera.h"
#include "EngineUtils.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

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
  
  if (!Action.ShakeClass)
    return;

  APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

  if (!IsValid(PlayerController))
    return;

  APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

  if (!IsValid(CameraManager))
    return;

  if (Action.bStartShake)
  {
    CameraManager->StartCameraShake(
        Action.ShakeClass,
        1.0f,
        ECameraShakePlaySpace::CameraLocal,
        FRotator::ZeroRotator
    );
  }
  else if (Action.bStopShake)
  {
    CameraManager->StopAllInstancesOfCameraShake(
        Action.ShakeClass,
        false
    );
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
