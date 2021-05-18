// �v���C���[���A�C�e�����擾���邽�߂̃N���X
//
// 2021/05/18 �n粗��� �J�����̌����Ă�������̃x�N�g�����擾�ł���悤�ɂ���

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
		void AddScore(int _score)
	{
		m_Score += _score;
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
