// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/UIComboBoxSpace.h"
#include "Widgets/SNullWidget.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "VeUI"

/////////////////////////////////////////////////////
// UUIComboBoxSpace

static FComboBoxStyle* DefaultComboBoxSpaceStyle = nullptr;
static FTableRowStyle* DefaultComboBoxSpaceRowStyle = nullptr;

#if WITH_EDITOR
static FComboBoxStyle* EditorComboBoxSpaceStyle = nullptr;
static FTableRowStyle* EditorComboBoxSpaceRowStyle = nullptr;
#endif 

UUIComboBoxSpace::UUIComboBoxSpace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (DefaultComboBoxSpaceStyle == nullptr)
	{
		DefaultComboBoxSpaceStyle = new FComboBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"));

		// Unlink UMG default colors.
		DefaultComboBoxSpaceStyle->UnlinkColors();
	}

	if (DefaultComboBoxSpaceRowStyle == nullptr)
	{
		DefaultComboBoxSpaceRowStyle = new FTableRowStyle(FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG default colors.
		DefaultComboBoxSpaceRowStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultComboBoxSpaceStyle;
	ItemStyle = *DefaultComboBoxSpaceRowStyle;

#if WITH_EDITOR 
	if (EditorComboBoxSpaceStyle == nullptr)
	{
		EditorComboBoxSpaceStyle = new FComboBoxStyle(FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("EditorUtilityComboBox"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxSpaceStyle->UnlinkColors();
	}

	if (EditorComboBoxSpaceRowStyle == nullptr)
	{
		EditorComboBoxSpaceRowStyle = new FTableRowStyle(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxSpaceRowStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		WidgetStyle = *EditorComboBoxSpaceStyle;
		ItemStyle = *EditorComboBoxSpaceRowStyle;

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

void UUIComboBoxSpace::PostInitProperties()
{
	Super::PostInitProperties();

	// Initialize the set of options from the default set only once.
	for (const FVeWorldMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}
}

void UUIComboBoxSpace::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyComboBox.Reset();
	ComboBoxContent.Reset();
}

void UUIComboBoxSpace::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
}

void UUIComboBoxSpace::PostLoad()
{
	Super::PostLoad();

	// Initialize the set of options from the default set only once.
	for (const FVeWorldMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}

	if (GetLinkerCustomVersion(FEditorObjectVersion::GUID) < FEditorObjectVersion::ComboBoxControllerSupportUpdate)
	{
		EnableGamepadNavigationMode = false;
	}
}

TSharedRef<SWidget> UUIComboBoxSpace::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(SelectedOption.Id);
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}

	MyComboBox =
		SNew(SComboBox< TSharedPtr<FVeWorldMetadata> >)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItem(CurrentOptionPtr)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVeWorldMetadata> >::FOnGenerateWidget, HandleGenerateWidget))
		.OnSelectionChanged(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVeWorldMetadata> >::FOnSelectionChanged, HandleSelectionChanged))
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

void UUIComboBoxSpace::AddOption(const FVeWorldMetadata& Option)
{
	Options.Add(MakeShareable(new FVeWorldMetadata(Option)));

	RefreshOptions();
}

bool UUIComboBoxSpace::RemoveOption(const FGuid& Id)
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

int32 UUIComboBoxSpace::FindOptionIndex(const FGuid& Id) const {
	for ( int32 OptionIndex = 0; OptionIndex < Options.Num(); OptionIndex++ ) {
		const TSharedPtr<FVeWorldMetadata>& OptionAtIndex = Options[OptionIndex];

		if ( ( *OptionAtIndex ).Id == Id ) {
			return OptionIndex;
		}
	}

	return -1;
}

FVeWorldMetadata UUIComboBoxSpace::GetOptionAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < Options.Num())
	{
		return *(Options[Index]);
	}
	return FVeWorldMetadata();
}

void UUIComboBoxSpace::ClearOptions()
{
	ClearSelection();

	Options.Empty();

	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxSpace::ClearSelection()
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

void UUIComboBoxSpace::RefreshOptions()
{
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxSpace::SetSelectedOption(const FGuid& Id) {
	int32 InitialIndex = FindOptionIndex(Id);
	SetSelectedIndex(InitialIndex);
}

void UUIComboBoxSpace::SetSelectedIndex(const int32 Index)
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
FVeWorldMetadata UUIComboBoxSpace::GetSelectedOption() const
{
	if (CurrentOptionPtr.IsValid())
	{
		return *CurrentOptionPtr;
	}
	return FVeWorldMetadata();
}

int32 UUIComboBoxSpace::GetSelectedIndex() const
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

int32 UUIComboBoxSpace::GetOptionCount() const
{
	return Options.Num();
}

bool UUIComboBoxSpace::IsOpen() const
{
	return MyComboBox.IsValid() && MyComboBox->IsOpen();
}

void UUIComboBoxSpace::UpdateOrGenerateWidget(TSharedPtr<FVeWorldMetadata> Item)
{
	// If no custom widget was supplied and the default STextBlock already exists,
	// just update its text instead of rebuilding the widget.
	if (DefaultComboBoxContent.IsValid() && (IsDesignTime() || OnGenerateWidgetEvent.IsBound()))
	{
		const FVeWorldMetadata SpaceItem = Item.IsValid() ? *Item : FVeWorldMetadata();
		if (SpaceItem.Name.IsEmpty()) {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(SpaceItem.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
		} else {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(SpaceItem.Name));
		}
	}
	else
	{
		DefaultComboBoxContent.Reset();
		ComboBoxContent->SetContent(HandleGenerateWidget(Item));
	}
}

TSharedRef<SWidget> UUIComboBoxSpace::HandleGenerateWidget(TSharedPtr<FVeWorldMetadata> Item) const
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

void UUIComboBoxSpace::HandleSelectionChanged(TSharedPtr<FVeWorldMetadata> Item, ESelectInfo::Type SelectionType)
{
	CurrentOptionPtr = Item;
	SelectedOption = CurrentOptionPtr.IsValid() ? CurrentOptionPtr.ToSharedRef().Get() : FVeWorldMetadata();

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if ( ComboBoxContent.IsValid() )
	{
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	if ( !IsDesignTime() )
	{
		OnSelectionChanged.Broadcast(Item.IsValid() ? *Item : FVeWorldMetadata(), SelectionType);
	}
}

void UUIComboBoxSpace::HandleOpening()
{
	OnOpening.Broadcast();
}

#if WITH_EDITOR

const FText UUIComboBoxSpace::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
