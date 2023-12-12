// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/UIComboBoxUser.h"
#include "Widgets/SNullWidget.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"
#include "Styling/UMGCoreStyle.h"

#define LOCTEXT_NAMESPACE "VeUI"

/////////////////////////////////////////////////////
// UUIComboBoxUser

static FComboBoxStyle* DefaultComboBoxUserStyle = nullptr;
static FTableRowStyle* DefaultComboBoxUserRowStyle = nullptr;

#if WITH_EDITOR
static FComboBoxStyle* EditorComboBoxUserStyle = nullptr;
static FTableRowStyle* EditorComboBoxUserRowStyle = nullptr;
#endif 

UUIComboBoxUser::UUIComboBoxUser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (DefaultComboBoxUserStyle == nullptr)
	{
		DefaultComboBoxUserStyle = new FComboBoxStyle(FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"));

		// Unlink UMG default colors.
		DefaultComboBoxUserStyle->UnlinkColors();
	}

	if (DefaultComboBoxUserRowStyle == nullptr)
	{
		DefaultComboBoxUserRowStyle = new FTableRowStyle(FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG default colors.
		DefaultComboBoxUserRowStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultComboBoxUserStyle;
	ItemStyle = *DefaultComboBoxUserRowStyle;

#if WITH_EDITOR 
	if (EditorComboBoxUserStyle == nullptr)
	{
		EditorComboBoxUserStyle = new FComboBoxStyle(FCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("EditorUtilityComboBox"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxUserStyle->UnlinkColors();
	}

	if (EditorComboBoxUserRowStyle == nullptr)
	{
		EditorComboBoxUserRowStyle = new FTableRowStyle(FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"));

		// Unlink UMG Editor colors from the editor settings colors.
		EditorComboBoxUserRowStyle->UnlinkColors();
	}

	if (IsEditorWidget())
	{
		WidgetStyle = *EditorComboBoxUserStyle;
		ItemStyle = *EditorComboBoxUserRowStyle;

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

void UUIComboBoxUser::PostInitProperties()
{
	Super::PostInitProperties();

	// Initialize the set of options from the default set only once.
	for (const FVeUserMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}
}

void UUIComboBoxUser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyComboBox.Reset();
	ComboBoxContent.Reset();
}

void UUIComboBoxUser::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEditorObjectVersion::GUID);
}

void UUIComboBoxUser::PostLoad()
{
	Super::PostLoad();

	// Initialize the set of options from the default set only once.
	for (const FVeUserMetadata& DefaultOption : DefaultOptions)
	{
		AddOption(DefaultOption);
	}

	if (GetLinkerCustomVersion(FEditorObjectVersion::GUID) < FEditorObjectVersion::ComboBoxControllerSupportUpdate)
	{
		EnableGamepadNavigationMode = false;
	}
}

TSharedRef<SWidget> UUIComboBoxUser::RebuildWidget()
{
	int32 InitialIndex = FindOptionIndex(SelectedOption.Id);
	if ( InitialIndex != -1 )
	{
		CurrentOptionPtr = Options[InitialIndex];
	}

	MyComboBox =
		SNew(SComboBox< TSharedPtr<FVeUserMetadata> >)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.OptionsSource(&Options)
		.InitiallySelectedItem(CurrentOptionPtr)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(HasDownArrow)
		.EnableGamepadNavigationMode(EnableGamepadNavigationMode)
		.OnGenerateWidget(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVeUserMetadata> >::FOnGenerateWidget, HandleGenerateWidget))
		.OnSelectionChanged(BIND_UOBJECT_DELEGATE(SComboBox< TSharedPtr<FVeUserMetadata> >::FOnSelectionChanged, HandleSelectionChanged))
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

void UUIComboBoxUser::AddOption(const FVeUserMetadata& Option)
{
	Options.Add(MakeShareable(new FVeUserMetadata(Option)));

	RefreshOptions();
}

bool UUIComboBoxUser::RemoveOption(const FGuid& Id)
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

int32 UUIComboBoxUser::FindOptionIndex(const FGuid& Id) const {
	for ( int32 OptionIndex = 0; OptionIndex < Options.Num(); OptionIndex++ ) {
		const TSharedPtr<FVeUserMetadata>& OptionAtIndex = Options[OptionIndex];

		if ( ( *OptionAtIndex ).Id == Id ) {
			return OptionIndex;
		}
	}

	return -1;
}

FVeUserMetadata UUIComboBoxUser::GetOptionAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < Options.Num())
	{
		return *(Options[Index]);
	}
	return FVeUserMetadata();
}

void UUIComboBoxUser::ClearOptions()
{
	ClearSelection();

	Options.Empty();

	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxUser::ClearSelection()
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

void UUIComboBoxUser::RefreshOptions()
{
	if ( MyComboBox.IsValid() )
	{
		MyComboBox->RefreshOptions();
	}
}

void UUIComboBoxUser::SetSelectedOption(const FGuid& Id) {
	int32 InitialIndex = FindOptionIndex(Id);
	SetSelectedIndex(InitialIndex);
}

void UUIComboBoxUser::SetSelectedIndex(const int32 Index)
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
FVeUserMetadata UUIComboBoxUser::GetSelectedOption() const
{
	if (CurrentOptionPtr.IsValid())
	{
		return *CurrentOptionPtr;
	}
	return FVeUserMetadata();
}

int32 UUIComboBoxUser::GetSelectedIndex() const
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

int32 UUIComboBoxUser::GetOptionCount() const
{
	return Options.Num();
}

bool UUIComboBoxUser::IsOpen() const
{
	return MyComboBox.IsValid() && MyComboBox->IsOpen();
}

void UUIComboBoxUser::UpdateOrGenerateWidget(TSharedPtr<FVeUserMetadata> Item)
{
	// If no custom widget was supplied and the default STextBlock already exists,
	// just update its text instead of rebuilding the widget.
	if (DefaultComboBoxContent.IsValid() && (IsDesignTime() || OnGenerateWidgetEvent.IsBound()))
	{
		const FVeUserMetadata UserItem = Item.IsValid() ? *Item : FVeUserMetadata();
		if (UserItem.Name.IsEmpty()) {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(UserItem.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
		} else {
			DefaultComboBoxContent.Pin()->SetText(FText::FromString(UserItem.Name));
		}
	}
	else
	{
		DefaultComboBoxContent.Reset();
		ComboBoxContent->SetContent(HandleGenerateWidget(Item));
	}
}

TSharedRef<SWidget> UUIComboBoxUser::HandleGenerateWidget(TSharedPtr<FVeUserMetadata> Item) const
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

void UUIComboBoxUser::HandleSelectionChanged(TSharedPtr<FVeUserMetadata> Item, ESelectInfo::Type SelectionType)
{
	CurrentOptionPtr = Item;
	SelectedOption = CurrentOptionPtr.IsValid() ? CurrentOptionPtr.ToSharedRef().Get() : FVeUserMetadata();

	// When the selection changes we always generate another widget to represent the content area of the combobox.
	if ( ComboBoxContent.IsValid() )
	{
		UpdateOrGenerateWidget(CurrentOptionPtr);
	}

	if ( !IsDesignTime() )
	{
		OnSelectionChanged.Broadcast(Item.IsValid() ? *Item : FVeUserMetadata(), SelectionType);
	}
}

void UUIComboBoxUser::HandleOpening()
{
	OnOpening.Broadcast();
}

#if WITH_EDITOR

const FText UUIComboBoxUser::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMEUser
