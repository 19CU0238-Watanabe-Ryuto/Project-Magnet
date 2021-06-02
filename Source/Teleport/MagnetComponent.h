// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� MagnetComponentBP����̈ڍs
// 2021/05/14		 �n粗��� �I�u�W�F�N�g�̈����񂹂��s��
// 2021/05/16 - 5/17 �n粗��� ���g�E�I�u�W�F�N�g�̔������s��
// 2021/05/17		 �n粗��� �����񂹂̏�����VInterp����AddForce�ɕύX
//							  �\�͎g�p���Ƀ��b�N�I������������悤��
// 2020/05/21		 �n粗��� �\�͂̈ړ���VInterp�ɖ߂�
// 2020/05/24		 �n粗��� �����񂹂̃f�t�H�l��ύX
//							  �����񂹁E�����̃v���p�e�B���v���C���[�p�ƃI�u�W�F�N�g�p�ɕ�����
//							  �I�u�W�F�N�g�����񂹏�ԂŃv���C���[�̈����񂹂��\�ɂ���
//							  �v���C���[�������񂹂�Ƃ��ɑ����̈ʒu���Q��
//							  �ړ��̂��₷��������
// 2020/05/26		 �n粗��� �v���C���[�̔����̋����ύX
//							  ���̂𓖂Ă�ꂽ���̏�����ǉ�
// 2020/05/28		 �n粗��� �����ړ����`���[�W���ɕύX
// 2020/05/30		 �n粗��� �{�[��Actor��\�͂Ŏ������Ƃ��Ƀ{�[��������Ԃɂ���
//							  �I�u�W�F�N�g�����񂹏�Ԃ��O����������ł���֐���ǉ�

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagnetComponent.generated.h"

class UTPSCameraComponent;
class UItemShootComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UMagnetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMagnetComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// TPSCameraComponent
	UTPSCameraComponent* m_TPSCamera;

	// �\�͂ňړ�����Actor
	AActor* m_SmallerPlayerActor;

	// �\�͂ňړ����Ȃ�Actor
	AActor* m_GreaterPlayerActor;

	// ���b�N�I������Actor��StaticMesh
	UStaticMeshComponent* m_LockOnActorStaticMesh;

	// �\�͂ňړ�����Actor��StaticMesh
	UStaticMeshComponent* m_SmallerActorStaticMesh;

	// ���͔\�͂ň����񂹂��s�����Ƃ��ɌŒ肷��I�u�W�F�N�g�̈ʒu
	UStaticMeshComponent* m_AttractFloatingPoint;

	// �����񂹂Ă���I�u�W�F�N�g��ItemShootComponent
	UItemShootComponent* m_AttractItemShootComp;

	// �����񂹏�Ԃł��邩
	bool m_IsAttract;

	// ������Ԃł��邩
	bool m_IsRepulsion;

	// �����{�^���������Ă��邩
	bool m_IsPressed;

	// ��~��Ԃł��邩
	bool m_IsFreeze;

	// �I�u�W�F�N�g��Ǐ]�����Ă����Ԃ�
	bool m_IsAttractObject;

	// �����\�͂̑Ώۂ��v���C���[���i�\�͂Ńv���C���[���������I�u�W�F�N�g���������j
	bool m_IsRepulsionOfAbilityPlayer;

	// �����Ă���Actor���{�[��Actor��
	bool m_IsAttractActorIsBall;

	// �v���C���[�̂��Ƃ��Ƃ�gravityScale
	float m_playerOriginGravityScale;

	// �����̗�
	float m_RepulsionPlayerPower;

	// �����̃`���[�W���ԑ���p
	float m_RepulsionTimer;

	// private�ϐ��擾�p
public:
	// m_IsFreeze�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsFreeze() { return m_IsFreeze; }

	// m_IsAttract�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsAttract() { return m_IsAttract; }

	// m_IsAttract�擾�p
	UFUNCTION(BlueprintPure)
		AActor* GetAttractActor() { return m_SmallerPlayerActor; }

public:
	// �v���C���[�������񂹂��
	UPROPERTY(EditAnyWhere)
		float m_AttractPlayerPower;

	// �I�u�W�F�N�g�������񂹂��
	UPROPERTY(EditAnyWhere)
		float m_AttractObjectPower;

	// �I�u�W�F�N�g�ɑ΂��ăv���C���[����������ŏ��̗�
	UPROPERTY(EditAnyWhere)
		float m_RepulsionPlayerPowerMin;

	// �I�u�W�F�N�g�ɑ΂��ăv���C���[����������ő�̗�
	UPROPERTY(EditAnyWhere)
		float m_RepulsionPlayerPowerMax;

	// �I�u�W�F�N�g�ɑ΂��ăv���C���[����������ő�̗�
	UPROPERTY(EditAnyWhere)
		float m_RepulsionChargeTime;

	// �v���C���[���I�u�W�F�N�g�𔽔��������
	UPROPERTY(EditAnyWhere)
		float m_RepulsionObjectPower;

	//  �\�͂̑Ώۂ��v���C���[�̎��̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityPlayerTagName;

	// �\�͂̑Ώۂ��I�u�W�F�N�g�̎��̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityObjectTagName;

	// �{�[���̃^�O
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		FName m_BallTag;


private:
	// �����񂹏����֐�
	//
	// 
	void Attract(float _DeltaTime);

	// ���������֐�
	//
	// 
	void Repulsion(float _DeltaTime);

public:
	// �������֐�
	//
	// ������...�L�����N�^�[�ɂ��Ă���TPSCameraComponent
	UFUNCTION(BlueprintCallable)
		void Init(UTPSCameraComponent* _TPSCameraComponent, UStaticMeshComponent* _attractFloatingPoint);

	// �����񂹏�Ԑ؂�ւ��֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchAttract();

	// ������Ԑ؂�ւ��֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchRepulsion();

	// �����{�^���𗣂������̊֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void ReleaseRepulsion();

	// �����������ɌĂԊ֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void Hit(AActor* _hitActor);

	// �����񂹂Ă���Actor���{�[��Actor���ǂ���

	UFUNCTION(BlueprintPure)
		bool IsAttractObjectIsBallActor()
	{
		if (m_SmallerPlayerActor != nullptr)
		{
			return m_SmallerPlayerActor->ActorHasTag(m_BallTag);
		}
		return false;
	}

	// �I�u�W�F�N�g�����񂹏�Ԃ���������

	UFUNCTION(BlueprintCallable)
		void DisableAttractObject();

	// �I�u�W�F�N�g�����񂹏�Ԃ�ݒ肷��

	UFUNCTION(BlueprintCallable)
		void SetAttractObject(AActor* _attractActor);
};
