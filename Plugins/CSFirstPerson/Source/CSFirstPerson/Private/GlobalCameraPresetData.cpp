
#include "GlobalCameraPresetData.h"

bool UGlobalCameraPresetData::FindPreset(FName PresetName, FGlobalCameraEventerPreset& OutPreset) const
{
	if (PresetName.IsNone())
	{
		return false;
	}
	for (const FGlobalCameraEventerPreset& Preset : Presets)
	{
		if (Preset.PresetName == PresetName)
		{
			OutPreset = Preset;
			return true;
		}
	}
	return false;
}

bool UGlobalCameraPresetData::SavePreset(FName PresetName, const FGlobalCameraEventerPreset& Preset, bool bOverwrite)
{
	if (PresetName.IsNone())
	{
		return false;
	}
	for (int32 Index = 0; Index < Presets.Num(); ++Index)
	{
		if (Presets[Index].PresetName == PresetName)
		{
			if (!bOverwrite)
			{
				return false;
			}
			Presets[Index] = Preset;
			Presets[Index].PresetName = PresetName;
			MarkPackageDirty();
			return true;
		}
	}
	FGlobalCameraEventerPreset NewPreset = Preset;
	NewPreset.PresetName = PresetName;
	Presets.Add(NewPreset);
	MarkPackageDirty();
	return true;
}

bool UGlobalCameraPresetData::DeletePreset(FName PresetName)
{
	if (PresetName.IsNone())
	{
		return false;
	}
	for (int32 Index = 0; Index < Presets.Num(); ++Index)
	{
		if (Presets[Index].PresetName == PresetName)
		{
			Presets.RemoveAt(Index);
			MarkPackageDirty();
			return true;
		}
	}
	return false;
}
