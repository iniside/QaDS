#include "DialogSystemRuntime.h"
#include "DialogPhraseNode.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"
#include "StoryInformationManager.h"
#include "QuestProcessor.h"

void UDialogPhraseNode::Invoke(UDialogProcessor* processor)
{
	check(processor);

	processor->StoryKeyManager->AddKey(Data.GiveKeys);

	processor->StoryKeyManager->RemoveKey(Data.RemoveKeys);

	for (auto& Event : Data.Action)
		Event.Invoke(processor);
	
	if (!Data.StartQuest.ToSoftObjectPath().IsNull())
	{
		UQuestProcessor::GetQuestProcessor(processor)->StartQuest(Data.StartQuest.ToSoftObjectPath().TryLoad());
	}
	
	if (Data.Source == EDialogPhraseSource::Player)
	{
		processor->OnShowPlayerPhrase.Broadcast(Data);
	}
	else
	{
		processor->OnShowNPCPhrase.Broadcast(Data);
	}

	if (processor->IsPlayerNext)
	{
		TArray<FDialogPhraseShortInfo> playerPhrases;

		for (auto nextNode : processor->NextNodes)
		{
			FDialogPhraseShortInfo answerInfo;
			answerInfo.Text = nextNode->Data.Text;
			answerInfo.UID = nextNode->Data.UID;

			playerPhrases.Add(answerInfo);
		}

		processor->OnChangePhraseVariant.Broadcast(playerPhrases);
	}
	else
	{
		processor->DelayNext();
	}
}

bool UDialogPhraseNode::Check(UDialogProcessor* processor)
{
	unimplemented();

	if (processor->StoryKeyManager->DontHasKey(Data.CheckHasKeys))
		return false;
	
	if (processor->StoryKeyManager->HasKey(Data.CheckDontHasKeys))
		return false;

	for (auto& Conditions : Data.Predicate)
	{
		if (!Conditions.InvokeCheck(processor))
			return false;
	}

	return true;
}

TArray<UDialogPhraseNode*> UDialogPhraseNode::GetNextPhrases(UDialogProcessor* processor)
{
	TArray<UDialogPhraseNode*> result;
	result.Add(this);

    return result;
}
