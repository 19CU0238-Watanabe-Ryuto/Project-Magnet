// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 �n粗��� ���g�̈����񂹁E�������s���iMagnetComponentBP����̈ڍs�j

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagnetComponent.generated.h"

class UTPSCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	// �v���C���[�̂��Ƃ��Ƃ�gravityScale
	float m_playerOriginGravityScale;

public:
	// �������֐�
	//
	// ������...�L�����N�^�[�ɂ��Ă���TPSCameraComponent
	void Init(UTPSCameraComponent* _TPSCameraComponent);
};
