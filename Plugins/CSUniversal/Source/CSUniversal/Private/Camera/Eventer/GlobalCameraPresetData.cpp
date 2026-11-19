#include "Camera/Eventer/GlobalCameraPresetData.h"

static FCameraEventerPresetAction
ToPresetAction(const FCameraEventerActionSettings &Action) {
  FCameraEventerPresetAction Preset;
  Preset.CameraState = Action.CameraState;
  Preset.TransitionMode = Action.TransitionMode;
  Preset.ConnectTargetName = IsValid(Action.ConnectTarget)
                                 ? Action.ConnectTarget->GetFName()
                                 : NAME_None;
  Preset.CameraPathActorName = IsValid(Action.CameraPathActor)
                                   ? Action.CameraPathActor->GetFName()
                                   : NAME_None;
  Preset.PathSettings = Action.PathSettings;
  return Preset;
}

static FGlobalCameraEventerPreset
ToPreset(FName PresetName, const FCameraEventerSettings &Settings) {
  FGlobalCameraEventerPreset Preset;
  Preset.PresetName = PresetName;
  Preset.Activation = ToPresetAction(Settings.Activation);
  Preset.Restore = ToPresetAction(Settings.Restore);
  Preset.bRestoreOnExit = Settings.bRestoreOnExit;
  Preset.bFireOnlyOnce = Settings.bFireOnlyOnce;
  Preset.ActivationDelay = Settings.ActivationDelay;
  return Preset;
}

bool UGlobalCameraPresetData::FindPreset(
    FName PresetName, FGlobalCameraEventerPreset &OutPreset) const {
  if (PresetName.IsNone()) {
    return false;
  }

  for (const FGlobalCameraEventerPreset &Preset : Presets) {
    if (Preset.PresetName == PresetName) {
      OutPreset = Preset;
      return true;
    }
  }

  return false;
}

bool UGlobalCameraPresetData::SavePreset(FName PresetName,
                                         const FCameraEventerSettings &Preset,
                                         bool bOverwrite) {
  if (PresetName.IsNone()) {
    return false;
  }

  for (FGlobalCameraEventerPreset &ExistingPreset : Presets) {
    if (ExistingPreset.PresetName == PresetName) {
      if (!bOverwrite) {
        return false;
      }

      ExistingPreset = ToPreset(PresetName, Preset);
      MarkPackageDirty();
      return true;
    }
  }

  Presets.Add(ToPreset(PresetName, Preset));
  MarkPackageDirty();
  return true;
}

bool UGlobalCameraPresetData::DeletePreset(FName PresetName) {
  if (PresetName.IsNone()) {
    return false;
  }

  for (int32 Index = 0; Index < Presets.Num(); ++Index) {
    if (Presets[Index].PresetName == PresetName) {
      Presets.RemoveAt(Index);
      MarkPackageDirty();
      return true;
    }
  }

  return false;
}