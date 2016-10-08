// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editor/UnrealEd/Public/Toolkits/BaseToolkit.h"


class FBatchProcessAssetsEdModeToolkit : public FModeToolkit
{
public:

	FBatchProcessAssetsEdModeToolkit();

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }
	static void ReImport(float scale);
	static void ReImportSelected(float scale);
private:

	TSharedPtr<SWidget> ToolkitWidget;
};
