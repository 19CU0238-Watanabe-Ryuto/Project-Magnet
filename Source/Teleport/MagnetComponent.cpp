// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 MagnetComponentBPからの移行
// 2021/05/14		 渡邊龍音 オブジェクトの引き寄せを行う
// 2021/05/16 - 5/17 渡邊龍音 自身・オブジェクトの反発を行う

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_IsAttract(false)
	, m_IsRepulsion(false)
	, m_IsFreeze(false)
	, m_IsTargetOfAbilityPlayer(false)
	, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPower(10.0f)
	, m_RepulsionPower(20000000.0f)
	, m_TargetOfAbilityPlayerTagName("A")
	, m_TargetOfAbilityObjectTagName("B")
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
	Repulsion(DeltaTime);
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
		// 反発状態を切る
		m_IsRepulsion = false;

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクト位置を目標地点として保存
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// プレイヤーの重力を0にする
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			// プレイヤー引き寄せ状態にする
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Attract)"));
		}
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// 能力使用地点のプレイヤーの位置を目標地点として保存
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// プレイヤー引き寄せ状態にしない（オブジェクト引き寄せ状態）
			m_IsTargetOfAbilityPlayer = false;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Attract)"));
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


// 反発状態切り替え関数
void UMagnetComponent::SwitchRepulsion()
{
	// ロックオン状態でなければ終了
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not locked on."));
		return;
	}

	// ロックオン対象Actorがなければ終了
	if (m_TPSCamera->GetIsLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but LockOnActor is nullptr."));
		return;
	}

	// 反発状態切り替え
	m_IsRepulsion = !m_IsRepulsion;

	// 反発状態ならActorの位置の保存
	if (m_IsRepulsion)
	{
		// 引き寄せ状態を切る
		m_IsAttract = false;

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクト位置を反発の基準点として保存
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// プレイヤーを反発させる状態にする
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// 能力使用地点のプレイヤーの位置を目標地点として保存
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// プレイヤーを反発させる状態にしない（オブジェクトを反発させる状態）
			m_IsTargetOfAbilityPlayer = false;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Repulsion)"));
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

	if (m_TPSCamera != nullptr)
	{
		m_playerOriginGravityScale = m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale;
	}
	else
	{
	}
}


// 引き寄せ処理関数
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
	if (m_IsTargetOfAbilityPlayer)
	{
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(interpPos);

		if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(m_TargetActorLocation))
		{
			m_IsFreeze = true;
		}
	}
	// オブジェクトを引き寄せる
	else
	{
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetIsLockOnActor()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetIsLockOnActor()->SetActorLocation(interpPos);

		if (m_TPSCamera->GetIsLockOnActor()->GetActorLocation().Equals(m_TargetActorLocation, 50.0f))
		{
			m_IsAttract = false;
		}
	}

	/*
	 * オブジェクトとプレイヤーの距離計測用
	float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TargetActorLocation).Size();
	UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Actor Length = %f"), length);
	*/
}


// 反発処理関数
void UMagnetComponent::Repulsion(float _DeltaTime)
{
	// 一度のみAddForceするため
	static bool isOnce = true;

	// 反発状態でなければ終了
	if (!m_IsRepulsion)
	{
		isOnce = true;
		return;
	}

	// ロックオン状態でなければ反発状態を解除して終了
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not locked on."));

		// 引き寄せを解除
		m_IsRepulsion = false;

		// 位置のリセット
		m_TargetActorLocation = FVector::ZeroVector;

		// 重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
		return;
	}

	if (isOnce)
	{
		isOnce = false;
		// プレイヤーを反発させる
		if (m_IsTargetOfAbilityPlayer)
		{
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddForce(-m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPower);
		}
		// オブジェクトを反発させる
		else
		{
			UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetIsLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			if (staticMesh != nullptr)
			{
				staticMesh->AddForce(FVector(0.0f, 0.0f, 1.0f) * m_RepulsionPower);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Don't Get ProjectileMovementComponent."));
			}
		}
	}
}


// 当たった時に呼ぶ関数
void UMagnetComponent::Hit(AActor* _hitActor)
{
	//m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}