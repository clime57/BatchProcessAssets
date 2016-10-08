// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatchProcessAssetsPrivatePCH.h"
#include "BatchProcessAssetsEdMode.h"

#define LOCTEXT_NAMESPACE "FBatchProcessAssetsModule"

void FBatchProcessAssetsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FBatchProcessAssetsEdMode>(FBatchProcessAssetsEdMode::EM_BatchProcessAssetsEdModeId, LOCTEXT("BatchProcessAssetsEdModeName", "BatchProcessAssetsEdMode"), FSlateIcon(), true);
}

void FBatchProcessAssetsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FBatchProcessAssetsEdMode::EM_BatchProcessAssetsEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBatchProcessAssetsModule, BatchProcessAssets)