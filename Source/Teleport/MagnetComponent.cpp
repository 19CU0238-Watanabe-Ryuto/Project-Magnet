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

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_LockOnActorStaticMesh(nullptr)
	, m_AttractFloatingPoint(nullptr)
	, m_IsAttract(false)
	, m_IsRepulsion(false)
	, m_IsFreeze(false)
	, m_IsTargetOfAbilityPlayer(false)
	//, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPlayerPower(5.0f)
	, m_AttractObjectPower(5.0f)
	, m_RepulsionPlayerPower(20000000.0f)
	, m_RepulsionObjectPower(20000000.0f)
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

	// ���b�N�I������
	m_TPSCamera->DisableLockOn();

	// �����񂹏�ԂȂ�Actor�̈ʒu�̕ۑ��E�d�͂̕ύX
	if (m_IsAttract)
	{
		// ������Ԃ�؂�
		m_IsRepulsion = false;

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
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
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�����񂹏�Ԃɂ��Ȃ��i�I�u�W�F�N�g�����񂹏�ԁj
			m_IsTargetOfAbilityPlayer = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

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
	if (m_TPSCamera->GetLockOnActor() == nullptr)
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
		if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu�𔽔��̊�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ���
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			//m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ��Ȃ��i�I�u�W�F�N�g�𔽔��������ԁj
			m_IsTargetOfAbilityPlayer = false;

			// ���b�N�I��Actor�̃X�^�e�B�b�N���b�V���擾
			m_LockOnActorStaticMesh = Cast<UStaticMeshComponent>(m_TPSCamera->GetLockOnActor()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

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
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent, UStaticMeshComponent* _attractFloatingPoint)
{
	m_TPSCamera = _TPSCameraComponent;
	m_AttractFloatingPoint = _attractFloatingPoint;

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
		if (m_TPSCamera->GetPlayerCharacter() == nullptr || m_TPSCamera->GetLockOnActor() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera->Playercharacter or TPSCamera->LockOnActor is nullptr."));
			return;
		}
		
		// �I�u�W�F�N�g�ƃv���C���[�̋����v��
		float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TPSCamera->GetLockOnActor()->GetActorLocation()).Size();

		// �v���C���[�ƃI�u�W�F�N�g�̃o�E���f�B���O�{�b�N�X�̃T�C�Y�擾
		FVector playerBound;
		FVector objectBound;
		FVector tmp;

		m_TPSCamera->GetPlayerCharacter()->GetActorBounds(true, tmp, playerBound);

		m_TPSCamera->GetLockOnActor()->GetActorBounds(true, tmp, objectBound);

		float boundBoxLength = (playerBound + objectBound).Size();

		// �G�ꂽ��Œ�
		if (length <= boundBoxLength)
		{
			m_IsFreeze = true;
		}
		else
		{
			FVector lerpPos = UKismetMathLibrary::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), m_TPSCamera->GetLockOnActor()->GetActorLocation(), _DeltaTime, m_AttractPlayerPower);
			m_TPSCamera->GetPlayerCharacter()->SetActorLocation(lerpPos);
		}		
	}
	// �I�u�W�F�N�g�������񂹂�
	else
	{
		// ���b�N�I��Actor�����݂��Ă��Ȃ����
		if (m_TPSCamera->GetLockOnActor() == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActor is nullptr."));
			return;
		}

		// ���b�N�I��Actor��StaticMesh�������ł����
		if (m_LockOnActorStaticMesh == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] LockOnActorStaticMesh is nullptr."));
			return;
		}

		// �v���C���[�̈����񂹃I�u�W�F�N�g�i�[�ʒu�������ł����
		if (m_AttractFloatingPoint == nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] AttractFloatingPoint is nullptr."));
			return;
		}

		FVector lerpPos = UKismetMathLibrary::VInterpTo(m_TPSCamera->GetLockOnActor()->GetActorLocation(), m_AttractFloatingPoint->GetComponentLocation(), _DeltaTime, m_AttractObjectPower);
		m_TPSCamera->GetLockOnActor()->SetActorLocation(lerpPos);
		m_LockOnActorStaticMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
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
			}
		}
	}
}


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	//m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}