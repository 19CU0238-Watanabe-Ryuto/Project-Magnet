// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�iTPSCameraComponentBP����̈ڍs�j
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬
// 2021/05/12 �n粗��� ���b�N�I���Ώۂƈ��̋����ȓ��ɓ������ꍇ�A���b�N�I������������

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

	// ���C��������������
	FHitResult m_HitResult;

	// ���b�N�I�����Ă��邩
	bool m_IsLockOn;

	// ���b�N�I������Actor
	AActor* m_LockOnActor;

	// �J�����R���|�[�l���g
	UCameraComponent* m_CameraComponent;

	// �v���C���[�L�����N�^�[
	ACharacter* m_PlayerCharacter;

// private�ϐ��ǂݎ��p�֐�
public:
	// m_HitResult�擾�p
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// m_IsHit�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; } 

	// m_IsLockOn�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsLockOn() { return m_IsLockOn; }

	// m_LockOnActor�擾�p
	UFUNCTION(BlueprintPure)
		AActor* GetIsLockOnActor() { return m_LockOnActor; }

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

	// ���C�̋���
	UPROPERTY(EditAnyWhere)
		float m_RayLength;

	// ���C�̃I�t�Z�b�g�ʒu
	UPROPERTY(EditAnyWhere)
		FVector m_RayOffset;

	// ���b�N�I���𖳌�������I�u�W�F�N�g�Ƃ̋���
	UPROPERTY(EditAnyWhere)
		float m_DisableLockOnLength;

private:
	// ���b�N�I�������֐�
	void LockOn();

public:
	// �������֐�
	// 
	// ������...�ΏۂɂȂ�J�����R���|�[�l���g
	// ������...�ΏۂɂȂ�L�����N�^�[
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character);

	// ���b�N�I����Ԃɂ���֐�
	//
	// �����Ȃ�
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();
};
