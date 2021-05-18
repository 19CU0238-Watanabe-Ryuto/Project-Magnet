// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� MagnetComponentBP����̈ڍs
// 2021/05/14		 �n粗��� �I�u�W�F�N�g�̈����񂹂��s��
// 2021/05/16 - 5/17 �n粗��� ���g�E�I�u�W�F�N�g�̔������s��
// 2021/05/17		 �n粗��� �����񂹂̏�����VInterp����AddForce�ɕύX
//							  �\�͎g�p���Ƀ��b�N�I������������悤��

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
	if (m_TPSCamera->GetIsLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but LockOnActor is nullptr."));
		return;
	}

	// �����񂹏�Ԑ؂�ւ�
	m_IsAttract = !m_IsAttract;

	// ���b�N�I������
	m_TPSCamera->DisableLockOn();

	// �����񂹏�ԂȂ�Actor�̈ʒu�̕ۑ��E�d�͂̕ύX
	if (m_IsAttract)
	{
		// ������Ԃ�؂�
		m_IsRepulsion = false;

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// �v���C���[�̏d�͂�0�ɂ���
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			// �v���C���[�����񂹏�Ԃɂ���
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Attract)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�����񂹏�Ԃɂ��Ȃ��i�I�u�W�F�N�g�����񂹏�ԁj
			m_IsTargetOfAbilityPlayer = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetIsLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Attract)"));
		}
		// �\�͂̑Ώۂɂ͂Ȃ�Ȃ�
		else
		{

		}
	}
	else
	{
		// �ڕW�n�_�̃��Z�b�g
		//m_TargetActorLocation = FVector::ZeroVector;

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

	// ���b�N�I���Ώ�Actor���Ȃ���ΏI��
	if (m_TPSCamera->GetIsLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but LockOnActor is nullptr."));
		return;
	}

	// ������Ԑ؂�ւ�
	m_IsRepulsion = !m_IsRepulsion;

	// ������ԂȂ�Actor�̈ʒu�̕ۑ�
	if (m_IsRepulsion)
	{
		// �����񂹏�Ԃ�؂�
		m_IsAttract = false;

		// ���b�N�I������
		m_TPSCamera->DisableLockOn();

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu�𔽔��̊�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ���
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ��Ȃ��i�I�u�W�F�N�g�𔽔��������ԁj
			m_IsTargetOfAbilityPlayer = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetIsLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object (Repulsion)"));
		}
		// �\�͂̑Ώۂɂ͂Ȃ�Ȃ�
		else
		{

		}
	}
	else
	{
		// �ڕW�n�_�̃��Z�b�g
		//m_TargetActorLocation = FVector::ZeroVector;

		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;

		// ���b�N�I�����S����
		m_TPSCamera->DisableLockOn(true);
	}
}


// �������֐�
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


// �����񂹏����֐�
void UMagnetComponent::Attract(float _DeltaTime)
{
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

	// �v���C���[�������񂹂�
	if (m_IsTargetOfAbilityPlayer)
	{
		// �I�u�W�F�N�g�ƃv���C���[�̋����v��
		float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TPSCamera->GetIsLockOnActor()->GetActorLocation()).Size();

		// �v���C���[�ƃI�u�W�F�N�g�̃o�E���f�B���O�{�b�N�X�̃T�C�Y�擾
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_TPSCamera->GetIsLockOnActor()->GetActorBounds(true, tmp, objectBound);

		float boundBoxLength = (playerBound + objectBound).Size();

		// �G�ꂽ��Œ�
		if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(m_TPSCamera->GetIsLockOnActor()->GetActorLocation(), boundBoxLength))
		{
			m_IsFreeze = true;
		}
		else
		{
			// �v���C���[�ƃI�u�W�F�N�g�̋����ɉ����ė͂�����������i�߂��قǗ͂���������j
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddForce(m_TPSCamera->GetCameraVectorNormalized() * m_AttractPower * (m_TPSCamera->m_RayLength / length));
		}
	}
	// �I�u�W�F�N�g�������񂹂�
	else
	{
		// ���b�N�I��Actor�����݂��Ă����
		if (m_TPSCamera->GetIsLockOnActor() == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActor is nullptr."));
			return;
		}
		// �I�u�W�F�N�g�ƃv���C���[�̋����v��
		float length = (m_TPSCamera->GetIsLockOnActor()->GetActorLocation() - m_TPSCamera->GetPlayerCharacter()->GetActorLocation()).Size();

		// �v���C���[�ƃI�u�W�F�N�g�̃o�E���f�B���O�{�b�N�X�̃T�C�Y�擾
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_TPSCamera->GetIsLockOnActor()->GetActorBounds(true, tmp, objectBound);

		float boundBoxLength = (playerBound + objectBound).Size();

		// �G�ꂽ��Œ�
		if (m_TPSCamera->GetIsLockOnActor()->GetActorLocation().Equals(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), boundBoxLength + 10))
		{
			m_LockOnActorStaticMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
			m_IsAttract = false;
		}
		else
		{
			// �v���C���[�ƃI�u�W�F�N�g�̋����ɉ����ė͂�����������i�߂��قǗ͂���������j
			FVector toPlayerVector = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TPSCamera->GetIsLockOnActor()->GetActorLocation());
			toPlayerVector.Normalize();
			m_LockOnActorStaticMesh->AddForce(toPlayerVector * m_AttractPower * (m_TPSCamera->m_RayLength / (length * 5)));

		}
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
		// �v���C���[�𔽔�������
		if (m_IsTargetOfAbilityPlayer)
		{
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->AddForce(-m_TPSCamera->GetCameraVectorNormalized() * m_RepulsionPower);
		}
		// �I�u�W�F�N�g�𔽔�������
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


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	//m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}