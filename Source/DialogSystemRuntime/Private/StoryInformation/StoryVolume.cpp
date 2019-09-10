#include "DialogSystemRuntime.h"
#include "StoryVolume.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"
#include "GameFramework/Pawn.h"

bool AStoryVolume::CanActivate(AActor* Other)
{
	unimplemented();
	if (CheckHasKeys.Num() + CheckDontHasKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager(this);

		if (skm->DontHasKey(CheckHasKeys))
			return false;
		
		if (skm->HasKey(CheckDontHasKeys))
			return false;
		
	}

	return true;
}

void AStoryVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	auto playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (playerCharacter != NULL && Other == playerCharacter)
	{
		if (CanActivate(Other))
			Activate();
	}
}

void AStoryVolume::Activate()
{
	unimplemented();
	UE_LOG(DialogModuleLog, Log, TEXT("Activate story volume %s"), *GetFName().ToString());

	if (RemoveKeys.Num() + GiveKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager(this);

		skm->AddKey(GiveKeys);
		
		skm->RemoveKey(RemoveKeys);
	
	}
	
	if (ActivateTriggers.Num() > 0)
	{
		auto stm = UStoryTriggerManager::GetStoryTriggerManager(this);
		for (auto& trigger : ActivateTriggers)
		{
			stm->InvokeTrigger(trigger);
		}
	}

	if (!StartQuest.IsNull())
	{
		auto questProcesstor = UQuestProcessor::GetQuestProcessor(this);
		questProcesstor->StartQuest(StartQuest);
	}

	if (bDestroySelf)
	{
		Destroy();
	}
}