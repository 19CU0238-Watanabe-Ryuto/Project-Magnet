// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�iTPSCameraComponentBP����̈ڍs�j
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬
// 2021/05/12 �n粗��� ���b�N�I���Ώۂƈ��̋����ȓ��ɓ������ꍇ�A���b�N�I������������
// 2021/05/14 �n粗��� ���b�N�I���\�^�O���������̂̂݃��b�N�I���\�ɂ���
// 2021/05/17 �n粗��� �J�����̌����Ă�������̃x�N�g�����擾�ł���悤�ɂ���
//					   �v���C���[�ƃI�u�W�F�N�g�̋��������C�̋����ȏ�ɂȂ����烍�b�N�I����������悤��
// 2021/05/24 �n粗��� ���b�N�I���\�I�u�W�F�N�g�ł��ꎞ�I�Ƀ��b�N�I���s�ɂ���@�\��ǉ�
// 2021/05/26 �n粗��� ���g�Ƀ��b�N�I���o���Ȃ��悤��
//					   ���b�N�I���̑ΏۂƂ���ʒu���ǂ̃I�u�W�F�N�g�ł����Ă��^�񒆂ɂ���
// 2021/05/27 �n粗��� ���̃R���|�[�l���g����A�N�Z�X�ł���J�����̒����Ɍ������x�N�g�����擾�ł���֐���ǉ�


#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTPSCameraComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// ���C���ق���Actor�ɂ������Ă��邩
	bool m_IsHit;

	// ���C�����b�N�I���\��Actor�ɓ������Ă��邩
	bool m_IsHitCanLockOnActor;

	// ���C��������������
	FHitResult m_HitResult;

	// ���C�̏Փ˔���p�����[�^�[
	FCollisionQueryParams m_CollisionParams;

	// �J�����̌����Ă���x�N�g��
	FVector m_CameraVector;

	// ���b�N�I�����Ă��邩
	bool m_IsLockOn;

	// ���b�N�I������Actor
	AActor* m_LockOnActor;

	// �ꎞ�I�Ƀ��b�N�I���s�ɂ���Actor
	AActor* m_CantLockOnActor;

	// �J�����R���|�[�l���g
	UCameraComponent* m_CameraComponent;

	// �v���C���[�L�����N�^�[
	ACharacter* m_PlayerCharacter;

	// ���b�N�I���\Actor�����ׂē����
	TArray<AActor*> m_CanLockOnActorArray;

	// ��ԋ߂����b�N�I���\�ȋ����ɂ��郍�b�N�I���\Actor
	AActor* m_NearCanLockOnActor;

	// private�ϐ��ǂݎ��p�֐�
public:
	// m_IsHit�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; }

	//m_IsHitCanLockOnActor�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsHitCanLockOnActor() { return m_IsHitCanLockOnActor; }

	// m_HitResult�擾�p
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// m_CameraVector�擾�p
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorRaw() { return m_CameraVector; }

	// m_CameraVector�擾�p�i���K���j
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorNormalized()
	{
		FVector normal = m_CameraVector;
		normal.Normalize();
		return normal;
	}

	// m_CameraVector�擾�p�i���K���j
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorOtherActor(FVector _originPos);

	// m_CameraVector�擾�p�i���K���j
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorNormalizedOtherActor(FVector _originPos);

	// m_IsLockOn�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsLockOn() { return m_IsLockOn; }

	// m_LockOnActor�擾�p
	UFUNCTION(BlueprintPure)
		AActor* GetLockOnActor() { return m_LockOnActor; }

	// m_CameraComponent�擾�p
	UFUNCTION(BlueprintPure)
		UCameraComponent* GetCameraComponent() { return m_CameraComponent; }

	// m_PlayerCharacter�擾�p
	UFUNCTION(BlueprintPure)
		ACharacter* GetPlayerCharacter() { return m_PlayerCharacter; }

public:

	// ���C���g���[�X��L���ɂ��邩�i���C���΂����j
	UPROPERTY(EditAnyWhere)
		bool m_EnableLineTrace;

	// �f�o�b�O�p�Ƀ��C��`�悷�邩
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool m_IsDrawDebugLine;

	// �f�o�b�O�p���C�̕\������
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float m_DrawDebugLineTime;

	// �f�o�b�O�p���C�̃��b�N�I�����̐F
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_LockOnRayColor;

	// �f�o�b�O�p���C�̔񃍃b�N�I�����̐F
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_NoLockOnRayColor;

	// �f�o�b�O�p���C�̑�Actor�Ƃ̃q�b�g���̐F
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_HitRayColor;

	// ���b�N�I���\�ɂȂ�Œ��̋���
	UPROPERTY(EditAnyWhere)
		float m_CanLockOnLength;

	// ���b�N�I���\�ɂȂ�Œ��̋���
	UPROPERTY(EditAnyWhere, meta = (ClampMin = "0", ClampMax = "90.0"))
		float m_CanLockOnDegree;

	// ���C�̃I�t�Z�b�g�ʒu
	UPROPERTY(EditAnyWhere)
		FVector m_RayOffset;

	// ���b�N�I���𖳌�������I�u�W�F�N�g�Ƃ̋���
	UPROPERTY(EditAnyWhere)
		float m_DisableLockOnLength;

	// ���b�N�I���ł���Actor�ɂ���^�O��
	UPROPERTY(EditAnyWhere)
		FName m_LockOnTag;

private:
	// ���b�N�I�������֐�
	void LockOn();

public:
	// �������֐�
	// 
	// ������...UCameraComponent* �ΏۂɂȂ�J�����R���|�[�l���g
	// ������...ACharacter*		�ΏۂɂȂ�L�����N�^�[
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character);

	// ���b�N�I����Ԃɂ���֐�
	//
	// �����Ȃ�
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();

	// ���b�N�I����Ԃ���������֐�
	//
	// ������...bool	true�Ń��b�N�I��Actor�����Z�b�g
	UFUNCTION(BlueprintCallable)
		void DisableLockOn(bool isResetLockOnActor = false);

	// �ēx���b�N�I����Ԃɂ���֐�
	// �߂�l...�Đݒ�ł������ǂ���
	//
	// �����Ȃ�
	UFUNCTION(BlueprintCallable)
		bool LockOnAgain();

	// ���b�N�I���s��Actor�̐ݒ�
	// �߂�l...�Đݒ�ł������ǂ���
	//
	// �����Ȃ�
	UFUNCTION(BlueprintCallable)
		void SetCantLockOnActor(AActor* _cantLockOnActor)
	{
		if (_cantLockOnActor != nullptr)
		{
			m_CantLockOnActor = _cantLockOnActor;
			m_CanLockOnActorArray.Remove(m_CantLockOnActor);
		}
		else
		{
			m_CanLockOnActorArray.Add(m_CantLockOnActor);
			m_CantLockOnActor = nullptr;
		}
	}

	UFUNCTION(BlueprintCallable)
		FVector GetLockOnLocation();
};