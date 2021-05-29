// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� MagnetComponentBP����̈ڍs
// 2021/05/14		 �n粗��� �I�u�W�F�N�g�̈����񂹂��s��
// 2021/05/16 - 5/17 �n粗��� ���g�E�I�u�W�F�N�g�̔������s��
// 2021/05/17		 �n粗��� �����񂹂̏�����VInterp����AddForce�ɕύX
//							  �\�͎g�p���Ƀ��b�N�I������������悤��
// 2020/05/21		 �n粗��� �\�͂̈ړ���VInterp�ɖ߂�
// 2020/05/24		 �n粗��� �����񂹂̃f�t�H�l��ύX
//							  �����񂹁E�����̃v���p�e�B���v���C���[�p�ƃI�u�W�F�N�g�p�ɕ�����
//							  �I�u�W�F�N�g�����񂹏�ԂŃv���C���[�̈����񂹂��\�ɂ���
//							  �v���C���[�������񂹂�Ƃ��ɑ����̈ʒu���Q��
//							  �ړ��̂��₷��������
// 2020/05/26		 �n粗��� �v���C���[�̔����̋����ύX
//							  ���̂𓖂Ă�ꂽ���̏�����ǉ�

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

	// TPSCameraComponent���ݒ肳��Ă��Ȃ���ΏI��
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
		return;
	}

	Attract(DeltaTime);
	Repulsion(DeltaTime);
}


// �����񂹏�Ԑ؂�ւ��֐�
void UMagnetComponent::SwitchAttract()
{
	// TPSCameraComponent�������ł���ΏI��
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
		return;
	}

	// ���b�N�I���Ώ�Actor���Ȃ���ΏI��
	if (m_TPSCamera->GetLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but LockOnActor is nullptr."));
		return;
	}

	// �����񂹏�Ԑ؂�ւ�
	m_IsAttract = !m_IsAttract;

	// �����񂹏�ԂȂ�Actor�̈ʒu�̕ۑ��E�d�͂̕ύX
	if (m_IsAttract)
	{
		// ���b�N�I����ԂłȂ���ΏI��
		if (!m_TPSCamera->GetIsLockOn())
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not lock-on."));
			m_IsAttract = false;
			return;
		}

		// ���b�N�I������
		m_TPSCamera->DisableLockOn();

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// �v���C���[�̏d�͂�0�ɂ���
			//m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Attract)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_SmallerPlayerActor = m_TPSCamera->GetLockOnActor();

			// �I�u�W�F�N�g�����񂹏�Ԃɂ���
			m_IsAttractObject = true;

			// �����񂹂Ă���I�u�W�F�N�g���ꎞ�I�Ƀ��b�N�I���s�\�ɂ���
			m_TPSCamera->SetCantLockOnActor(m_SmallerPlayerActor);

			// �����񂹏�Ԃ�؂�
			m_IsAttract = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_SmallerActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetRootComponent());

			if (m_SmallerActorStaticMesh == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] The RootComponent of the Actor that can be attracted must be a StaticMeshComponent."))
			}

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Attract)"));
		}
		// �\�͂̑Ώۂɂ͂Ȃ�Ȃ�
		else
		{
			m_IsAttract = false;
		}
	}
	else
	{
		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ���b�N�I�����S����
		m_TPSCamera->DisableLockOn(true);
	}
}


// ������Ԑ؂�ւ��֐�
void UMagnetComponent::SwitchRepulsion()
{
	// TPSCameraComponent�������ł���ΏI��
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
		return;
	}

	/*
	// ���b�N�I���Ώ�Actor���Ȃ���ΏI��
	if (m_TPSCamera->GetLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but LockOnActor is nullptr."));
		return;
	}*/

	// ������Ԑ؂�ւ�
	m_IsRepulsion = !m_IsRepulsion;

	// ������ԂȂ�Actor�̈ʒu�̕ۑ�
	if (m_IsRepulsion)
	{
		// ���b�N�I����ԂłȂ���ΏI��
		if (!m_TPSCamera->GetIsLockOn() && !m_IsAttractObject)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not lock-on."));

			m_IsRepulsion = false;
			return;
		}

		// ���b�N�I������
		m_TPSCamera->DisableLockOn();

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetLockOnActor() != nullptr && m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// �v���C���[�𔽔��������Ԃɂ���
			m_IsRepulsionOfAbilityPlayer = true;

			// �{�^������������Ԃɂ���
			m_IsPressed = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		/*
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_SmallerPlayerActor = m_TPSCamera->GetLockOnActor();

			// �v���C���[�𔽔��������Ԃɂ��Ȃ��i�I�u�W�F�N�g�𔽔��������ԁj
			m_IsRepulsionOfAbilityPlayer = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Repulsion)"));
		}
		// �\�͂̑Ώۂɂ͂Ȃ�Ȃ�
		else
		{
		}*/
	}
	else
	{
		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ���b�N�I�����S����
		m_TPSCamera->DisableLockOn(true);
	}
}

// �����{�^���𗣂������̊֐�
void UMagnetComponent::ReleaseRepulsion()
{
	m_RepulsionTimer = 0.0f;
	m_IsPressed = false;
}


// �������֐�
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


// �����񂹏����֐�
void UMagnetComponent::Attract(float _DeltaTime)
{
	// �I�u�W�F�N�g�������񂹂�
	if (m_IsAttractObject)
	{
		// ���b�N�I��Actor�����݂��Ă��Ȃ����
		if (m_SmallerPlayerActor == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActor is nullptr."));
			return;
		}

		// ���b�N�I��Actor��StaticMesh�������ł����
		if (m_SmallerActorStaticMesh == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] SmallerActorStaticMesh is nullptr."));
			return;
		}

		// �v���C���[�̈����񂹃I�u�W�F�N�g�i�[�ʒu�������ł����
		if (m_AttractFloatingPoint == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] AttractFloatingPoint is nullptr."));
			return;
		}

		FVector lerpPos = UKismetMathLibrary::VInterpTo(m_SmallerPlayerActor->GetActorLocation(), m_AttractFloatingPoint->GetComponentLocation(), _DeltaTime, m_AttractObjectPower);
		m_SmallerPlayerActor->SetActorLocation(lerpPos);
		m_SmallerActorStaticMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
	}

	// �d�͂�����
	m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

	// �����񂹏�ԂłȂ���ΏI��
	if (!m_IsAttract)
	{
		// ��~��Ԃ̉���
		m_IsFreeze = false;
		return;
	}

	// �Œ肷���Ԃł����
	if (m_IsFreeze)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Player is freeze."));
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	// �v���C���[������
	if (m_PlayerAnchor == nullptr || m_GreaterPlayerActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] PlayerFoot or TPSCamera->LockOnActor is nullptr."));
		return;
	}

	// �d�͂��Ȃ���
	m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

	// �v���C���[�ƃI�u�W�F�N�g�̃o�E���f�B���O�{�b�N�X�̃T�C�Y�擾
	FVector playerBound;
	FVector playerOrigin;
	FVector objectBound;
	FVector objectOrigin;

	m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, playerOrigin, playerBound, true);

	m_GreaterPlayerActor->GetActorBounds(true, objectOrigin, objectBound, true);

	// �ڕW�n�_�̍X�V
	FVector targetPos = m_GreaterPlayerActor->GetActorLocation();
	targetPos.Z += (objectBound.Z * 2.0f) + playerBound.Z;

	float length = (playerOrigin - objectOrigin).Size();
	float boundLength = (playerBound - objectBound).Size() * 2.0f;

	// �G�ꂽ��Œ�
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


// ���������֐�
void UMagnetComponent::Repulsion(float _DeltaTime)
{
	// ������ԂłȂ���ΏI��
	if (!m_IsRepulsion)
	{
		return;
	}

	// �I�u�W�F�N�g��ێ����Ă���
	if (m_IsAttractObject)
	{
		if (m_SmallerActorStaticMesh != nullptr)
		{
			// ���b�N�I�����ǂ���
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

			// �����񂹂Ă���I�u�W�F�N�g�̈ꎞ�I���b�N�I���s�\�̉���
			m_TPSCamera->SetCantLockOnActor(nullptr);
		}
		m_IsRepulsion = false;
	}
	else
	{
		// �v���C���[�𔽔�������
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
		// �I�u�W�F�N�g�𔽔�������
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


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	/*
	if (m_IsAttract == true && m_GreaterPlayerActor != nullptr && m_GreaterPlayerActor == _hitActor)
	{
		// �v���C���[�ƃI�u�W�F�N�g�̃o�E���f�B���O�{�b�N�X�̃T�C�Y�擾
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_GreaterPlayerActor->GetActorBounds(true, tmp, objectBound);

		// �ڕW�n�_�̍X�V
		FVector targetPos = m_GreaterPlayerActor->GetActorLocation();
		targetPos.Z += (objectBound.Z * 2.0f) + playerBound.Z;

		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(targetPos);
		m_IsAttract = false;
		m_GreaterPlayerActor = nullptr;
	}*/
}