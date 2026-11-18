
#include "GlobalCameraPresetData.h"

bool UGlobalCameraPresetData::FindPreset(
    FName PresetName, FGlobalCameraEventerSettings &OutPreset) const {
  if (PresetName.IsNone()) {
    return false;
  }
  for (const FGlobalCameraEventerSettings &Preset : Presets) {
    if (Preset.Name == PresetName) {
      OutPreset = Preset;
      return true;
    }
  }
  return false;
}

bool UGlobalCameraPresetData::SavePreset(
    FName PresetName, const FGlobalCameraEventerSettings &Preset,
    bool bOverwrite) {
  if (PresetName.IsNone()) {
    return false;
  }
  for (int32 Index = 0; Index < Presets.Num(); ++Index) {
    if (Presets[Index].Name == PresetName) {
      if (!bOverwrite) {
        return false;
      }
      Presets[Index] = Preset;
      Presets[Index].Name = PresetName;
      MarkPackageDirty();
      return true;
    }
  }
  FGlobalCameraEventerSettings NewPreset = Preset;
  NewPreset.Name = PresetName;
  Presets.Add(NewPreset);
  MarkPackageDirty();
  return true;
}

bool UGlobalCameraPresetData::DeletePreset(FName PresetName) {
  if (PresetName.IsNone()) {
    return false;
  }
  for (int32 Index = 0; Index < Presets.Num(); ++Index) {
    if (Presets[Index].Name == PresetName) {
      Presets.RemoveAt(Index);
      MarkPackageDirty();
      return true;
    }
  }
  return false;
}
