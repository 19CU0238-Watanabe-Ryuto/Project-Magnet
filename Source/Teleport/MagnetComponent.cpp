// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 MagnetComponentBPからの移行
// 2021/05/14		 渡邊龍音 オブジェクトの引き寄せを行う
// 2021/05/16 - 5/17 渡邊龍音 自身・オブジェクトの反発を行う
// 2021/05/17		 渡邊龍音 引き寄せの処理をVInterpからAddForceに変更
//							  能力使用時にロックオンを解除するように
// 2020/05/21		 渡邊龍音 能力の移動をVInterpに戻す
// 2020/05/24		 渡邊龍音 引き寄せのデフォ値を変更
//							  引き寄せ・反発のプロパティをプレイヤー用とオブジェクト用に分ける
//							  オブジェクト引き寄せ状態でプレイヤーの引き寄せを可能にする
//							  プレイヤーを引き寄せるときに足元の位置を参照
//							  移動のやりやすさを向上
// 2020/05/26		 渡邊龍音 プレイヤーの反発の挙動変更
//							  ものを当てられた時の処理を追加

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"
#include "ItemShootComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_SmallerPlayerActor(nullptr)
	, m_GreaterPlayerActor(nullptr)
	, m_PlayerAnchor(nullptr)
	, m_LockOnActorStaticMesh(nullptr)
	, m_SmallerActorStaticMesh(nullptr)
	, m_AttractFloatingPoint(nullptr)
	, m_IsAttract(false)
	, m_IsRepulsion(false)
	, m_IsPressed(false)
	, m_IsFreeze(false)
	, m_IsAttractObject(false)
	, m_IsRepulsionOfAbilityPlayer(false)
	, m_RepulsionPlayerPower(0.0f)
	, m_RepulsionTimer(0.0f)
	, m_AttractPlayerPower(5.0f)
	, m_AttractObjectPower(5.0f)
	, m_RepulsionPlayerPowerMin(50000.0f)
	, m_RepulsionPlayerPowerMax(250000.0f)
	, m_RepulsionChargeTime(1.0f)
	, m_RepulsionObjectPower(1500000.0f)
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
	if (m_TPSCamera->GetLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but LockOnActor is nullptr."));
		return;
	}

	// 引き寄せ状態切り替え
	m_IsAttract = !m_IsAttract;

	// 引き寄せ状態ならActorの位置の保存・重力の変更
	if (m_IsAttract)
	{
		// ロックオン状態でなければ終了
		if (!m_TPSCamera->GetIsLockOn())
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not lock-on."));
			m_IsAttract = false;
			return;
		}

		// ロックオン解除
		m_TPSCamera->DisableLockOn();

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクトを保存
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// プレイヤーの重力を0にする
			//m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Attract)"));
		}
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// ロックオンしたオブジェクトを保存
			m_SmallerPlayerActor = m_TPSCamera->GetLockOnActor();

			// オブジェクト引き寄せ状態にする
			m_IsAttractObject = true;

			// 引き寄せているオブジェクトを一時的にロックオン不可能にする
			m_TPSCamera->SetCantLockOnActor(m_SmallerPlayerActor);

			// 引き寄せ状態を切る
			m_IsAttract = false;

			// ロックオンActorのスタティックメッシュ取得
			m_SmallerActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetRootComponent());

			if (m_SmallerActorStaticMesh == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] The RootComponent of the Actor that can be attracted must be a StaticMeshComponent."))
			}

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Attract)"));
		}
		// 能力の対象にはならない
		else
		{
			m_IsAttract = false;
		}
	}
	else
	{
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

	/*
	// ロックオン対象Actorがなければ終了
	if (m_TPSCamera->GetLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but LockOnActor is nullptr."));
		return;
	}*/

	// 反発状態切り替え
	m_IsRepulsion = !m_IsRepulsion;

	// 反発状態ならActorの位置の保存
	if (m_IsRepulsion)
	{
		// ロックオン状態でなければ終了
		if (!m_TPSCamera->GetIsLockOn() && !m_IsAttractObject)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not lock-on."));

			m_IsRepulsion = false;
			return;
		}

		// ロックオン解除
		m_TPSCamera->DisableLockOn();

		// 能力対象がプレイヤーを移動させるものの場合
		if (m_TPSCamera->GetLockOnActor() != nullptr && m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ロックオンしたオブジェクトを保存
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// プレイヤーを反発させる状態にする
			m_IsRepulsionOfAbilityPlayer = true;

			// ボタンを押した状態にする
			m_IsPressed = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		/*
		// 能力対象がオブジェクトを移動させるものの場合
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// ロックオンしたオブジェクトを保存
			m_SmallerPlayerActor = m_TPSCamera->GetLockOnActor();

			// プレイヤーを反発させる状態にしない（オブジェクトを反発させる状態）
			m_IsRepulsionOfAbilityPlayer = false;

			// ロックオンActorのスタティックメッシュ取得
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Repulsion)"));
		}
		// 能力の対象にはならない
		else
		{
		}*/
	}
	else
	{
		// プレイヤーの重力を元に戻す
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ロックオン完全解除
		m_TPSCamera->DisableLockOn(true);
	}
}

// 反発ボタンを離した時の関数
void UMagnetComponent::ReleaseRepulsion()
{
	m_RepulsionTimer = 0.0f;
	m_IsPressed = false;
}


// 初期化関数
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent, UStaticMeshComponent* _attractFloatingPoint, USceneComponent* _playerFoot)
{
	m_TPSCamera = _TPSCameraComponent;
	m_AttractFloatingPoint = _attractFloatingPoint;
	m_PlayerAnchor = _playerFoot;

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
	// オブジェクトを引き寄せる
	if (m_IsAttractObject)
	{
		// ロックオンActorが存在していなければ
		if (m_SmallerPlayerActor == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActor is nullptr."));
			return;
		}

		// ロックオンActorのStaticMeshが無効であれば
		if (m_SmallerActorStaticMesh == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] SmallerActorStaticMesh is nullptr."));
			return;
		}

		// プレイヤーの引き寄せオブジェクト格納位置が無効であれば
		if (m_AttractFloatingPoint == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] AttractFloatingPoint is nullptr."));
			return;
		}

		FVector lerpPos = UKismetMathLibrary::VInterpTo(m_SmallerPlayerActor->GetActorLocation(), m_AttractFloatingPoint->GetComponentLocation(), _DeltaTime, m_AttractObjectPower);
		m_SmallerPlayerActor->SetActorLocation(lerpPos);
		m_SmallerActorStaticMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
	}

	// 重力をつける
	m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

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

	// プレイヤー引き寄せ
	if (m_PlayerAnchor == nullptr || m_GreaterPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] PlayerFoot or TPSCamera->LockOnActor is nullptr."));
		return;
	}

	// 重力をなくす
	m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

	// プレイヤーとオブジェクトのバウンディングボックスのサイズ取得
	FVector playerBound;
	FVector playerOrigin;
	FVector objectBound;
	FVector objectOrigin;

	m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, playerOrigin, playerBound, true);

	m_GreaterPlayerActor->GetActorBounds(true, objectOrigin, objectBound, true);

	// 目標地点の更新
	FVector targetPos = m_GreaterPlayerActor->GetActorLocation();
	targetPos.Z += (objectBound.Z * 2.0f) + playerBound.Z;

	float length = (playerOrigin - objectOrigin).Size();
	float boundLength = (playerBound - objectBound).Size() * 2.0f;

	// 触れたら固定
	if (length <= boundLength)
	{
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(targetPos);
		m_IsAttract = false;
		m_GreaterPlayerActor = nullptr;
	}
	else
	{
		FVector lerpPos = UKismetMathLibrary::VInterpTo(playerOrigin, targetPos, _DeltaTime, m_AttractPlayerPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(lerpPos, false, nullptr, ETeleportType::TeleportPhysics);
	}
}


// 反発処理関数
void UMagnetComponent::Repulsion(float _DeltaTime)
{
	// 反発状態でなければ終了
	if (!m_IsRepulsion)
	{
		return;
	}

	// オブジェクトを保持している
	if (m_IsAttractObject)
	{
		if (m_SmallerActorStaticMesh != nullptr)
		{
			// ロックオンかどうか
			if (m_TPSCamera != nullptr && m_TPSCamera->GetIsLockOn() && m_TPSCamera->GetLockOnActor() != nullptr)
			{
				FVector vector = m_TPSCamera->GetLockOnLocation() - m_SmallerActorStaticMesh->GetComponentLocation();
				vector.Normalize();

				m_SmallerActorStaticMesh->AddImpulse(vector * m_RepulsionObjectPower);
			}
			else
			{
				m_SmallerActorStaticMesh->AddImpulse(m_TPSCamera->GetCameraVectorNormalizedOtherActor(m_SmallerActorStaticMesh->GetComponentLocation()) * m_RepulsionObjectPower);
			}

			m_SmallerActorStaticMesh = nullptr;
			
			UItemShootComponent* shootComp = Cast<UItemShootComponent>(m_SmallerPlayerActor->GetComponentByClass(UItemShootComponent::StaticClass()));

			if (shootComp != nullptr)
			{
				shootComp->Shoot(m_SmallerPlayerActor->GetActorLocation());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Need to add an \"Item Shoot\" component to an Actor that can be repelled"))
			}

			m_SmallerPlayerActor = nullptr;

			m_IsAttractObject = false;

			// 引き寄せているオブジェクトの一時的ロックオン不可能の解除
			m_TPSCamera->SetCantLockOnActor(nullptr);
		}
		m_IsRepulsion = false;
	}
	else
	{
		// プレイヤーを反発させる
		if (m_IsRepulsionOfAbilityPlayer && m_IsPressed == false)
		{
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddImpulse(-m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPlayerPower);

			m_GreaterPlayerActor = nullptr;
			m_IsRepulsion = false;

			/*
			static FVector targetPos = FVector::ZeroVector;
			if (targetPos == FVector::ZeroVector)
			{
				targetPos = m_TPSCamera->GetPlayerCharacter()->GetActorLocation() + (-m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPlayerAmount);
			}

			FVector lerpPos = UKismetMathLibrary::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), targetPos, _DeltaTime, m_RepulsionPlayerSpeed);

			m_TPSCamera->GetPlayerCharacter()->SetActorLocation(lerpPos);

			FVector playerPosXY = FVector(m_TPSCamera->GetPlayerCharacter()->GetActorLocation().X, m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Y, 0.0f);
			FVector targetPosXY = FVector(targetPos.X, targetPos.Y, 0.0f);


			if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(targetPos), 1.0f)
			{
				targetPos = FVector::ZeroVector;
				m_GreaterPlayerActor = nullptr;
				m_IsRepulsion = false;
			}*/
		}
		else if (m_IsPressed == true)
		{
			m_RepulsionTimer += _DeltaTime;

			if (m_RepulsionTimer > m_RepulsionChargeTime)
			{
				m_RepulsionTimer = m_RepulsionChargeTime;
			}

			m_RepulsionPlayerPower = FMath::Lerp(m_RepulsionPlayerPowerMin, m_RepulsionPlayerPowerMax, m_RepulsionTimer / m_RepulsionChargeTime);

			UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Repulsion Power = %f"), m_RepulsionPlayerPower);
		}
		/*
		// オブジェクトを反発させる
		else
		{
			if (m_LockOnActorStaticMesh != nullptr)
			{
				m_LockOnActorStaticMesh->AddForce(m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionObjectPower);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Don't Get ProjectileMovementComponent."));
			}
		}*/
	}
}


// 当たった時に呼ぶ関数
void UMagnetComponent::Hit(AActor* _hitActor)
{
	/*
	if (m_IsAttract == true && m_GreaterPlayerActor != nullptr && m_GreaterPlayerActor == _hitActor)
	{
		// プレイヤーとオブジェクトのバウンディングボックスのサイズ取得
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_GreaterPlayerActor->GetActorBounds(true, tmp, objectBound);

		// 目標地点の更新
		FVector targetPos = m_GreaterPlayerActor->GetActorLocation();
		targetPos.Z += (objectBound.Z * 2.0f) + playerBound.Z;

		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(targetPos);
		m_IsAttract = false;
		m_GreaterPlayerActor = nullptr;
	}*/
}