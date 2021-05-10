// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// �J�����R���|�[�l���g
	UPROPERTY(EditAnyWhere)
		UCameraComponent* cameraComponent;

	// ���C���g���[�X��L���ɂ��邩�i���C���΂����j
	UPROPERTY(EditAnyWhere)
		bool enableLineTrace;

	// �f�o�b�O�p�Ƀ��C��`�悷�邩
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool isDrawDebugLine;

	// �f�o�b�O�p���C�̕\������
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float drawDebugLineTime;

	// ���C�̋���
	UPROPERTY(EditAnyWhere)
		float rayLength;

	// ���C�̃I�t�Z�b�g�ʒu
	UPROPERTY(EditAnyWhere)
		FVector rayOffset;

	// ���C���ق���Actor�ɂ������Ă��邩
	UPROPERTY(VisibleAnyWhere)
		bool isHit;

	// ���C��������������
	UPROPERTY(VisibleAnyWhere)
		FHitResult hitResult;

public:
	// �������֐�
	// 
	// ������ �J�����R���|�[�l���g
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera);
};
