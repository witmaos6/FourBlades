// Fill out your copyright notice in the Description page of Project Settings.


#include "Cosmetics/FBControllerComponent_CharacterParts.h"
#include "FBPawnComponent_CharacterParts.h"

UFBControllerComponent_CharacterParts::UFBControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFBControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UFBControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (UFBPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UFBPawnComponent_CharacterParts>() : nullptr)
	{
		for (FFBControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	if (UFBPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UFBPawnComponent_CharacterParts>() : nullptr)
	{
		for (FFBControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(!Entry.Handle.IsValid());
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}
}

UFBPawnComponent_CharacterParts* UFBControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UFBPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UFBControllerComponent_CharacterParts::AddCharacterPart(const FFBCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void UFBControllerComponent_CharacterParts::AddCharacterPartInternal(const FFBCharacterPart& NewPart)
{
	FFBControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;

	if (UFBPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		// 라이라에서는 ControllerComponent는 저장만 하고 적용은 하지 않는다.
	}
}

void UFBControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();

	Super::EndPlay(EndPlayReason);
}

void UFBControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UFBPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FFBControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}
