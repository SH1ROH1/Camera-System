
#include "Camera/Eventer/SystemCameraEventer.h"
#include "Camera/Eventer/CameraEventerComponent.h"
#include "Camera/Eventer/CameraPathComponent.h"
#include "Camera/SystemCamera.h"
#include "FPCharacter.h"
#include "EngineUtils.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#endif

ASystemCameraEventer::ASystemCameraEventer() {
  PrimaryActorTick.bCanEverTick = true;

  OnActorBeginOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorEntered);
  OnActorEndOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorLeft);

  CameraEventerComponent = CreateDefaultSubobject<UCameraEventerComponent>(
      TEXT("CameraEventerComponent"));

  CameraPathComponent = CreateDefaultSubobject<UCameraPathComponent>(
      TEXT("CameraPathComponent"));
}

void ASystemCameraEventer::BeginPlay() {
  Super::BeginPlay();

  if (CameraEventerComponent) {
    CameraEventerComponent->InitializeCamera();
  }

  for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It) {
    GameCamera = *It;
    break;
  }

  CheckExistingOverlap();
}

void ASystemCameraEventer::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  // Handle an actor that has left the trigger.
  if (AActor* Actor = TriggeredActor.Get()) {
    if (!Actor->IsOverlappingActor(this)) {
      OnActorLeft(this, Actor);
    }
  } else {
    TriggeredActor = nullptr;
  }

  // Process pending activation.
  if (PendingActor.IsValid()) {
    AActor* Actor = PendingActor.Get();

    if (!Actor->IsOverlappingActor(this)) {
      PendingActor = nullptr;
      ActivationTimer = 0.f;
    } else if (Settings.bFireOnlyOnce && bTriggered) {
      PendingActor = nullptr;
      ActivationTimer = 0.f;
    } else {
      ActivationTimer += DeltaTime;

      if (ActivationTimer >= Settings.ActivationDelay) {
        ActivateEventer(Actor);

        PendingActor = nullptr;
        ActivationTimer = 0.f;
      }
    }
  }

  // Find another actor only when no actor is being processed.
  if (!PendingActor.IsValid() &&
      !TriggeredActor.IsValid() &&
      (!Settings.bFireOnlyOnce || !bTriggered)) {
    CheckExistingOverlap();
  }
}

void ASystemCameraEventer::OnActorEntered(
    AActor* OverlappedActor,
    AActor* OtherActor) {

  if (!OtherActor || OtherActor == this ||
      !OtherActor->IsA<ACharacter>()) {
    return;
  }

  if (Settings.bFireOnlyOnce && bTriggered) {
    return;
  }

  if (TriggeredActor.IsValid() || PendingActor.IsValid()) {
    return;
  }

  PendingActor = OtherActor;
  ActivationTimer = 0.f;
}

void ASystemCameraEventer::ActivateEventer(AActor* OtherActor) {
  if (!IsValid(OtherActor) ||
      (Settings.bFireOnlyOnce && bTriggered)) {
    return;
  }

  GameCamera = nullptr;

  // Find a camera owned by the entering actor.
  for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It) {
    if (It->GetOwner() == OtherActor) {
      GameCamera = *It;
      break;
    }
  }

  // Fallback to the initialized camera.
  if (!GameCamera && CameraEventerComponent) {
    GameCamera = CameraEventerComponent->GetCamera();
  }

  if (!IsValid(GameCamera)) {
    UE_LOG(LogTemp, Error,
           TEXT("EVENTER: Camera for %s not found"),
           *GetNameSafe(OtherActor));
    return;
  }

  // Lock this actor until it leaves the trigger.
  TriggeredActor = OtherActor;
  bTriggered = true;

  ApplyCameraSettings();

  // Store the last activated eventer.
  GameCamera->SetLastTriggeredEventer(this);
}

void ASystemCameraEventer::OnActorLeft(
    AActor* OverlappedActor,
    AActor* OtherActor) {

  if (!OtherActor || OtherActor == this) {
    return;
  }

  // Cancel pending activation.
  if (PendingActor.Get() == OtherActor) {
    PendingActor = nullptr;
    ActivationTimer = 0.f;
  }

  // Only restore the camera belonging to the activated actor.
  if (TriggeredActor.Get() != OtherActor) {
    return;
  }

  TriggeredActor = nullptr;

  if (CameraPathComponent) {
    CameraPathComponent->StopPath();
  }

  if (Settings.bRestoreOnExit && CameraEventerComponent) {
    CameraEventerComponent->DeactivateEventer(Settings);
    GameCamera->SetLastTriggeredEventer(nullptr);
  }

  GameCamera = nullptr;
}

void ASystemCameraEventer::CheckExistingOverlap() {
  if (PendingActor.IsValid() || TriggeredActor.IsValid()) {
    return;
  }

  TArray<AActor*> OverlappingActors;
  GetOverlappingActors(OverlappingActors);

  for (AActor* Actor : OverlappingActors) {
    if (!IsValid(Actor) || !Actor->IsA<ACharacter>()) {
      continue;
    }

    if (Settings.bFireOnlyOnce && bTriggered) {
      return;
    }

    PendingActor = Actor;
    ActivationTimer = 0.f;
    return;
  }
}

void ASystemCameraEventer::ApplyCameraSettings() {
  if (!IsValid(GameCamera) || !CameraEventerComponent) {
    return;
  }

  CameraEventerComponent->SetCamera(GameCamera);
  CameraEventerComponent->ActivateEventer(Settings);
}

#if WITH_EDITOR

bool ASystemCameraEventer::ConfirmOverwrite() const {
  const EAppReturnType::Type Result = FMessageDialog::Open(
      EAppMsgType::YesNo,
      NSLOCTEXT("SystemCameraEventer", "OverwritePreset",
                "Preset already exists. Overwrite it?"));

  return Result == EAppReturnType::Yes;
}

AActor* ASystemCameraEventer::FindActorByName(
    const FName& ActorName) const {

  if (ActorName.IsNone() || !GetWorld()) {
    return nullptr;
  }

  for (TActorIterator<AActor> It(GetWorld()); It; ++It) {
    if (It->GetFName() == ActorName) {
      return *It;
    }
  }

  return nullptr;
}

void ASystemCameraEventer::SavePreset() {
  if (!PresetData || PresetName.IsNone()) {
    return;
  }

  FGlobalCameraEventerPreset ExistingPreset;
  const bool bExists = PresetData->FindPreset(PresetName, ExistingPreset);

  if (bExists && !ConfirmOverwrite()) {
    return;
  }

  PresetData->SavePreset(PresetName, Settings, bExists);
}

void ASystemCameraEventer::LoadPreset() {
  if (!PresetData || PresetName.IsNone()) {
    return;
  }

  FGlobalCameraEventerPreset Preset;

  if (!PresetData->FindPreset(PresetName, Preset)) {
    return;
  }

  Settings.Activation.CameraState = Preset.Activation.CameraState;
  Settings.Activation.TransitionMode = Preset.Activation.TransitionMode;
  Settings.Activation.PathSettings = Preset.Activation.PathSettings;
  Settings.Activation.ConnectTarget =
      FindActorByName(Preset.Activation.ConnectTargetName);
  Settings.Activation.CameraPathActor =
      FindActorByName(Preset.Activation.CameraPathActorName);

  Settings.Restore.CameraState = Preset.Restore.CameraState;
  Settings.Restore.TransitionMode = Preset.Restore.TransitionMode;
  Settings.Restore.PathSettings = Preset.Restore.PathSettings;
  Settings.Restore.ConnectTarget =
      FindActorByName(Preset.Restore.ConnectTargetName);
  Settings.Restore.CameraPathActor =
      FindActorByName(Preset.Restore.CameraPathActorName);

  Settings.bRestoreOnExit = Preset.bRestoreOnExit;
  Settings.bFireOnlyOnce = Preset.bFireOnlyOnce;
  Settings.ActivationDelay = Preset.ActivationDelay;

  if (CameraPathComponent) {
    CameraPathComponent->StopPath();
  }
}

void ASystemCameraEventer::DeletePreset() {
  if (!PresetData || PresetName.IsNone()) {
    return;
  }

  PresetData->DeletePreset(PresetName);
}

#endif
