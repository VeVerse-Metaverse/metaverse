// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/UIComboBoxPortal.h"
#include "Widgets/SNullWidget.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "VeUI"

/////////////////////////////////////////////////////
// UUIComboBoxPortal

static FComboBoxStyle* DefaultComboBoxPortalStyle = nullptr;
static FTableRowStyle* DefaultComboBoxPortalRowStyle = nullptr;

#if WITH_EDITOR
static FComboBoxStyle* EditorComboBoxPortalStyle = nullptr;
static FTableRowStyle* EditorComboBoxPortalRowStyle = nullptr;
#endif 

UUIComboBoxPortal::UUIComboBoxPortal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (DefaultComboBoxPortalStyle == nullptr)
	{
		DefaultComboBoxPortalStyle = new FComboBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"));

		// Unlink UMG default colors.
		DefaultComboBoxPortalStyle->UnlinkColors();
	}

	if (DefaultComboBoxPortalRowStyle == nullptr)
	{
		DefaultComboBoxPortalRowStyle = new FTableRowStyle(FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG default colors.
		DefaultComboBoxPortalRowStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultComboBoxPortalStyle;
	ItemStyle = *DefaultComboBoxPortalRowStyle;

#if WITH_EDITOR 
	if (EditorComboBoxPortalStyle == nullptr)
	{
		EditorComboBoxPortalStyle = new FComboBoxStyle(FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("EditorUtilityComboBox"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxPortalStyle->UnlinkColors();
	}

	if (EditorComboBoxPortalRowStyle == nullptr)
	{
		EditorComboBoxPortalRowStyle = new FTableRowStyle(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxPortalRowStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		WidgetStyle = *EditorComboBoxPortalStyle;
		ItemStyle = *EditorComboBoxPortalRowStyle;

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

void UUIComboBoxPortal::PostInitProperties()
{
	Super::PostInitProperties();

	// Initialize the set of options from the default set only once.
	for (const FVePortalMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}
}

void UUIComboBoxPortal::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyComboBox.Reset();
	ComboBoxContent.Reset();
}

void UUIComboBoxPortal::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
}

void UUIComboBoxPortal::PostLoad()
{
	Super::PostLoad();

	// Initialize the set of options from the default set only once.
	for (const FVePortalMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}

	if (GetLinkerCustomVersion(FEditorObjectVersion::GUID) < FEditorObjectVersion::ComboBoxControllerSupportUpdate)
	{
		EnableGamepadNavigationMode = false;
	}
}

TSharedRef<SWidget> UUIComboBoxPortal::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(SelectedOption.Id);
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}

	MyComboBox =
		SNew(SComboBox< TSharedPtr<FVePortalMetadata> >)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItem(CurrentOptionPtr)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVePortalMetadata> >::FOnGenerateWidget, HandleGenerateWidget))
		.OnSelectionChanged(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVePortalMetadata> >::FOnSelectionChanged, HandleSelectionChanged))
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

void UUIComboBoxPortal::AddOption(const FVePortalMetadata& Option)
{
	Options.Add(MakeShareable(new FVePortalMetadata(Option)));

	RefreshOptions();
}

bool UUIComboBoxPortal::RemoveOption(const FGuid& Id)
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

int32 UUIComboBoxPortal::FindOptionIndex(const FGuid& Id) const
{
	for ( int32 OptionIndex = 0; OptionIndex < Options.Num(); OptionIndex++ )
	{
		const TSharedPtr<FVePortalMetadata>& OptionAtIndex = Options[OptionIndex];

		if ( ( *OptionAtIndex ).Id == Id )
		{
			return OptionIndex;
		}
	}

	return -1;
}

FVePortalMetadata UUIComboBoxPortal::GetOptionAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < Options.Num())
	{
		return *(Options[Index]);
	}
	return FVePortalMetadata();
}

void UUIComboBoxPortal::ClearOptions()
{
	ClearSelection();

	Options.Empty();

	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxPortal::ClearSelection()
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

void UUIComboBoxPortal::RefreshOptions()
{
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxPortal::SetSelectedOption(const FGuid& Id)
{
	int32 InitialIndex = FindOptionIndex(Id);
	SetSelectedIndex(InitialIndex);
}

void UUIComboBoxPortal::SetSelectedIndex(const int32 Index)
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
FVePortalMetadata UUIComboBoxPortal::GetSelectedOption() const
{
	if (CurrentOptionPtr.IsValid())
	{
		return *CurrentOptionPtr;
	}
	return FVePortalMetadata();
}

int32 UUIComboBoxPortal::GetSelectedIndex() const
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

int32 UUIComboBoxPortal::GetOptionCount() const
{
	return Options.Num();
}

bool UUIComboBoxPortal::IsOpen() const
{
	return MyComboBox.IsValid() && MyComboBox->IsOpen();
}

void UUIComboBoxPortal::UpdateOrGenerateWidget(TSharedPtr<FVePortalMetadata> Item)
{
	// If no custom widget was supplied and the default STextBlock already exists,
	// just update its text instead of rebuilding the widget.
	if (DefaultComboBoxContent.IsValid() && (IsDesignTime() || OnGenerateWidgetEvent.IsBound()))
	{
		const FVePortalMetadata PortalItem = Item.IsValid() ? *Item : FVePortalMetadata();
		if (PortalItem.Name.IsEmpty()) {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(PortalItem.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
		} else {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(PortalItem.Name));
		}
	}
	else
	{
		DefaultComboBoxContent.Reset();
		ComboBoxContent->SetContent(HandleGenerateWidget(Item));
	}
}

TSharedRef<SWidget> UUIComboBoxPortal::HandleGenerateWidget(TSharedPtr<FVePortalMetadata> Item) const
{
	FString StringItem;
	if (Item.IsValid()) {
		if (Item->Name.IsEmpty()) {
			StringItem = Item->Id.ToString(EGuidFormats::DigitsWithHyphensLower);
		} else {
			StringItem = Item->Name;
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

void UUIComboBoxPortal::HandleSelectionChanged(TSharedPtr<FVePortalMetadata> Item, ESelectInfo::Type SelectionType)
{
	CurrentOptionPtr = Item;
	SelectedOption = CurrentOptionPtr.IsValid() ? CurrentOptionPtr.ToSharedRef().Get() : FVePortalMetadata();

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if ( ComboBoxContent.IsValid() )
	{
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	if ( !IsDesignTime() )
	{
		OnSelectionChanged.Broadcast(Item.IsValid() ? *Item : FVePortalMetadata(), SelectionType);
	}
}

void UUIComboBoxPortal::HandleOpening()
{
	OnOpening.Broadcast();
}

#if WITH_EDITOR

const FText UUIComboBoxPortal::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
