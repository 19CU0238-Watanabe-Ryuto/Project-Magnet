// 磁力の能力を管理するクラス
//
// 2021/05/12 渡邊龍音 自身の引き寄せ・反発を行う（MagnetComponentBPからの移行）

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


// 初期化関数
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent)
{
	m_TPSCamera = _TPSCameraComponent;
	m_playerOriginGravityScale = m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale;
}

