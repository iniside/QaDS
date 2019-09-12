// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogSystemRuntime.h"
#include "QuestTypes.h"

QuestTypes::QuestTypes()
{
}

QuestTypes::~QuestTypes()
{
}


void FQuestGameplayTagCountContainer::Notify_StackCountChange(const FGameplayTag& Tag)
{
	// The purpose of this function is to let anyone listening on the EGameplayTagEventType::AnyCountChange event know that the 
	// stack count of a GE that was backing this GE has changed. We do not update our internal map/count with this info, since that
	// map only counts the number of GE/sources that are giving that tag.
	FGameplayTagContainer TagAndParentsContainer = Tag.GetGameplayTagParents();
	for (auto CompleteTagIt = TagAndParentsContainer.CreateConstIterator(); CompleteTagIt; ++CompleteTagIt)
	{
		const FGameplayTag& CurTag = *CompleteTagIt;
		FDelegateInfo* DelegateInfo = GameplayTagEventMap.Find(CurTag);
		if (DelegateInfo)
		{
			int32 TagCount = GameplayTagCountMap.FindOrAdd(CurTag);
			DelegateInfo->OnAnyChange.Broadcast(CurTag, TagCount);
		}
	}
}

FQuestOnGameplayEffectTagCountChanged& FQuestGameplayTagCountContainer::RegisterGameplayTagEvent(const FGameplayTag& Tag, EGameplayTagEventType::Type EventType)
{
	FDelegateInfo& Info = GameplayTagEventMap.FindOrAdd(Tag);

	if (EventType == EGameplayTagEventType::NewOrRemoved)
	{
		return Info.OnNewOrRemove;
	}

	return Info.OnAnyChange;
}

void FQuestGameplayTagCountContainer::Reset()
{
	GameplayTagEventMap.Reset();
	GameplayTagCountMap.Reset();
	ExplicitTagCountMap.Reset();
	ExplicitTags.Reset();
	OnAnyTagChangeDelegate.Clear();
}

bool FQuestGameplayTagCountContainer::UpdateTagMap_Internal(const FGameplayTag& Tag, int32 CountDelta)
{
	const bool bTagAlreadyExplicitlyExists = ExplicitTags.HasTagExact(Tag);

	// Need special case handling to maintain the explicit tag list correctly, adding the tag to the list if it didn't previously exist and a
	// positive delta comes in, and removing it from the list if it did exist and a negative delta comes in.
	if (!bTagAlreadyExplicitlyExists)
	{
		// Brand new tag with a positive delta needs to be explicitly added
		if (CountDelta > 0)
		{
			ExplicitTags.AddTag(Tag);
		}
		// Block attempted reduction of non-explicit tags, as they were never truly added to the container directly
		else
		{
			// only warn about tags that are in the container but will not be removed because they aren't explicitly in the container
			if (ExplicitTags.HasTag(Tag))
			{
				ABILITY_LOG(Warning, TEXT("Attempted to remove tag: %s from tag count container, but it is not explicitly in the container!"), *Tag.ToString());
			}
			return false;
		}
	}

	// Update the explicit tag count map. This has to be separate than the map below because otherwise the count of nested tags ends up wrong
	int32& ExistingCount = ExplicitTagCountMap.FindOrAdd(Tag);

	ExistingCount = FMath::Max(ExistingCount + CountDelta, 0);

	// If our new count is 0, remove us from the explicit tag list
	if (ExistingCount <= 0)
	{
		// Remove from the explicit list
		ExplicitTags.RemoveTag(Tag);
	}

	// Check if change delegates are required to fire for the tag or any of its parents based on the count change
	FGameplayTagContainer TagAndParentsContainer = Tag.GetGameplayTagParents();
	bool CreatedSignificantChange = false;
	for (auto CompleteTagIt = TagAndParentsContainer.CreateConstIterator(); CompleteTagIt; ++CompleteTagIt)
	{
		const FGameplayTag& CurTag = *CompleteTagIt;

		// Get the current count of the specified tag. NOTE: Stored as a reference, so subsequent changes propogate to the map.
		int32& TagCountRef = GameplayTagCountMap.FindOrAdd(CurTag);

		const int32 OldCount = TagCountRef;

		// Apply the delta to the count in the map
		int32 NewTagCount = FMath::Max(OldCount + CountDelta, 0);
		TagCountRef = NewTagCount;

		// If a significant change (new addition or total removal) occurred, trigger related delegates
		bool SignificantChange = (OldCount == 0 || NewTagCount == 0);
		CreatedSignificantChange |= SignificantChange;
		if (SignificantChange)
		{
			OnAnyTagChangeDelegate.Broadcast(CurTag, NewTagCount);
		}

		FDelegateInfo* DelegateInfo = GameplayTagEventMap.Find(CurTag);
		if (DelegateInfo)
		{
			// Prior to calling OnAnyChange delegate, copy our OnNewOrRemove delegate, since things listening to OnAnyChange could add or remove 
			// to this map causing our pointer to become invalid.
			FQuestOnGameplayEffectTagCountChanged OnNewOrRemoveLocalCopy = DelegateInfo->OnNewOrRemove;

			DelegateInfo->OnAnyChange.Broadcast(CurTag, NewTagCount);
			if (SignificantChange)
			{
				OnNewOrRemoveLocalCopy.Broadcast(CurTag, NewTagCount);
			}
		}
	}

	return CreatedSignificantChange;
}