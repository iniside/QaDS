#pragma once

#include "QuestStageEvent.h"
#include "StoryTriggerManager.h"
#include "GameplayTagContainer.h"
#include "QuestNode.generated.h"

UENUM(BlueprintType)
enum class EQuestCompleteStatus : uint8
{
	None,
	Active,
	Failed,
	Completed,
	Skiped
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FStoryTriggerCondition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TriggerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> ParamsMasks;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestStageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stage")
	FGuid UID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Caption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	bool bIsOptional;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TMap<FName, FString> AditionalData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	bool bGenerateEvents = true;

	/* I must have all I these tags to meet requirements */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Conditions")
	FGameplayTagContainer CheckHasKeys;

	/* I must not have any of these tags to meet requirements. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Conditions")
	FGameplayTagContainer CheckDontHasKeys;

	/* Custom predicate to check if we meet requirements */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FQuestStageCondition> Predicate;

	/* I will try to complete this stage when all of these are added. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Task")
	FGameplayTagContainer WaitHasKeys;

	/* I will try to complete this stage when all of these tags are removed. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Task")
	FGameplayTagContainer WaitDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FStoryTriggerCondition> WaitTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FQuestStageCondition> WaitPredicate;

	/* If I get all of these tags I will fail this stage. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Failed")
	FGameplayTagContainer FailedIfGiveKeys;

	/* If all of these tags will be removed I will fail this stage. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Failed")
	FGameplayTagContainer FailedIfRemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FStoryTriggerCondition> FailedTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FQuestStageCondition> FailedPredicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	bool bFailedQuest = true;

	/* I will give you thse tags if you complete this stage. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Complete")
	FGameplayTagContainer GiveKeys;

	/* I will remove these tags if you complete this stage. */
	UPROPERTY(EditAnywhere, meta = (GameplayTagFilter = "GameplayQuest"), BlueprintReadOnly, Category = "Complete")
	FGameplayTagContainer RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FQuestStageEvent> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	EQuestCompleteStatus ChangeQuestState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	EQuestCompleteStatus ChangeOderActiveStagesState;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestRuntimeNode : public UObject
{
	GENERATED_BODY()

	TArray<UQuestRuntimeNode*> ChildCache;

	void Activate();
	void Failed();
	void Complete();
	void Deactivate();

	bool CkeckForActivate(class UQuestComponent* Owner);
	bool CkeckForComplete();
	bool CkeckForFailed();

	bool MatchTringger(FStoryTriggerCondition& condition, const FStoryTrigger& trigger);
	bool MatchTringgerParam(const FString& value, const FString& filter);

public:
	UPROPERTY()
	class UQuestProcessor* Processor;

	UPROPERTY()
	class UQuestRuntimeAsset* OwnerQuest;

	UPROPERTY()
	TArray<FGuid> Childs;

	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestStageInfo Stage;

	bool TryComplete();
	void SetStatus(EQuestCompleteStatus NewStatus);
	
	TArray<UQuestRuntimeNode*> GetNextStage(class UQuestComponent* Owner);

private:
	UFUNCTION()
	void OnChangeStoryKey(const FName& key);

	UFUNCTION()
	void OnTrigger(const FStoryTrigger& Trigger);
};