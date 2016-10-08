// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatchProcessAssetsPrivatePCH.h"
#include "BatchProcessAssetsEdMode.h"
#include "BatchProcessAssetsEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"

const FEditorModeID FBatchProcessAssetsEdMode::EM_BatchProcessAssetsEdModeId = TEXT("EM_BatchProcessAssetsEdMode");

FBatchProcessAssetsEdMode::FBatchProcessAssetsEdMode()
{

}

FBatchProcessAssetsEdMode::~FBatchProcessAssetsEdMode()
{

}

void FBatchProcessAssetsEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FBatchProcessAssetsEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FBatchProcessAssetsEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FBatchProcessAssetsEdMode::UsesToolkits() const
{
	return true;
}




