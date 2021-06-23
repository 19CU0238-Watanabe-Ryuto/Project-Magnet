// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬
// 2021/05/30 �n粗��� �N�������o�����̂�������悤��
// 2021/06/01 �n粗��� �N�������o�����̂� ->�N���������Ă���Ƃ��ɒN�������Ă���̂�������悤��
// 2021/06/04 �n粗��� �����Ă���Actor���O�����烊�Z�b�g����֐���ǉ�
// 2021/06/09 �n粗��� ���˂̏�����ǉ�

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemShootComponent.generated.h"

class UProjectileMovementComponent;

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
	// ���̃R���|�[�l���g���t���Ă���Actor�̃X�^�e�B�b�N���b�V���iRoot�j
	UStaticMeshComponent* m_StaticMeshComp;

	// ���̃R���|�[�l���g���t���Ă���Actor��ProjectileMovement
	UProjectileMovementComponent* m_ProjectileMovement;

	// �N�������Ă���̂�
	AActor* m_WhoHave;

	// ���˂����Ƃ��̃^�[�Q�b�gActor
	AActor* m_ShootTargetActor;

	// �����o�����ʒu
	FVector m_BeginShootLocation;

	// ���˂���Ƃ��̃^�[�Q�b�g�ʒu
	FVector m_ShootTargetLocation;

	// ���݂̈З�
	int m_NowDamage;

	// �����o������
	float m_ShootPower;

	// �V���[�g���̃^�[�Q�b�g�Ƃ̋���
	float m_TargetLength;

	bool m_ActorResetFlg;

private:

	// �������Ƃ��̊֐�
	//
	// �������F�I�u�W�F�N�g�𔽔��������ʒu
	void Shoot(float _time);

public:
	UFUNCTION(BlueprintPure)
		int GetNowDamage()
	{
		return m_NowDamage;
	}

	UFUNCTION(BlueprintPure)
		AActor* GetWhoHave()
	{
		return m_WhoHave;
	}

	UFUNCTION(BlueprintPure)
		bool GetAnyoneHave()
	{
		return m_WhoHave != nullptr;
	}

public:
	UPROPERTY(BlueprintReadWrite)
	bool m_IsShoot;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	int m_InitialHitDamage;
public:
	// ������Ƃ��̊֐�
	//
	// �������F�I�u�W�F�N�g�������Actor
	UFUNCTION(BlueprintCallable)
		void GetItem(AActor* _getActor);

	// �����͂��߂�֐�
	UFUNCTION(BlueprintCallable)
		void BeginShoot(FVector _shootPos, float _shootPower, FVector targetPos, AActor* _lockOnActor = nullptr);

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

	UFUNCTION(BlueprintCallable)
		void Reset(bool _resetActor = true);
};
