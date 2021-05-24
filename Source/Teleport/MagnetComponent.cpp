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

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"
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
	, m_IsFreeze(false)
	, m_IsAttractObject(false)
	, m_IsRepulsionOfAbilityPlayer(false)
	//, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPlayerPower(5.0f)
	, m_AttractObjectPower(5.0f)
	, m_RepulsionPlayerPower(40000000.0f)
	, m_RepulsionObjectPower(40000000.0f)
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
			// ���b�N�I�������I�u�W�F�N�g�ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// �v���C���[�̏d�͂�0�ɂ���
			//m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Attract)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_SmallerPlayerActor = m_TPSCamera->GetLockOnActor();

			// �I�u�W�F�N�g�����񂹏�Ԃɂ���
			m_IsAttractObject = true;

			// �����񂹏�Ԃ�؂�
			m_IsAttract = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_SmallerActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

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
		/*
		// ���b�N�I����ԂłȂ���ΏI��
		if (!m_TPSCamera->GetIsLockOn())
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not lock-on."));
			m_IsRepulsion = false;
			return;
		}*/

		// ���b�N�I������
		m_TPSCamera->DisableLockOn();

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetLockOnActor() != nullptr && m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu�𔽔��̊�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// ���b�N�I�������I�u�W�F�N�g��ۑ�
			m_GreaterPlayerActor = m_TPSCamera->GetLockOnActor();

			// �v���C���[�𔽔��������Ԃɂ���
			m_IsRepulsionOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		/*
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();
			// 
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
	FVector objectBound;
	FVector tmp;

	m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

	m_GreaterPlayerActor->GetActorBounds(true, tmp, objectBound);

	// �ڕW�n�_�̍X�V
	FVector targetPos = m_GreaterPlayerActor->GetActorLocation();
	targetPos.Z += objectBound.Z + playerBound.Z;

	// �G�ꂽ��Œ�
	if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(targetPos, 50.0f))
	{
		m_IsAttract = false;
	}
	else
	{
		FVector lerpPos = UKismetMathLibrary::VInterpTo(m_PlayerAnchor->GetComponentLocation(), targetPos, _DeltaTime, m_AttractPlayerPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(lerpPos);
	}


	/*
	 * �I�u�W�F�N�g�ƃv���C���[�̋����v���p
	float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TargetActorLocation).Size();
	UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Actor Length = %f"), length);
	*/
}


// ���������֐�
void UMagnetComponent::Repulsion(float _DeltaTime)
{
	// ��x�̂�AddForce���邽��
	static bool isOnce = true;

	// ������ԂłȂ���ΏI��
	if (!m_IsRepulsion)
	{
		isOnce = true;
		return;
	}

	if (isOnce)
	{
		isOnce = false;
		// �I�u�W�F�N�g��ێ����Ă���
		if (m_IsAttractObject)
		{
			if (m_SmallerActorStaticMesh != nullptr)
			{
				m_SmallerActorStaticMesh->AddForce(m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionObjectPower);
				m_IsAttractObject = false;
				m_IsRepulsion = false;
				isOnce = true;
			}
		}
		else
		{
			// �v���C���[�𔽔�������
			if (m_IsRepulsionOfAbilityPlayer)
			{
				m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddForce(-m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPlayerPower);
			}
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
					m_IsRepulsion = false;
					isOnce = true;
				}
			}
		}
	}
}


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	// �����񂹒��ɃI�u�W�F�N�g�ɓ������Ďx�������p
	static float moveValue = 10.0f;

	if (m_IsAttract == true && m_GreaterPlayerActor != nullptr && m_GreaterPlayerActor == _hitActor)
	{
		FVector loc = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();
		loc.Z += moveValue;

		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(loc);

		moveValue += moveValue;
	}
	else
	{
		moveValue = 10.0f;
	}
}