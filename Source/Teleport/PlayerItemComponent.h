// �v���C���[���A�C�e�����擾���邽�߂̃N���X
//
// 2021/05/19 �n粗��� �X�R�A���Ǘ��ł���

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UPlayerItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// �X�R�A�ϐ�
	int m_Score;

public:
	// �X�R�A�ǉ��֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void AddScore(int _addScore = 1)
	{
		m_Score += _addScore;
	}

	// �X�R�A�擾�֐�
	//
	//
	UFUNCTION(BlueprintPure)
		int GetScore()
	{
		return m_Score;
	}
};
