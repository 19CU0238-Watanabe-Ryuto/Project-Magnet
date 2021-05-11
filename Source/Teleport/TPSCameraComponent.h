// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTPSCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// ���C���ق���Actor�ɂ������Ă��邩
	bool m_IsHit;

	// ���C��������������
	FHitResult m_HitResult;

	// ���b�N�I�����Ă��邩
	bool m_IsLockOn;

	// ���b�N�I������Actor
	AActor* m_LockOnActor;

// private�ϐ��ǂݎ��p�֐�
public:
	// hitResult�擾�p
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// isHit�擾�p
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; }

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// �J�����R���|�[�l���g
	UPROPERTY(EditAnyWhere)
		UCameraComponent* m_CameraComponent;

	// �v���C���[�R���|�[�l���g
	UPROPERTY(EditAnyWhere)
		ACharacter* m_PlayerCharacter;

	// ���C���g���[�X��L���ɂ��邩�i���C���΂����j
	UPROPERTY(EditAnyWhere)
		bool m_EnableLineTrace;

	// �f�o�b�O�p�Ƀ��C��`�悷�邩
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool m_IsDrawDebugLine;

	// �f�o�b�O�p���C�̕\������
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float m_DrawDebugLineTime;

	// ���C�̋���
	UPROPERTY(EditAnyWhere)
		float m_RayLength;

	// ���C�̃I�t�Z�b�g�ʒu
	UPROPERTY(EditAnyWhere)
		FVector m_RayOffset;

private:
	// ���b�N�I�������֐�
	void LockOn();

public:
	// �������֐�
	// 
	// ������ �ΏۂɂȂ�J�����R���|�[�l���g
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character);

	// ���b�N�I����Ԃɂ���֐�
	//
	// �����Ȃ�
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();
};
