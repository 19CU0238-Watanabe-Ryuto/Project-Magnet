// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� ���g�̈����񂹁E�������s���iMagnetComponentBP����̈ڍs�j

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

	// �����񂹏�Ԃł��邩
	bool m_IsAttract;

	// ��~��Ԃł��邩
	bool m_IsFreeze;

	// �v���C���[�������񂹂��Ԃ�
	bool m_IsAttractPlayer;

	// �v���C���[�̂��Ƃ��Ƃ�gravityScale
	float m_playerOriginGravityScale;

	// �����񂹈ړ����鎞�̑Ώ�Actor�̈ʒu
	FVector m_TargetActorLocation;

	// private�ϐ��擾�p
public:
	// m_IsFreeze�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsFreeze() { return m_IsFreeze; }

public:
	// �����񂹂��
	UPROPERTY(EditAnyWhere)
		float m_AttractPower;

	// �v���C���[�������񂹂鎞�̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_AttractPlayerTagName;

	// �I�u�W�F�N�g�������񂹂鎞�̃I�u�W�F�N�g�̃^�O��
	UPROPERTY(EditAnyWhere)
		FName m_AttractObjectTagName;

private:
	// �����񂹊֐�
	//
	// 
	void Attract(float _DeltaTime);

public:
	// �������֐�
	//
	// ������...�L�����N�^�[�ɂ��Ă���TPSCameraComponent
	UFUNCTION(BlueprintCallable)
		void Init(UTPSCameraComponent* _TPSCameraComponent);

	// �����񂹏�Ԑ؂�ւ��֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchAttract();

	// �����������ɌĂԊ֐�
	//
	//
	UFUNCTION(BlueprintCallable)
		void Hit(AActor* _hitActor);
};
