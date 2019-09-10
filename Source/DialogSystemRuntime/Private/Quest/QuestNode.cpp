#include "DialogSystemRuntime.h"
#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

TArray<UQuestRuntimeNode*> UQuestRuntimeNode::GetNextStage(class UQuestComponent* Owner)
{
	if (ChildCache.Num() == 0)
	{
		for (auto child : Childs)
		{
			ChildCache.Add(OwnerQuest->LoadNode(child));
		}
	}

	return ChildCache.FilterByPredicate([=](UQuestRuntimeNode* node)
	{
		return node->CkeckForActivate(Owner);
	});
}

void UQuestRuntimeNode::OnChangeStoryKey(const FName& key)
{
	unimplemented();
	//if (Stage.WaitHasKeys.Contains(key)		 ||
	//	Stage.WaitDontHasKeys.Contains(key)  ||
	//	Stage.FailedIfGiveKeys.Contains(key) ||
	//	Stage.FailedIfRemoveKeys.Contains(key))
	//{
	//	TryComplete();
	//}
}

void UQuestRuntimeNode::OnTrigger(const FStoryTrigger& Trigger)
{
	for (auto& cond : Stage.WaitTriggers)
	{
		if (MatchTringger(cond, Trigger))
			break;
	}

	for (auto& cond : Stage.FailedTriggers)
	{
		if (MatchTringger(cond, Trigger))
			break;
	}
}

bool UQuestRuntimeNode::TryComplete()
{
	if (CkeckForFailed())
	{
		SetStatus(EQuestCompleteStatus::Failed);
	}
	else
	{
		if (!CkeckForComplete())
			return false;

		SetStatus(EQuestCompleteStatus::Completed);
	}
	return true;
}

void UQuestRuntimeNode::SetStatus(EQuestCompleteStatus NewStatus)
{
	if (NewStatus == Status)
		return;

	Status = NewStatus;

	switch (Status)
	{
	case EQuestCompleteStatus::None:
		break;
	case EQuestCompleteStatus::Active:
		Activate();
		break;
	case EQuestCompleteStatus::Completed:
		Complete();
		Deactivate();
		break;
	case EQuestCompleteStatus::Failed:
		Failed();
		Deactivate();
		break;
	case EQuestCompleteStatus::Skiped:
		Deactivate();
		break;
	}
}

void UQuestRuntimeNode::Activate()
{
	OwnerQuest->ActiveNodes.Add(this);

	if (TryComplete())
		return;

	if (Stage.WaitHasKeys.Num()			> 0 ||
		Stage.WaitDontHasKeys.Num()		> 0 ||
		Stage.FailedIfGiveKeys.Num()	> 0 ||
		Stage.FailedIfRemoveKeys.Num()	> 0)
	{
		Processor->StoryKeyManager->OnKeyRemoveBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		Processor->StoryKeyManager->OnKeyAddBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	}

	if (Stage.FailedTriggers.Num() > 0 || Stage.WaitTriggers.Num() > 0)
	{
		Processor->StoryTriggerManager->OnTriggerInvoke.AddDynamic(this, &UQuestRuntimeNode::OnTrigger);
	}
}

void UQuestRuntimeNode::Failed()
{
	if (Stage.bFailedQuest || OwnerQuest->ActiveNodes.Num() == 1)
	{
		Processor->EndQuest(OwnerQuest, EQuestCompleteStatus::Failed);
	}
}

void UQuestRuntimeNode::Complete()
{
	//if (Stage.ChangeOderActiveStagesState != EQuestCompleteStatus::None)
	//{
	//	auto nodes = OwnerQuest->ActiveNodes;
	//	for (auto stage : nodes)
	//	{
	//		if (stage == this)
	//			continue;
	//
	//		stage->SetStatus(Stage.ChangeOderActiveStagesState);
	//	}
	//}
	//
	//if (Stage.ChangeQuestState != EQuestCompleteStatus::None)
	//{
	//	Processor->EndQuest(OwnerQuest, Stage.ChangeQuestState);
	//}
	//
	//for (auto& Event : Stage.Action)
	//	Event.Invoke(this);
}

void UQuestRuntimeNode::Deactivate()
{
	OwnerQuest->ActiveNodes.Remove(this);
	OwnerQuest->ArchiveNodes.Add(this);

	Processor->CompleteStage(this);

	Processor->StoryKeyManager->OnKeyRemoveBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	Processor->StoryKeyManager->OnKeyAddBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	Processor->StoryTriggerManager->OnTriggerInvoke.RemoveDynamic(this, &UQuestRuntimeNode::OnTrigger);
}

bool UQuestRuntimeNode::MatchTringger(FStoryTriggerCondition& condition, const FStoryTrigger& trigger)
{
	if (condition.TriggerName != trigger.TriggerName)
		return false;

	for (auto kpv : trigger.Params)
	{
		if (!condition.ParamsMasks.Contains(kpv.Key))
			return false;

		auto filter = condition.ParamsMasks[kpv.Key];

		if (!MatchTringgerParam(kpv.Value, filter))
			return false;
	}

	condition.TotalCount -= trigger.Count;

	if (condition.TotalCount <= 0)
		TryComplete();

	return true;
}

bool UQuestRuntimeNode::MatchTringgerParam(const FString& value, const FString& filter)
{
	if (filter == "*")
		return true;

	if (filter == value)
		return true;

	return false;
}

bool UQuestRuntimeNode::CkeckForActivate(class UQuestComponent* Owner)
{
	//unimplemented();
	//if (Processor->StoryKeyManager->DontHasKey(Stage.CheckHasKeys))
	//	return false;
	//
	//if (Processor->StoryKeyManager->HasKey(Stage.CheckDontHasKeys))
	//	return false;
	//
	//for (auto& Conditions : Stage.Predicate)
	//{
	//	if (!Conditions.InvokeCheck(this))
	//		return false;
	//}

	return true;
}

bool UQuestRuntimeNode::CkeckForComplete()
{
	//unimplemented();
	//for (auto& cond : Stage.WaitTriggers)
	//{
	//	if (cond.TotalCount != 0)
	//		return false;
	//}
	//
	//if (Processor->StoryKeyManager->DontHasKey(Stage.WaitHasKeys))
	//	return false;
	//
	//if (Processor->StoryKeyManager->HasKey(Stage.WaitDontHasKeys))
	//	return false;
	//
	//for (auto& Conditions : Stage.WaitPredicate)
	//{
	//	if (!Conditions.InvokeCheck(this))
	//		return false;
	//}

	return true;
}

bool UQuestRuntimeNode::CkeckForFailed()
{
	//unimplemented();
	//for (auto& cond : Stage.FailedTriggers)
	//{
	//	if (cond.TotalCount == 0)
	//		return true;
	//}
	//
	//if (Processor->StoryKeyManager->HasKey(Stage.FailedIfGiveKeys))
	//	return true;
	//
	//
	//if (Processor->StoryKeyManager->DontHasKey(Stage.FailedIfRemoveKeys))
	//	return true;
	//
	//
	//for (auto& Conditions : Stage.FailedPredicate)
	//{
	//	if (Conditions.InvokeCheck(this))
	//		return true;
	//}

	return false;
}

FString FStoryTriggerCondition::ToString() const
{
	auto result = TriggerName.ToString() + "[";

	TArray<FString> params;
	ParamsMasks.GenerateValueArray(params);

	for (int i = 0; i < params.Num(); i++)
	{
		if (i != 0)
			result += ", ";

		result += params[i];
	}

	result += "]";

	if(TotalCount > 1)
		result += " x" + FString::FromInt(TotalCount);

	return result;
}