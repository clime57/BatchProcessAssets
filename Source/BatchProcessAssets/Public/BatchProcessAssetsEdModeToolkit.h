// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Editor/UnrealEd/Public/Toolkits/BaseToolkit.h"


class FBatchProcessAssetsEdModeToolkit : public FModeToolkit
{

public:
	typedef TSharedPtr<FString> FComboItemType;

	FBatchProcessAssetsEdModeToolkit();

	static void OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type);
	static TSharedRef<SWidget> MakeWidgetForOption(FComboItemType InOption);
	static FText GetCurrentItemLabel();
	static void OnIsModifyOrigenalChanged(ECheckBoxState CheckBoxState);
	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }
	static void ReImport();

	static UObject* ReImport(FAssetData& AssetData);
	static void ImportADL(TArray<FAssetData>& SelectedAssets);
	static void Import();
	static void SyncBrowserToAssets(const TArray<UObject*>& AssetsToSync);
	static void ReImportSelected(TArray<FAssetData>& SelectedAssets);

	static void FindUnreferencedAssets();
private:
	TSharedPtr<SWidget> ToolkitWidget;
	TArray< FComboItemType > Options;
	static FComboItemType CurrentItem;
public:
	static int32 MaxTexSize;
	static bool bIsNotReallyModifyOriginalTex;


};
