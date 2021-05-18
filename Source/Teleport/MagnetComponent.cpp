// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� MagnetComponentBP����̈ڍs
// 2021/05/14		 �n粗��� �I�u�W�F�N�g�̈����񂹂��s��
// 2021/05/16 - 5/17 �n粗��� ���g�E�I�u�W�F�N�g�̔������s��

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
	// ���b�N�I����ԂłȂ���ΏI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));
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

	// �����񂹏�ԂȂ�Actor�̈ʒu�̕ۑ��E�d�͂̕ύX
	if (m_IsAttract)
	{
		// ������Ԃ�؂�
		m_IsRepulsion = false;

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu��ڕW�n�_�Ƃ��ĕۑ�
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

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
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�����񂹏�Ԃɂ��Ȃ��i�I�u�W�F�N�g�����񂹏�ԁj
			m_IsTargetOfAbilityPlayer = false;

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
		m_TargetActorLocation = FVector::ZeroVector;

		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
	}
}


// ������Ԑ؂�ւ��֐�
void UMagnetComponent::SwitchRepulsion()
{
	// ���b�N�I����ԂłȂ���ΏI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not locked on."));
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

		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu�𔽔��̊�_�Ƃ��ĕۑ�
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ���
			m_IsTargetOfAbilityPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player (Repulsion)"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_TargetOfAbilityObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�𔽔��������Ԃɂ��Ȃ��i�I�u�W�F�N�g�𔽔��������ԁj
			m_IsTargetOfAbilityPlayer = false;

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
		m_TargetActorLocation = FVector::ZeroVector;

		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
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

	// ���b�N�I����ԂłȂ���Έ����񂹏�Ԃ��������ďI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));

		// �����񂹂�����
		m_IsAttract = false;

		// ��~��Ԃ̉���
		m_IsFreeze = false;

		// �ʒu�̃��Z�b�g
		m_TargetActorLocation = FVector::ZeroVector;

		// �d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
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
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(interpPos);

		if (m_TPSCamera->GetPlayerCharacter()->GetActorLocation().Equals(m_TargetActorLocation))
		{
			m_IsFreeze = true;
		}
	}
	// �I�u�W�F�N�g�������񂹂�
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

	// ���b�N�I����ԂłȂ���Δ�����Ԃ��������ďI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Repulsion button was pressed but not locked on."));

		// �����񂹂�����
		m_IsRepulsion = false;

		// �ʒu�̃��Z�b�g
		m_TargetActorLocation = FVector::ZeroVector;

		// �d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
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


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	//m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}