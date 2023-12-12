// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#if WITH_EDITORONLY_DATA

#include "PropertyEditorModule.h"
#include "IDetailCustomization.h"

class FCogAiMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiActionMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiActionMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiWhoamiSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiWhoamiSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiActionsSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiActionsSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiContextSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiContextSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiInspectSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiInspectSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiPerceptionSystemMessageDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiPerceptionSystemMessageDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FCogAiNpcComponentDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FCogAiNpcComponentDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

class FVeAiCharacterDetails : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance() {
		return MakeShareable(new FVeAiCharacterDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};

void RegisterCogAiDetailsCustomization();

void UnregisterCogAiDetailsCustomization();

#endif // WITH_EDITORONLY_DATA