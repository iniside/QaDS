// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogSystemRuntime.h"
#include "QuestComponent.h"
#include "QuestAsset.h"
#include "ObjectKey.h"
#include "QaDSSettings.h"

FQuestItem& FQuestItemNode::GetOwner(UQuestComponent* Owner)
{
	return Owner->ActiveQuests[QuestAsset->GetFName()];
}

bool FQuestItemNode::CkeckForActivate(UQuestComponent* Owner) const
{
	if(Owner->QuestTags.Num() > 0)
	{
		if (!Owner->QuestTags.HasAll(GetStage().CheckHasKeys))
		{
			return false;
		}
		if (Owner->QuestTags.HasAll(GetStage().CheckDontHasKeys))
		{
			return false;
		}
	}
	
	const TArray<FQuestStageCondition>& Predicates = GetStage().Predicate;
	
	for (auto& Conditions : Predicates)
	{
		//if (!Conditions.InvokeCheck(this, Owner))
		//	return false;
	}

	return true;
}

void FQuestItemNode::Activate(UQuestComponent* Owner)
{
	FQuestItem& OwnerQuest = GetOwner(Owner);
	
	OwnerQuest.ActiveNodes.Add(*this);
	
	if (TryComplete(Owner))
		return;

	const FQuestStageInfo& Stage = GetStage();
	
	if (Stage.WaitHasKeys.Num() > 0 ||
		Stage.WaitDontHasKeys.Num() > 0 ||
		Stage.FailedIfGiveKeys.Num() > 0 ||
		Stage.FailedIfRemoveKeys.Num() > 0)
	{
	//	Processor->StoryKeyManager->OnKeyRemoveBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	//	Processor->StoryKeyManager->OnKeyAddBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	}
	
	if (Stage.FailedTriggers.Num() > 0 || Stage.WaitTriggers.Num() > 0)
	{
	//	Processor->StoryTriggerManager->OnTriggerInvoke.AddDynamic(this, &UQuestRuntimeNode::OnTrigger);
	}
}

void FQuestItemNode::Failed(UQuestComponent* Owner)
{
	if (GetStage().bFailedQuest || GetOwner(Owner).ActiveNodes.Num() == 1)
	{
		Owner->EndQuest(QuestAsset.Get(), EQuestCompleteStatus::Failed);
	}
}

void FQuestItemNode::Complete(UQuestComponent* Owner)
{
	const FQuestStageInfo& Stage = GetStage();
	
	if (Stage.ChangeOderActiveStagesState != EQuestCompleteStatus::None)
	{
		TArray<FQuestItemNode>& nodes = GetOwner(Owner).ActiveNodes;
		for (FQuestItemNode& stage : nodes)
		{
			if (stage == this)
				continue;
	
			stage.SetStatus(Stage.ChangeOderActiveStagesState, Owner);
		}
	}
	
	if (Stage.ChangeQuestState != EQuestCompleteStatus::None)
	{
		Owner->EndQuest(QuestAsset.Get(), Stage.ChangeQuestState);
	}
	
	//for (auto& Event : Stage.Action)
	//	Event.Invoke(this);
}

void FQuestItemNode::Deactivate(UQuestComponent* Owner)
{
	GetOwner(Owner).ActiveNodes.Remove(*this);
	GetOwner(Owner).ArchiveNodes.Add(*this);
	//
	Owner->CompleteStage(*this);
	//
	//Processor->StoryKeyManager->OnKeyRemoveBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	//Processor->StoryKeyManager->OnKeyAddBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	//Processor->StoryTriggerManager->OnTriggerInvoke.RemoveDynamic(this, &UQuestRuntimeNode::OnTrigger);
}

void FQuestItemNode::SetStatus(EQuestCompleteStatus NewStatus, UQuestComponent* Owner)
{
	if (NewStatus == Status)
		return;

	Status = NewStatus;

	switch (Status)
	{
	case EQuestCompleteStatus::None:
		break;
	case EQuestCompleteStatus::Active:
		Activate(Owner);
		break;
	case EQuestCompleteStatus::Completed:
		Complete(Owner);
		Deactivate(Owner);
		break;
	case EQuestCompleteStatus::Failed:
		Failed(Owner);
		Deactivate(Owner);
		break;
	case EQuestCompleteStatus::Skiped:
		Deactivate(Owner);
		break;
	}
}

bool FQuestItemNode::CkeckForComplete(UQuestComponent* Owner)
{
	const FQuestStageInfo& Stage = GetStage();
	//for (auto& cond : Stage.WaitTriggers)
	//{
	//	if (cond.TotalCount != 0)
	//		return false;
	//}
	//

	if (Owner->QuestTags.Num() > 0)
	{
		if (Owner->NotHaveAllTags(Stage.WaitHasKeys))
			return false;

		if (Owner->HasAllTags(Stage.WaitDontHasKeys))
			return false;
	}
	
	for (auto& Conditions : Stage.WaitPredicate)
	{
		if (!Conditions.InvokeCheck(this, Owner))
			return false;
	}
	
	return true;
}

bool FQuestItemNode::CkeckForFailed(UQuestComponent* Owner)
{
	//for (auto& cond : Stage.FailedTriggers)
	//{
	//	if (cond.TotalCount == 0)
	//		return true;
	//}
	//
	const FQuestStageInfo& Stage = GetStage();
	if(Owner->QuestTags.Num() > 0)
	{
		if (Owner->HasAllTags(Stage.FailedIfGiveKeys))
			return true;


		if (Owner->NotHaveAllTags(Stage.FailedIfRemoveKeys))
			return true;
	}
	
	for (auto& Conditions : Stage.FailedPredicate)
	{
		if (Conditions.InvokeCheck(this, Owner))
			return true;
	}
	
	return false;
}

bool FQuestItemNode::TryComplete(UQuestComponent* Owner)
{
	if (CkeckForFailed(Owner))
	{
		SetStatus(EQuestCompleteStatus::Failed, Owner);
	}
	else
	{
		if (!CkeckForComplete(Owner))
			return false;

		SetStatus(EQuestCompleteStatus::Completed, Owner);
	}
	return true;
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
	StartNode.Status = EQuestCompleteStatus::Active;
	StartNode.Asset = Quest;
	TSubclassOf<AQuestScript> Cls = Quest->QuestScriptClass.LoadSynchronous();

	StartNode.Script = GetWorld()->SpawnActor<AQuestScript>(Cls);

	ActiveQuests.Add(Quest->GetFName(), StartNode);
	//activeQuests.Add(runtimeQuest);
	//OnQuestStart.Broadcast(runtimeQuest);

	auto root = StartNode.LoadNode(Quest->RootNode);
	WaitStage(root);
}

void UQuestComponent::CompleteStage(FQuestItemNode& StageNode)
{
	//if (bIsResetBegin)
	//	return;
	
	check(StageNode.QuestAsset.Get());

	if (!ActiveQuests.Contains(StageNode.QuestAsset->GetFName()))
		return;

	const FQuestStageInfo& Stage = StageNode.GetStage();
	
	auto isNeedEvents = Stage.bGenerateEvents;
	isNeedEvents = !Stage.Caption.IsEmpty();

	auto isEmpty = Stage.Caption.IsEmpty();
	auto generateForEmpty = !GetDefault<UQaDSSettings>()->bDontGenerateEventForEmptyQuestNode;

	if (generateForEmpty && Stage.bGenerateEvents || !generateForEmpty && !isEmpty)
	{
		//OnStageComplete.Broadcast(StageNode->OwnerQuest, StageNode->Stage);
	}

	if (StageNode.Status == EQuestCompleteStatus::Completed)
		WaitStage(StageNode);
}

void UQuestComponent::WaitStage(FQuestItemNode& StageNode)
{
	//if (bIsResetBegin)
	//	return;
	check(StageNode.QuestAsset.Get());

	const FQuestStageInfo& Stage = StageNode.GetStage();
	
	auto Stages = StageNode.GetNextStage(this);
	auto bIsOptionalOnly = true;
	
	for (const FQuestItemNode& stage : Stages)
	{
		bIsOptionalOnly &= stage.GetStage().bIsOptional;
	}
	//
	if (Stages.Num() == 0 || bIsOptionalOnly)
	{
		EndQuest(StageNode.QuestAsset.Get(), EQuestCompleteStatus::Completed);
		return;
	}
	
	for (FQuestItemNode& stage : Stages)
	{
		stage.SetStatus(EQuestCompleteStatus::Active, this);
	
		if (stage.TryComplete(this))
		{
			if (ActiveQuests[StageNode.QuestAsset->GetFName()].Status != EQuestCompleteStatus::Active)
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

