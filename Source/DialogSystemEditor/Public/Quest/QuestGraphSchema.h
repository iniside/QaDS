#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "DialogGraphSchema.h"
#include "QuestGraphSchema.generated.h"

UCLASS()
class UQuestGraphSchema : public UDialogGraphSchema
{
	GENERATED_BODY()

public:
#if ENGINE_MINOR_VERSION < 24
	virtual void GetGraphContextActions(FGraphContextMenuBuilder & ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
#else
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
#endif
};
