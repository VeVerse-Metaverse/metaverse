// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/UIComboBoxMetaverse.h"
#include "Widgets/SNullWidget.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "VeUI"

/////////////////////////////////////////////////////
// UUIComboBoxMetaverse

static FComboBoxStyle* DefaultComboBoxMetaverseStyle = nullptr;
static FTableRowStyle* DefaultComboBoxMetaverseRowStyle = nullptr;

#if WITH_EDITOR
static FComboBoxStyle* EditorComboBoxMetaverseStyle = nullptr;
static FTableRowStyle* EditorComboBoxMetaverseRowStyle = nullptr;
#endif 

UUIComboBoxMetaverse::UUIComboBoxMetaverse(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (DefaultComboBoxMetaverseStyle == nullptr)
	{
		DefaultComboBoxMetaverseStyle = new FComboBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"));

		// Unlink UMG default colors.
		DefaultComboBoxMetaverseStyle->UnlinkColors();
	}

	if (DefaultComboBoxMetaverseRowStyle == nullptr)
	{
		DefaultComboBoxMetaverseRowStyle = new FTableRowStyle(FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG default colors.
		DefaultComboBoxMetaverseRowStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultComboBoxMetaverseStyle;
	ItemStyle = *DefaultComboBoxMetaverseRowStyle;

#if WITH_EDITOR 
	if (EditorComboBoxMetaverseStyle == nullptr)
	{
		EditorComboBoxMetaverseStyle = new FComboBoxStyle(FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("EditorUtilityComboBox"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxMetaverseStyle->UnlinkColors();
	}

	if (EditorComboBoxMetaverseRowStyle == nullptr)
	{
		EditorComboBoxMetaverseRowStyle = new FTableRowStyle(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxMetaverseRowStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		WidgetStyle = *EditorComboBoxMetaverseStyle;
		ItemStyle = *EditorComboBoxMetaverseRowStyle;

		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif // WITH_EDITOR

	WidgetStyle.UnlinkColors();
	ItemStyle.UnlinkColors();

	ForegroundColor = ItemStyle.TextColor;
	bIsFocusable = true;

	ContentPadding = FMargin(4.0, 2.0);
	MaxListHeight = 450.0f;
	HasDownArrow = true;
	EnableGamepadNavigationMode = true;
	// We don't want to try and load fonts on the server.
	if ( !IsRunningDedicatedServer() )
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 16, FName("Bold"));
	}
}

void UUIComboBoxMetaverse::PostInitProperties()
{
	Super::PostInitProperties();

	// Initialize the set of options from the default set only once.
	for (const FVePackageMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}
}

void UUIComboBoxMetaverse::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyComboBox.Reset();
	ComboBoxContent.Reset();
}

void UUIComboBoxMetaverse::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
}

void UUIComboBoxMetaverse::PostLoad()
{
	Super::PostLoad();

	// Initialize the set of options from the default set only once.
	for (const FVePackageMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}

	if (GetLinkerCustomVersion(FEditorObjectVersion::GUID) < FEditorObjectVersion::ComboBoxControllerSupportUpdate)
	{
		EnableGamepadNavigationMode = false;
	}
}

TSharedRef<SWidget> UUIComboBoxMetaverse::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(SelectedOption.Id);
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}

	MyComboBox =
		SNew(SComboBox< TSharedPtr<FVePackageMetadata> >)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItem(CurrentOptionPtr)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVePackageMetadata> >::FOnGenerateWidget, HandleGenerateWidget))
		.OnSelectionChanged(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVePackageMetadata> >::FOnSelectionChanged, HandleSelectionChanged))
		.OnComboBoxOpening(BIND_UOBJECT_DELEGATE(FOnComboBoxOpening, HandleOpening))
		.IsFocusable(bIsFocusable)
		[
			SAssignNew(ComboBoxContent, SBox)
		];

	if ( InitialIndex != -1 )
	{
		// Generate the widget for the initially selected widget if needed
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	return MyComboBox.ToSharedRef();
}

void UUIComboBoxMetaverse::AddOption(const FVePackageMetadata& Option)
{
	Options.Add(MakeShareable(new FVePackageMetadata(Option)));

	RefreshOptions();
}

bool UUIComboBoxMetaverse::RemoveOption(const FGuid& Id)
{
	int32 OptionIndex = FindOptionIndex(Id);

	if ( OptionIndex != -1 )
	{
		if ( Options[OptionIndex] == CurrentOptionPtr )
		{
			ClearSelection();
		}

		Options.RemoveAt(OptionIndex);

		RefreshOptions();

		return true;
	}

	return false;
}

int32 UUIComboBoxMetaverse::FindOptionIndex(const FGuid& Id) const
{
	for ( int32 OptionIndex = 0; OptionIndex < Options.Num(); OptionIndex++ )
	{
		const TSharedPtr<FVePackageMetadata>& OptionAtIndex = Options[OptionIndex];

		if ( ( *OptionAtIndex ).Id == Id )
		{
			return OptionIndex;
		}
	}

	return -1;
}

FVePackageMetadata UUIComboBoxMetaverse::GetOptionAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < Options.Num())
	{
		return *(Options[Index]);
	}
	return FVePackageMetadata();
}

void UUIComboBoxMetaverse::ClearOptions()
{
	ClearSelection();

	Options.Empty();

	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxMetaverse::ClearSelection()
{
	CurrentOptionPtr.Reset();

	if ( MyComboBox.IsValid() )
	{
		MyComboBox->ClearSelection();
	}

	if ( ComboBoxContent.IsValid() )
	{
		ComboBoxContent->SetContent(SNullWidget::NullWidget);
	}
}

void UUIComboBoxMetaverse::RefreshOptions()
{
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxMetaverse::SetSelectedOption(const FGuid& Id)
{
	int32 InitialIndex = FindOptionIndex(Id);
	SetSelectedIndex(InitialIndex);
}

void UUIComboBoxMetaverse::SetSelectedIndex(const int32 Index)
{
	if (Options.IsValidIndex(Index))
	{
		CurrentOptionPtr = Options[Index];
		// Don't select item if its already selected
		if (SelectedOption.Id != (*CurrentOptionPtr).Id)
		{
			SelectedOption = *CurrentOptionPtr;

			if (ComboBoxContent.IsValid())
			{
				MyComboBox->SetSelectedItem(CurrentOptionPtr);
				UpdateOrGenerateWidget(CurrentOptionPtr);
			}		
			else
			{
				HandleSelectionChanged(CurrentOptionPtr, ESelectInfo::Direct);
			}
		}
	}
}
FVePackageMetadata UUIComboBoxMetaverse::GetSelectedOption() const
{
	if (CurrentOptionPtr.IsValid())
	{
		return *CurrentOptionPtr;
	}
	return FVePackageMetadata();
}

int32 UUIComboBoxMetaverse::GetSelectedIndex() const
{
	if (CurrentOptionPtr.IsValid())
	{
		for (int32 OptionIndex = 0; OptionIndex < Options.Num(); ++OptionIndex)
		{
			if (Options[OptionIndex] == CurrentOptionPtr)
			{
				return OptionIndex;
			}
		}
	}

	return -1;
}

int32 UUIComboBoxMetaverse::GetOptionCount() const
{
	return Options.Num();
}

bool UUIComboBoxMetaverse::IsOpen() const
{
	return MyComboBox.IsValid() && MyComboBox->IsOpen();
}

void UUIComboBoxMetaverse::UpdateOrGenerateWidget(TSharedPtr<FVePackageMetadata> Item)
{
	// If no custom widget was supplied and the default STextBlock already exists,
	// just update its text instead of rebuilding the widget.
	if (DefaultComboBoxContent.IsValid() && (IsDesignTime() || OnGenerateWidgetEvent.IsBound()))
	{
		const FVePackageMetadata MetaverseItem = Item.IsValid() ? *Item : FVePackageMetadata();
		if (!MetaverseItem.Title.IsEmpty()) {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(MetaverseItem.Title));
		} else if (!MetaverseItem.Name.IsEmpty()) {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(MetaverseItem.Name));
		} else {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(MetaverseItem.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
		}
	}
	else
	{
		DefaultComboBoxContent.Reset();
		ComboBoxContent->SetContent(HandleGenerateWidget(Item));
	}
}

TSharedRef<SWidget> UUIComboBoxMetaverse::HandleGenerateWidget(TSharedPtr<FVePackageMetadata> Item) const
{
	FString StringItem;
	if (Item.IsValid()) {
		if (!Item->Title.IsEmpty()) {
			StringItem = Item->Title;
		} else if (!Item->Name.IsEmpty()) {
			StringItem = Item->Name;
		} else {
			StringItem = Item->Id.ToString(EGuidFormats::DigitsWithHyphensLower);
		}
	} else {
		StringItem = FString();
	}

	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if ( !IsDesignTime() && OnGenerateWidgetEvent.IsBound() )
	{
		UWidget* Widget = OnGenerateWidgetEvent.Execute(StringItem);
		if ( Widget != NULL )
		{
			return Widget->TakeWidget();
		}
	}

	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
		.Text(FText::FromString(StringItem))
		.Font(Font);
}

void UUIComboBoxMetaverse::HandleSelectionChanged(TSharedPtr<FVePackageMetadata> Item, ESelectInfo::Type SelectionType)
{
	CurrentOptionPtr = Item;
	SelectedOption = CurrentOptionPtr.IsValid() ? CurrentOptionPtr.ToSharedRef().Get() : FVePackageMetadata();

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if ( ComboBoxContent.IsValid() )
	{
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	if ( !IsDesignTime() )
	{
		OnSelectionChanged.Broadcast(Item.IsValid() ? *Item : FVePackageMetadata(), SelectionType);
	}
}

void UUIComboBoxMetaverse::HandleOpening()
{
	OnOpening.Broadcast();
}

#if WITH_EDITOR

const FText UUIComboBoxMetaverse::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
