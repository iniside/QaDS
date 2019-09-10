// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogSystemRuntime.h"
#include "QuestComponent.h"
#include "QuestAsset.h"
#include "ObjectKey.h"

FQuestItem& FQuestItemNode::GetOwner(UQuestComponent* Owner)
{
	return Owner->ActiveQuests[OwnerQuest->GetFName()];
}

// Sets default values for this component's properties
UQuestComponent::UQuestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UQuestComponent::StartQuest(TAssetPtr<UQuestAsset> QuestAsset)
{
	UQuestAsset* Quest = QuestAsset.LoadSynchronous();
	if (Quest == nullptr)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: asset is not set"));
		return;
	}

	
	if(ActiveQuests.Contains(Quest->GetFName()))
	{
		UE_LOG(DialogModuleLog, Log, TEXT("Quest %s already active"), *QuestAsset.GetAssetName());
		return;
	}

	FQuestItem StartNode;
	
	auto runtimeQuest = NewObject<UQuestRuntimeAsset>();
	StartNode.Status = EQuestCompleteStatus::Active;
	StartNode.Asset = Quest;
	//runtimeQuest->CreateScript();

	//activeQuests.Add(runtimeQuest);
	//OnQuestStart.Broadcast(runtimeQuest);

	auto root = StartNode.LoadNode(Quest->RootNode);
	WaitStage(root);
}

void UQuestComponent::CompleteStage(UQuestRuntimeNode* Stage)
{
}

void UQuestComponent::WaitStage(FQuestItemNode& Stage)
{
	//if (bIsResetBegin)
	//	return;
	check(Stage.OwnerQuest);
	
	auto Stages = Stage.GetNextStage(this);
	auto bIsOptionalOnly = true;
	
	for (FQuestItemNode& stage : Stages)
	{
		bIsOptionalOnly &= stage.Stage.bIsOptional;
	}
	//
	if (Stages.Num() == 0 || bIsOptionalOnly)
	{
		EndQuest(Stage.OwnerQuest, EQuestCompleteStatus::Completed);
		return;
	}
	
	for (FQuestItemNode& stage : Stages)
	{
		stage.SetStatus(EQuestCompleteStatus::Active);
	
		if (stage.TryComplete(this))
		{
			if (ActiveQuests[Stage.OwnerQuest->GetFName()].Status != EQuestCompleteStatus::Active)
				break;
		}
	}
}

void UQuestComponent::EndQuest(UQuestAsset* Quest, EQuestCompleteStatus QuestStatus)
{
}

TArray<UQuestRuntimeAsset*> UQuestComponent::GetQuests(EQuestCompleteStatus FilterStatus) const
{
	return TArray<UQuestRuntimeAsset*>();
}

