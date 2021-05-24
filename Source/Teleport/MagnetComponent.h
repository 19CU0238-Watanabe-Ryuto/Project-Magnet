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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagnetComponent.generated.h"

class UTPSCameraComponent;

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

	// �v���C���[�̑����̈ʒu��SceneComponent
	USceneComponent* m_PlayerAnchor;

	// ���b�N�I������Actor��StaticMesh
	UStaticMeshComponent* m_LockOnActorStaticMesh;

	// �\�͂ňړ�����Actor��StaticMesh
	UStaticMeshComponent* m_SmallerActorStaticMesh;

	// ���͔\�͂ň����񂹂��s�����Ƃ��ɌŒ肷��I�u�W�F�N�g�̈ʒu
	UStaticMeshComponent* m_AttractFloatingPoint;

	// �����񂹏�Ԃł��邩
	bool m_IsAttract;

	// ������Ԃł��邩
	bool m_IsRepulsion;

	// ��~��Ԃł��邩
	bool m_IsFreeze;

	// �I�u�W�F�N�g��Ǐ]�����Ă����Ԃ�
	bool m_IsAttractObject;

	// �����\�͂̑Ώۂ��v���C���[���i�\�͂Ńv���C���[���������I�u�W�F�N�g���������j
	bool m_IsRepulsionOfAbilityPlayer;

	// �v���C���[�̂��Ƃ��Ƃ�gravityScale
	float m_playerOriginGravityScale;

	// �����񂹈ړ����鎞�̑Ώ�Actor�̈ʒu
	//FVector m_TargetActorLocation;

	// private�ϐ��擾�p
public:
	// m_IsFreeze�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsFreeze() { return m_IsFreeze; }

	// m_IsAttract�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsAttract() { return m_IsAttract; }

public:
	// �v���C���[�������񂹂��
	UPROPERTY(EditAnyWhere)
		float m_AttractPlayerPower;

	// �I�u�W�F�N�g�������񂹂��
	UPROPERTY(EditAnyWhere)
		float m_AttractObjectPower;

	// �I�u�W�F�N�g�ɑ΂��ăv���C���[�����������
	UPROPERTY(EditAnyWhere)
		float m_RepulsionPlayerPower;

	// �v���C���[���I�u�W�F�N�g�𔽔��������
	UPROPERTY(EditAnyWhere)
		float m_RepulsionObjectPower;

	//  �\�͂̑Ώۂ��v���C���[�̎��̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityPlayerTagName;

	// �\�͂̑Ώۂ��I�u�W�F�N�g�̎��̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityObjectTagName;

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
		void Init(UTPSCameraComponent* _TPSCameraComponent, UStaticMeshComponent* _attractFloatingPoint, USceneComponent* _playerFoot);

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

	// �����������ɌĂԊ֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void Hit(AActor* _hitActor);
};
