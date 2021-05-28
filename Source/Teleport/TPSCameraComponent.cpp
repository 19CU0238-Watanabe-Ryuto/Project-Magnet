// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�iTPSCameraComponentBP����̈ڍs�j
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬
// 2021/05/12 �n粗��� ���b�N�I���Ώۂƈ��̋����ȓ��ɓ������ꍇ�A���b�N�I������������
// 2021/05/14 �n粗��� ���b�N�I���\�^�O���������̂̂݃��b�N�I���\�ɂ���
// 2021/05/17 �n粗��� �J�����̌����Ă�������̃x�N�g�����擾�ł���悤�ɂ���
//					   �v���C���[�ƃI�u�W�F�N�g�̋��������C�̋����ȏ�ɂȂ����烍�b�N�I����������悤��
// 2021/05/24 �n粗��� ���b�N�I���\�I�u�W�F�N�g�ł��ꎞ�I�Ƀ��b�N�I���s�ɂ���@�\��ǉ�
// 2021/05/26 �n粗��� ���g�Ƀ��b�N�I���o���Ȃ��悤��
//					   ���b�N�I���̑ΏۂƂ���ʒu���ǂ̃I�u�W�F�N�g�ł����Ă��^�񒆂ɂ���
// 2021/05/27 �n粗��� ���̃R���|�[�l���g����A�N�Z�X�ł���J�����̒����Ɍ������x�N�g�����擾�ł���֐���ǉ�

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_IsHit(false)
	, m_IsHitCanLockOnActor(false)
	, m_CollisionParams(FCollisionQueryParams::DefaultQueryParam)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CantLockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_EnableLineTrace(true)
	, m_IsDrawDebugLine(true)
	, m_DrawDebugLineTime(0.5f)
	, m_LockOnRayColor(FColor::Red)
	, m_NoLockOnRayColor(FColor::Green)
	, m_HitRayColor(FColor::Blue)
	, m_RayLength(5000.0f)
	, m_RayOffset(FVector::ZeroVector)
	, m_DisableLockOnLength(0.0f)
	, m_LockOnTag("CanLockOn")
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UTPSCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ���C���g���[�X�����Ȃ��ꍇ�A�֐����I��
	if (!m_EnableLineTrace)
	{
		return;
	}

	// �J�����R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (m_CameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Camera is null."));
		return;
	}

	// �v���C���[�R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (m_PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] PlayerCharacter is null."));
		return;
	}

	// ���C�J�n�ʒu
	FVector start = m_CameraComponent->GetComponentLocation();

	// ���C�I���ʒu
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_RayLength;
	FVector end = start + forwardVec + m_RayOffset;

	// �J�����̕����x�N�g���i�[
	m_CameraVector = end - start;

	// ���C�̓������Ă�����̂ɉ����ă��C�̐F��ύX
	FColor lineColor;

	if (m_IsLockOn)
	{
		lineColor = m_LockOnRayColor;
		m_IsHitCanLockOnActor = true;
	}
	else if (m_HitResult.GetActor() == nullptr)
	{
		lineColor = m_NoLockOnRayColor;
		m_IsHitCanLockOnActor = false;
	}
	else if (m_HitResult.GetActor()->ActorHasTag(m_LockOnTag))
	{
		lineColor = m_HitRayColor;
		m_IsHitCanLockOnActor = true;
	}
	else
	{
		lineColor = m_NoLockOnRayColor;
		m_IsHitCanLockOnActor = false;
	}

	// �f�o�b�O�m�F�p�̃��C����`��
	if (m_IsDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), start, end, lineColor, false, m_DrawDebugLineTime);
	}

	// ���C���΂������蔻������
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Pawn, m_CollisionParams);

	// ���b�N�I������
	LockOn();
}


// ���b�N�I�������֐�
void UTPSCameraComponent::LockOn()
{
	// ���b�N�I����ԂłȂ���Ώ������I��
	if (!m_IsLockOn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not lock-On."));
		return;
	}

	// ���b�N�I���Ώۂ��L���łȂ���Ώ������I��
	if (m_LockOnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
		return;
	}

	// �o�E���f�B���O�{�b�N�X�擾
	FVector targetOrigin;
	FVector targetExtent;

	m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ���b�N�I�����Ă���I�u�W�F�N�g�Ƃ̋��������C�̋����ȏ�ɂȂ����烍�b�N�I����ԉ���
	if (targetDistance >= m_RayLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player disabled lock-on because the distance between the player and the object is longer than RayLength."));
		m_IsLockOn = false;
		return;
	}

	// ���b�N�I���ΏۂƂ̋������ݒ�l�ȉ��ł���΃��b�N�I����Ԃ��������������I��
	if (targetDistance < m_DisableLockOnLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player got within a certain distance, so player disabled lock-on."));
		//m_IsLockOn = false;
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Lock-on Actor is \"%s\"."), *m_LockOnActor->GetName());

	//FVector targetLocation = m_LockOnActor->GetActorLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), targetOrigin);

	m_PlayerCharacter->GetController()->SetControlRotation(rot);
}


// m_CameraVector�擾�p�i���K���j
FVector UTPSCameraComponent::GetCameraVectorOtherActor(FVector _originPos)
{
	if (m_CameraComponent != nullptr)
	{
		// ���C�J�n�ʒu
		FVector start = m_CameraComponent->GetComponentLocation();

		// ���C�I���ʒu
		FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_RayLength;
		FVector end = start + forwardVec + m_RayOffset;

		FVector vector = end - _originPos;

		return vector;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] CameraComponent is nullptr."))
		return FVector::ZeroVector;
	}
}


// m_CameraVector�擾�p�i���K���j
UFUNCTION(BlueprintPure)
FVector UTPSCameraComponent::GetCameraVectorNormalizedOtherActor(FVector _originPos)
{
	FVector normal = GetCameraVectorOtherActor(_originPos);
	normal.Normalize();

	return normal;
}


// �������֐�
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;

	// ���C�̏Փ˂��������g�̃L�����N�^�[�ɓ�����Ȃ��悤�ɂ���
	m_CollisionParams.AddIgnoredActor(_character);
}


// ���b�N�I����Ԃɂ���֐�
void UTPSCameraComponent::SwitchLockOn()
{
	// ���b�N�I����Ԃ̃X�C�b�`
	m_IsLockOn = !m_IsLockOn;

	if (m_IsLockOn)
	{
		// ���C�̓������Ă���Actor��ێ�
		m_LockOnActor = m_HitResult.GetActor();

		// Actor���擾�ł��Ă��Ȃ�
		if (m_LockOnActor == nullptr)
		{
			m_IsLockOn = false;
		}
		// ���b�N�I���o���Ȃ�Actor�ł���Ώ������I��
		else if (!m_LockOnActor->ActorHasTag(m_LockOnTag))
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] Lock-on enable tag is not set for the Actor who tried to lock-on."));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}
		// �ꎞ�I�Ƀ��b�N�I���o���Ȃ�Actor�ł���Ώ������I��
		else if (m_LockOnActor == m_CantLockOnActor)
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] This Actor(%s) is temporarily unable to lock on."), *m_LockOnActor->GetName());
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}
		// ���b�N�I���Ώۂ��������g�ł���Ώ������I��
		else if (m_LockOnActor == GetPlayerCharacter())
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] The player himself is the target of lock-on"));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}
	}
}


// ���b�N�I����Ԃ���������֐�
void UTPSCameraComponent::DisableLockOn(bool isResetLockOnActor/* = false*/)
{
	m_IsLockOn = false;

	if (isResetLockOnActor)
	{
		m_LockOnActor = nullptr;
	}
}


// �ēx���b�N�I����Ԃɂ���֐�
bool UTPSCameraComponent::LockOnAgain()
{
	if (m_IsLockOn == false && m_LockOnActor != nullptr)
	{
		m_IsLockOn = true;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Could not reset to lock-on state because LockOnActor is not enabled or already lock-on."));
		return false;
	}
}