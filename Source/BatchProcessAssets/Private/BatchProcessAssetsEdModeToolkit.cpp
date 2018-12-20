// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatchProcessAssetsPrivatePCH.h"
#include "BatchProcessAssetsEdMode.h"
#include "BatchProcessAssetsEdModeToolkit.h"

#include "ContentBrowserModule.h"
#include "ScaleTextureFactory.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FBatchProcessAssetsEdModeToolkit"

int32 FBatchProcessAssetsEdModeToolkit::MaxTexSize = 32;
bool FBatchProcessAssetsEdModeToolkit::bIsNotReallyModifyOriginalTex = false;
FBatchProcessAssetsEdModeToolkit::FComboItemType FBatchProcessAssetsEdModeToolkit::CurrentItem;
FBatchProcessAssetsEdModeToolkit::FBatchProcessAssetsEdModeToolkit()
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return true;
		}

		static FReply OnButtonClick()
		{
			ReImport();
			return FReply::Handled();
		}
		
		static TSharedRef<SWidget> MakeButton(FText InLabel)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick);
		}
	};

	const float Factor = 256.0f;

	Options.Add(MakeShareable(new FString("32")));
	Options.Add(MakeShareable(new FString("64")));
	Options.Add(MakeShareable(new FString("128")));
	Options.Add(MakeShareable(new FString("256")));
	Options.Add(MakeShareable(new FString("512")));
	CurrentItem = Options[0];


	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(50)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("ScaleTextureDesc", "Select the texture to be scaled in the Content Browser, then click the ScaleTextures button, or select nothing to zoom all the textures."))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectMaxTextureSize", "SelectMaxTextureSize"))
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(5)
				[
					SNew(SComboBox<FComboItemType>)
					.OptionsSource(&Options)
					.OnSelectionChanged_Static(&FBatchProcessAssetsEdModeToolkit::OnSelectionChanged)
					.OnGenerateWidget_Static(&FBatchProcessAssetsEdModeToolkit::MakeWidgetForOption)
					.InitiallySelectedItem(CurrentItem)
					[
						SNew(STextBlock)
						.Text_Static(&FBatchProcessAssetsEdModeToolkit::GetCurrentItemLabel)
					]
				]
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(10)
			[
				SNew(SCheckBox)
				.OnCheckStateChanged_Static(&FBatchProcessAssetsEdModeToolkit::OnIsModifyOrigenalChanged)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DoNotModifyOrigenalAssets", "DoNotModifyOrigenalAssets"))
					.ToolTipText(LOCTEXT("DoNotModifyOrigenalAssetsToolTipText","Checked State means only modifying Maximum Texture Size Property of the Texture"))
				]
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(10)
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel1", "ScaleTextures"))
				]
		];
}

void FBatchProcessAssetsEdModeToolkit::OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type)
{
	CurrentItem = NewValue;
	MaxTexSize = FCString::Atoi(**(CurrentItem.Get()));
}

TSharedRef<SWidget> FBatchProcessAssetsEdModeToolkit::MakeWidgetForOption(FComboItemType InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

FText FBatchProcessAssetsEdModeToolkit::GetCurrentItemLabel()
{
	if (CurrentItem.IsValid())
	{
		return FText::FromString(*CurrentItem);
	}

	return LOCTEXT("InvalidComboEntryText", "<<Invalid option>>");
}

void FBatchProcessAssetsEdModeToolkit::OnIsModifyOrigenalChanged(ECheckBoxState CheckBoxState)
{
	bIsNotReallyModifyOriginalTex = CheckBoxState == ECheckBoxState::Checked ? true : false;
}

FName FBatchProcessAssetsEdModeToolkit::GetToolkitFName() const
{
	return FName("BatchProcessAssetsEdMode");
}

FText FBatchProcessAssetsEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("BatchProcessAssetsEdModeToolkit", "DisplayName", "BatchProcessAssetsEdMode Tool");
}

class FEdMode* FBatchProcessAssetsEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FBatchProcessAssetsEdMode::EM_BatchProcessAssetsEdModeId);
}


void FBatchProcessAssetsEdModeToolkit::ReImport()
{
	TArray<FAssetData> SelectedAssets;
	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();

	ContentBrowserSingleton.GetSelectedAssets(SelectedAssets);
	TArray<FAssetData> SelectedTexAssets;
	for (const FAssetData& ad : SelectedAssets)
	{
		if (ad.GetClass()->GetName().Equals(TEXT("Texture2D")))
		{
			SelectedTexAssets.Add(ad);
		}
	}

	if (SelectedTexAssets.Num() > 0)
	{
		ReImportSelected(SelectedTexAssets);
	}
	else
	{
		Import();
	}



}
void FBatchProcessAssetsEdModeToolkit::Import() {
	TArray<FAssetData> OutAssetDataList;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	new (Filter.ClassNames) FName("Texture2D");
	new (Filter.PackagePaths) FName("/Game");
	AssetRegistryModule.Get().GetAssets(Filter, OutAssetDataList);
	ImportADL(OutAssetDataList);
}

void FBatchProcessAssetsEdModeToolkit::SyncBrowserToAssets(const TArray<UObject*>& AssetsToSync)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(AssetsToSync, /*bAllowLockedBrowsers=*/true);
}

void FBatchProcessAssetsEdModeToolkit::ImportADL(TArray<FAssetData>& SelectedAssets)
{
	TArray<UObject*> ReturnObjects;
	auto TextureFact = NewObject<UScaleTextureFactory>();
	TextureFact->SetImportParam(MaxTexSize, bIsNotReallyModifyOriginalTex);
	FScopedSlowTask SlowTask(SelectedAssets.Num(), FText::FromString("Importing"));
	SlowTask.MakeDialog();
	for (int32 i = 0; i < SelectedAssets.Num(); ++i)
	{
		SlowTask.EnterProgressFrame(1, FText::Format(FText::FromString("ReImporting \"{0}\"..."), FText::FromName(SelectedAssets[i].PackagePath)));
		UObject* Result = TextureFact->ReImport(SelectedAssets[i]);
		if (Result)
		{
			ReturnObjects.Add(Result);
		}
	}

	if (ReturnObjects.Num())
	{
		SyncBrowserToAssets(ReturnObjects);
	}
}

void FBatchProcessAssetsEdModeToolkit::ReImportSelected(TArray<FAssetData>& SelectedAssets) {
	ImportADL(SelectedAssets);
}

#undef LOCTEXT_NAMESPACE
