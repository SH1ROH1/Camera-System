
#include "Camera/Eventer/SystemCameraEventer.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#endif

ASystemCameraEventer::ASystemCameraEventer() {
  PrimaryActorTick.bCanEverTick = true;
  OnActorBeginOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorEntered);
  OnActorEndOverlap.AddDynamic(this, &ASystemCameraEventer::OnActorLeft);

  CameraEventerComponent = CreateDefaultSubobject<UCameraEventerComponent>(
      TEXT("CameraEventerComponent"));
  CameraPathComponent =
      CreateDefaultSubobject<UCameraPathComponent>(TEXT("CameraPathComponent"));
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

  if (PendingActor.IsValid()) {
    if (!PendingActor->IsOverlappingActor(this)) {
      PendingActor = nullptr;
      ActivationTimer = 0.f;
    } else if (!Settings.bFireOnlyOnce || !bTriggered) {
      if (Settings.ActivationDelay <= 0.f) {
        ActivateEventer(PendingActor.Get());
        PendingActor = nullptr;
        ActivationTimer = 0.f;
      } else {
        ActivationTimer += DeltaTime;
        if (ActivationTimer >= Settings.ActivationDelay) {
          ActivateEventer(PendingActor.Get());
          PendingActor = nullptr;
          ActivationTimer = 0.f;
        }
      }
    }
  }

  if (!PendingActor.IsValid() && (!Settings.bFireOnlyOnce || !bTriggered)) {
    CheckExistingOverlap();
  }
}

void ASystemCameraEventer::OnActorEntered(AActor *OverlappedActor,
                                          AActor *OtherActor) {
  if (!OtherActor || OtherActor == this || !OtherActor->IsA<ACharacter>()) {
    return;
  }

  if (Settings.bFireOnlyOnce && bTriggered) {
    return;
  }

  PendingActor = OtherActor;
  ActivationTimer = 0.f;

  if (Settings.ActivationDelay <= 0.f) {
    ActivateEventer(OtherActor);
    PendingActor = nullptr;
    ActivationTimer = 0.f;
  }
}

void ASystemCameraEventer::ActivateEventer(AActor *OtherActor) {
  if (!OtherActor || (Settings.bFireOnlyOnce && bTriggered)) {
    return;
  }

  GameCamera = nullptr;

  for (TActorIterator<ASystemCamera> It(GetWorld()); It; ++It) {
    if (It->GetOwner() == OtherActor) {
      GameCamera = *It;
      break;
    }
  }

  if (!GameCamera && CameraEventerComponent) {
    GameCamera = CameraEventerComponent->GetCamera();
  }

  if (!GameCamera) {
    UE_LOG(LogTemp, Error, TEXT("EVENTER: Camera for %s not found"),
           *GetNameSafe(OtherActor));
    return;
  }

  bTriggered = true;

  ApplyCameraSettings();

  if (CameraPathComponent) {
    CameraPathComponent->StartPath(GameCamera, Settings.Activation);
  }
}

void ASystemCameraEventer::OnActorLeft(AActor *OverlappedActor,
                                       AActor *OtherActor) {
  if (!OtherActor || OtherActor == this || !OtherActor->IsA<ACharacter>()) {
    return;
  }

  if (PendingActor.Get() == OtherActor) {
    PendingActor = nullptr;
    ActivationTimer = 0.f;
  }

  if (!GameCamera) {
    return;
  }

  if (CameraPathComponent) {
    CameraPathComponent->StopPath();
  }

  if (Settings.bRestoreOnExit && CameraEventerComponent) {
    CameraEventerComponent->DeactivateEventer(Settings);
  }

  GameCamera = nullptr;
}

void ASystemCameraEventer::CheckExistingOverlap() {
  TArray<AActor *> OverlappingActors;
  GetOverlappingActors(OverlappingActors);

  for (AActor *Actor : OverlappingActors) {
    if (!Actor || !Actor->IsA<ACharacter>()) {
      continue;
    }

    if (Settings.bFireOnlyOnce && bTriggered) {
      return;
    }

    if (PendingActor.Get() == Actor) {
      continue;
    }

    OnActorEntered(this, Actor);
    return;
  }
}

void ASystemCameraEventer::ApplyCameraSettings() {
  if (!GameCamera || !CameraEventerComponent) {
    return;
  }

  CameraEventerComponent->SetCamera(GameCamera);
  CameraEventerComponent->ActivateEventer(Settings);
}

#if WITH_EDITOR

bool ASystemCameraEventer::ConfirmOverwrite() const {
  const EAppReturnType::Type Result = FMessageDialog::Open(
      EAppMsgType::YesNo, NSLOCTEXT("SystemCameraEventer", "OverwritePreset",
                                    "Preset already exists. Overwrite it?"));

  return Result == EAppReturnType::Yes;
}

AActor *ASystemCameraEventer::FindActorByName(const FName &ActorName) const {
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
