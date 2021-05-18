// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 MagnetComponentBPからの移行
// 2021/05/14		 渡邊龍音 オブジェクトの引き寄せを行う
// 2021/05/16 - 5/17 渡邊龍音 自身・オブジェクトの反発を行う
// 2021/05/17		 渡邊龍音 引き寄せの処理をVInterpからAddForceに変更
//							  能力使用時にロックオンを解除するように

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_LockOnActorStaticMesh(nullptr)
	, m_IsAttract(false)
	, m_IsRepulsion(false)
	, m_IsFreeze(false)
	, m_IsTargetOfAbilityPlayer(false)
	//, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPower(5000000.0f)
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
	// TPSCameraComponentが無効であれば終了
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
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

	// ロックオン解除
	m_TPSCamera->DisableLockOn();

	// 引き寄せ状態ならActorの位置の保存・重力の変更
	if (m_IsAttract)
	{
		// 反発状態を切る
		m_IsRepulsion = false;

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクト位置を目標地点として保存
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

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
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// プレイヤー引き寄せ状態にしない（オブジェクト引き寄せ状態）
			m_IsTargetOfAbilityPlayer = false;

			// ロックオンActorのスタティックメッシュ取得
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetIsLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

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
		//m_TargetActorLocation = FVector::ZeroVector;

		// プレイヤーの重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ロックオン完全解除
		m_TPSCamera->DisableLockOn(true);
	}
}


// 反発状態切り替え関数
void UMagnetComponent::SwitchRepulsion()
{
	// TPSCameraComponentが無効であれば終了
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
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

		// ロックオン解除
		m_TPSCamera->DisableLockOn();

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクト位置を反発の基準点として保存
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// プレイヤーを反発させる状態にする
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// 能力使用地点のプレイヤーの位置を目標地点として保存
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// プレイヤーを反発させる状態にしない（オブジェクトを反発させる状態）
			m_IsTargetOfAbilityPlayer = false;

			// ロックオンActorのスタティックメッシュ取得
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetIsLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

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
		//m_TargetActorLocation = FVector::ZeroVector;

		// プレイヤーの重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ロックオン完全解除
		m_TPSCamera->DisableLockOn(true);
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
		// オブジェクトとプレイヤーの距離計測
		float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TPSCamera->GetIsLockOnActor()->GetActorLocation()).Size();

		// プレイヤーとオブジェクトのバウンディングボックスのサイズ取得
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_TPSCamera->GetIsLockOnActor()->GetActorBounds(true, tmp, objectBound);

		float boundBoxLength = (playerBound + objectBound).Size();

		// 触れたら固定
		if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(m_TPSCamera->GetIsLockOnActor()->GetActorLocation(), boundBoxLength))
		{
			m_IsFreeze = true;
		}
		else
		{
			// プレイヤーとオブジェクトの距離に応じて力を加え続ける（近いほど力を強くする）
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddForce(m_TPSCamera->GetCameraVectorNormalized() * m_AttractPower * (m_TPSCamera->m_RayLength / length));
		}
	}
	// オブジェクトを引き寄せる
	else
	{
		// ロックオンActorが存在していれば
		if (m_TPSCamera->GetIsLockOnActor() == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActor is nullptr."));
			return;
		}
		// オブジェクトとプレイヤーの距離計測
		float length = (m_TPSCamera->GetIsLockOnActor()->GetActorLocation() - m_TPSCamera->GetPlayerCharacter()->GetActorLocation()).Size();

		// プレイヤーとオブジェクトのバウンディングボックスのサイズ取得
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_TPSCamera->GetIsLockOnActor()->GetActorBounds(true, tmp, objectBound);

		float boundBoxLength = (playerBound + objectBound).Size();

		// 触れたら固定
		if (m_TPSCamera->GetIsLockOnActor()->GetActorLocation().Equals(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), boundBoxLength + 10))
		{
			m_LockOnActorStaticMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
			m_IsAttract = false;
		}
		else
		{
			// プレイヤーとオブジェクトの距離に応じて力を加え続ける（近いほど力を強くする）
			FVector toPlayerVector = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TPSCamera->GetIsLockOnActor()->GetActorLocation());
			toPlayerVector.Normalize();
			m_LockOnActorStaticMesh->AddForce(toPlayerVector * m_AttractPower * (m_TPSCamera->m_RayLength / (length * 5)));

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
			if (m_LockOnActorStaticMesh != nullptr)
			{
				m_LockOnActorStaticMesh->AddForce(m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPower);
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