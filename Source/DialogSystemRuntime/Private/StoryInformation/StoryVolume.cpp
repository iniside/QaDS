#include "StoryVolume.h"
#include "DialogSystemRuntime.h"

#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"
#include "GameFramework/Pawn.h"
#include "QuestInterface.h"
#include "QuestComponent.h"

bool AStoryVolume::CanActivate(AActor* Other)
{

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
		//if (CanActivate(Other))
			Activate(Other);
	}
}

void AStoryVolume::Activate(AActor* Other)
{
	if(!Other->HasAuthority())
	{
		return;
	}
	UE_LOG(DialogModuleLog, Log, TEXT("Activate story volume %s"), *GetFName().ToString());

	IQuestInterface* QI = Cast<IQuestInterface>(Other);
	UQuestComponent* QE = QI->GetQuestComponent();

	if (ActivateTriggers.Num() > 0)
	{
		for (auto& trigger : ActivateTriggers)
		{
			QE->InvokeTrigger(trigger);
		}
	}

	if (!StartQuest.IsNull())
	{
		QE->StartQuest(StartQuest);
	}

	if (bDestroySelf)
	{
		Destroy();
	}
}