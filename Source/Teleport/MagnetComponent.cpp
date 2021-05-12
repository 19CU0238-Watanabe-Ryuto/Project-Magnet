// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 �n粗��� ���g�̈����񂹁E�������s���iMagnetComponentBP����̈ڍs�j

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_playerOriginGravityScale(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// �������֐�
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent)
{
	m_TPSCamera = _TPSCameraComponent;
	m_playerOriginGravityScale = m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale;
}

