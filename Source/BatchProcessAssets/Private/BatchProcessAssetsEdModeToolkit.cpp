// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatchProcessAssetsPrivatePCH.h"
#include "BatchProcessAssetsEdMode.h"
#include "BatchProcessAssetsEdModeToolkit.h"

#include "ContentBrowserModule.h"
#include "ScaleTextureFactory.h"
#include "IContentBrowserSingleton.h"

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
			ReImport(1);
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
					.ToolTipText(LOCTEXT("DoNotModifyOrigenalAssetsToolTipText","Checked Steate means only modifying Maximum Texture Size Property of the Texture"))
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


void FBatchProcessAssetsEdModeToolkit::ReImport(float scale)
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

	auto TextureFact = NewObject<UScaleTextureFactory>();
	if (SelectedTexAssets.Num() > 0)
	{
		TextureFact->ReImportSelected(scale, SelectedTexAssets , MaxTexSize, bIsNotReallyModifyOriginalTex);
	}
	else
	{
		TextureFact->Import(scale, MaxTexSize , bIsNotReallyModifyOriginalTex);
	}
}


#undef LOCTEXT_NAMESPACE
