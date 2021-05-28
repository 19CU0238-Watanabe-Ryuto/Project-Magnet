// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemShootComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UItemShootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemShootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// �����o�����ʒu
	FVector m_BeginShootLocation;

	// ���݂̈З�
	int m_NowDamage;

public:
	UFUNCTION(BlueprintPure)
		int GetNowDamage()
	{
		return m_NowDamage;
	}

public:
	UPROPERTY(BlueprintReadWrite)
	bool m_IsShoot;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	int m_InitialHitDamage;

public:
	// �������Ƃ��̊֐�
	//
	// �������F�I�u�W�F�N�g�𔽔��������ʒu
	UFUNCTION(BlueprintCallable)
	void Shoot(FVector _shootPos);

	// ���������̌v�Z
	//
	// �������F�Փ˂����ʒu
	// �������F�З͂��������鋗��	�i��m���ƂɁ��_���[�W���� �Ƃ������� ���̕����j
	// ��O�����F��������З�		�i��m���ƂɁ��_���[�W���� �Ƃ������� ���̕����j
	UFUNCTION(BlueprintCallable)
		int AttenuationDamage(FVector _hitPos, float _attenuationDistance = 100.0f, int _attenuationAmount = 1);

	// �ǂɂ����������Ɍ����o���ʒu�E�З͂����X�V����֐�
	//
	// �������F�Փ˂����ʒu
	// �������F�ǂɏՓ˂����Ƃ��Ɋ|������l
	// ��O�����F�З͂��������鋗��	�i��m���ƂɁ��_���[�W���� �Ƃ������� ���̕����j
	// ��l�����F��������З�		�i��m���ƂɁ��_���[�W���� �Ƃ������� ���̕����j
	UFUNCTION(BlueprintCallable)
		int HitAttenuationDamage(FVector _hitPos, float _attenuationRatio = 0.75f, float _attenuationDistance = 100.0f, int _attenuationAmount = 1);
};
