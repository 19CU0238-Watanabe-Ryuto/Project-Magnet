// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 自身の引き寄せ・反発を行う（MagnetComponentBPからの移行）

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_IsAttract(false)
	, m_IsFreeze(false)
	, m_IsAttractPlayer(false)
	, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPower(10.0f)
	, m_AttractPlayerTagName("")
	, m_AttractObjectTagName("")
	, m_playerOriginGravityScale(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TPSCameraComponentが設定されていなければ終了
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
		return;
	}

	Attract(DeltaTime);
}


// 引き寄せ状態切り替え関数
void UMagnetComponent::SwitchAttract()
{
	// ロックオン状態でなければ終了
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));
		return;
	}

	// ロックオン対象Actorがなければ終了
	if (m_TPSCamera->GetIsLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but LockOnActor is nullptr."));
		return;
	}

	// 引き寄せ状態切り替え
	m_IsAttract = !m_IsAttract;

	// 引き寄せ状態ならActorの位置の保存・重力の変更
	if (m_IsAttract)
	{
		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_AttractPlayerTagName))
		{
			// ロックオンしたオブジェクト位置を目標地点として保存
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// プレイヤーの重力を0にする
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			// プレイヤー引き寄せ状態にする
			m_IsAttractPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player"));
		}
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_AttractObjectTagName))
		{
			// 能力使用地点のプレイヤーの位置を目標地点として保存
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// プレイヤー引き寄せ状態にしない（オブジェクト引き寄せ状態）
			m_IsAttractPlayer = false;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object"));
		}
		// 能力の対象にはならない
		else
		{

		}
	}
	else
	{
		// 目標地点のリセット
		m_TargetActorLocation = FVector::ZeroVector;

		// プレイヤーの重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
	}
}


// 初期化関数
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent)
{
	m_TPSCamera = _TPSCameraComponent;
	m_playerOriginGravityScale = m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale;
}


// 引き寄せ関数
void UMagnetComponent::Attract(float _DeltaTime)
{
	// 引き寄せ状態でなければ終了
	if (!m_IsAttract)
	{
		// 停止状態の解除
		m_IsFreeze = false;
		return;
	}

	// ロックオン状態でなければ引き寄せ状態を解除して終了
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));

		// 引き寄せを解除
		m_IsAttract = false;

		// 停止状態の解除
		m_IsFreeze = false;

		// 位置のリセット
		m_TargetActorLocation = FVector::ZeroVector;

		// 重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
		return;
	}

	// 固定する状態であれば
	if (m_IsFreeze)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Player is freeze."));
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	// プレイヤーを引き寄せる
	if (m_IsAttractPlayer)
	{
		// 引き寄せを行う
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(interpPos);
	}
	// オブジェクトを引き寄せる
	else
	{
		// 引き寄せを行う
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetIsLockOnActor()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetIsLockOnActor()->SetActorLocation(interpPos);
	}

	float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TargetActorLocation).Size();
	UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Actor Length = %f"), length);
}


// 当たった時に呼ぶ関数
void UMagnetComponent::Hit(AActor* _hitActor)
{
	m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}