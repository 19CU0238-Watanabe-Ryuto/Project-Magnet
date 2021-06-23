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
// 2021/05/29 �n粗��� ���b�N�I���Ώۂ��L����
// 2021/05/30 �n粗��� ���b�N�I���Ώۂ��R���C�_�[�ɂ���čs���e�X�g
// 2021/06/02 �n粗��� ���b�N�I���s��Actor�𕡐��ݒ�ł���l��
//					   SwitchLockOn�֐��Ń��b�N�I������Actor��Ԃ��悤��
//					   ���b�N�I���\�Ȉ�ԋ߂�Actor�im_NearCanLockOnActor�j���擾�ł���悤��
// 2021/06/04 �n粗��� ���b�N�I�����ɒf��I�ɉ�p��������

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_BoxComponent(nullptr)
	, m_RayHitLocation(FVector::ZeroVector)
	, m_IsHit(false)
	, m_IsHitCanLockOnActor(false)
	, m_CollisionParams(FCollisionQueryParams::DefaultQueryParam)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_NearCanLockOnActor(nullptr)
	, m_NowFOV(90.0f)
	, m_LockOnTimer(0.0f)
	, m_DisableLockOnLength(0.0f)
	, m_FOVSmoothTime(0.2f)
	, m_NormalFOV(90.0f)
	, m_LockOnFOV(50.0f)
	, m_LockOnTag("CanLockOn")
	, m_BoxCollisionSize(FVector(7500.0f, 100.0f, 100.0f))
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UTPSCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_BoxComponent != nullptr)
	{
		// �R���W�����������I�ɖ��t���[���L���ɂ���
		m_BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] BoxComponent is null."))
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
	// ���b�N�I������
	LockOn(DeltaTime);
}


void UTPSCameraComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(m_LockOnTag))
	{
		m_OnCollisionCanLockOnActorArray.AddUnique(OtherActor);
	}
}


void UTPSCameraComponent::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(m_LockOnTag))
	{
		m_OnCollisionCanLockOnActorArray.Remove(OtherActor);
	}
}


// ���b�N�I�������֐�
void UTPSCameraComponent::LockOn(float DeltaTime)
{
	FVector start = m_CameraComponent->GetComponentLocation();
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_BoxCollisionSize.X;
	FVector end = start + forwardVec;

	m_CameraVector = end - start;

	// ���C���΂�
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, DeltaTime);
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Pawn, m_CollisionParams);

	// �J��������̃��C�����������ꏊ��ۑ�
	if (m_IsHit)
	{
		m_RayHitLocation = m_HitResult.Location;
	}
	else
	{
		m_RayHitLocation = end;
	}

	// ��ԋ߂�Actor�����Z�b�g����
	m_NearCanLockOnActor = nullptr;


	// ��ԋ߂�Actor���ǂꂩ����
	float nearLength = m_BoxCollisionSize.X;

	// �ۑ����Ă���Actor�̌������[�v����
	for (AActor* nearActor : m_OnCollisionCanLockOnActorArray)
	{
		if (!nearActor)
		{
			continue;
		}

		if (!m_PlayerCharacter)
		{
			continue;
		}
				
		if (nearActor == GetPlayerCharacter())
		{
			continue;
		}

		// Actor�̃o�E���f�B���O�{�b�N�X���擾
		FVector bound = FVector::ZeroVector;
		FVector ex = FVector::ZeroVector;

		nearActor->GetActorBounds(true, bound, ex);

		// Actor�Ƃ̋������v��
		float length = (bound - m_PlayerCharacter->GetActorLocation()).Size();

		/*
		// Actor�Ƃ̊p�x���Z�o
		FVector cameraVec = m_CameraComponent->GetForwardVector();
		FRotator objectRot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), bound);
		FVector objectVec = UKismetMathLibrary::GetForwardVector(objectRot);

		float distance = FVector::Dist(cameraVec, objectVec);
		float deg = UKismetMathLibrary::DegAcos(distance);

		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Degree = %f Actor = %s"), deg, *lockOnActor->GetName());

		if (deg > m_CanLockOnDegree)
		{

		}
		*/

		bool cantLockOn = false;

		for (AActor* ignore : m_CantLockOnActorArray)
		{
			// �ꎞ�I�Ƀ��b�N�I���o���Ȃ�Actor�ł���Ώ������I��
			if (nearActor == ignore)
			{
				cantLockOn = true;
				break;
			}
		}

		if (!cantLockOn)
		{
			// �J��������̃��C���������Ă����
			if (m_IsHit)
			{
				// �J�����̃��C�̓��������ʒu����Actor�܂Ń��C���΂�
				FHitResult res;

				// ���C�̏Փ˂����݂��ɓ�����Ȃ��悤�ɂ���
				FCollisionQueryParams colParams;
				colParams.AddIgnoredActor(m_PlayerCharacter);
				colParams.AddIgnoredActor(nearActor);

				bool isHit = GetWorld()->LineTraceSingleByChannel(res, bound, m_RayHitLocation, ECollisionChannel::ECC_Pawn, colParams);

				// ���b�N�I���\�͈͂ł��A�J�����̊p�x���͈͓��ŁA����Ƀ��C���������Ă��Ȃ���΁i�Ղ���̂��Ȃ��j
				if (length < nearLength && !isHit)
				{
					m_NearCanLockOnActor = nearActor;
					nearLength = length;
				}
			}
			else
			{
				m_NearCanLockOnActor = nearActor;
				nearLength = length;
			}
		}		
	}
	m_LockOnTimer += DeltaTime;

	if (m_LockOnTimer > m_FOVSmoothTime)
	{
		m_LockOnTimer = m_FOVSmoothTime;
	}

	// ���b�N�I����ԂłȂ���Ώ������I��
	if (!m_IsLockOn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not lock-On."));

		m_NowFOV = FMath::Lerp(m_LockOnFOV, m_NormalFOV, m_LockOnTimer / m_FOVSmoothTime);
		m_CameraComponent->FieldOfView = m_NowFOV;

		return;
	}
	else
	{
		m_NowFOV = FMath::Lerp(m_NormalFOV, m_LockOnFOV, m_LockOnTimer / m_FOVSmoothTime);
		m_CameraComponent->FieldOfView = m_NowFOV;
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
	float radius;

	UKismetSystemLibrary::GetComponentBounds(m_LockOnActor->GetRootComponent(), targetOrigin, targetExtent, radius);

	//m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ���b�N�I�����Ă���I�u�W�F�N�g�Ƃ̋��������C�̋����ȏ�ɂȂ����烍�b�N�I����ԉ���
	if (targetDistance >= m_BoxCollisionSize.X)
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

	if (m_PlayerCharacter != nullptr && m_PlayerCharacter->GetController() != nullptr)
	{
		//m_PlayerCharacter->GetController()->SetControlRotation(rot);
	}
}


// m_CameraVector�擾�p�i���K���j
FVector UTPSCameraComponent::GetCameraVectorOtherActor(FVector _originPos)
{
	if (m_CameraComponent != nullptr)
	{
		// ���C�I���ʒu
		FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation());
		FVector end = _originPos + forwardVec;

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
FVector UTPSCameraComponent::GetCameraVectorNormalizedOtherActor(FVector _originPos)
{
	FVector normal = GetCameraVectorOtherActor(_originPos);
	normal.Normalize();

	return normal;
}


// �������֐�
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character, UBoxComponent* _boxConponent)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;
	m_BoxComponent = _boxConponent;

	m_LockOnTimer = m_FOVSmoothTime;

	// ���C�̏Փ˂��������g�̃L�����N�^�[�ɓ�����Ȃ��悤�ɂ���
	m_CollisionParams.AddIgnoredActor(_character);

	if (m_BoxComponent != nullptr)
	{
		FVector tmpSize = m_BoxCollisionSize;
		tmpSize.X = m_BoxCollisionSize.X / 2.0f;

		FVector tmpLoc = m_BoxComponent->GetRelativeLocation();
		tmpLoc.X += m_BoxCollisionSize.X / 2.0f;

		m_BoxComponent->SetBoxExtent(tmpSize);
		m_BoxComponent->SetRelativeLocation(tmpLoc);

		m_BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UTPSCameraComponent::OnOverlapBegin);
		m_BoxComponent->OnComponentEndOverlap.AddDynamic(this, &UTPSCameraComponent::OnComponentOverlapEnd);
	}

	m_OnCollisionCanLockOnActorArray.Empty();
}


// ���b�N�I����Ԃɂ���֐�
AActor* UTPSCameraComponent::SwitchLockOn()
{
	// ���b�N�I����Ԃ̃X�C�b�`
	m_IsLockOn = !m_IsLockOn;

	if (m_IsLockOn)
	{
		/*
		// ���C�̓������Ă���Actor��ێ�
		m_LockOnActor = m_HitResult.GetActor();
		*/

		// ��ԋ߂���Actor��ێ�
		m_LockOnActor = m_NearCanLockOnActor;

		// Actor���擾�ł��Ă��Ȃ�
		if (m_LockOnActor == nullptr)
		{
			m_IsLockOn = false;
			return nullptr;
		}
		// ���b�N�I���o���Ȃ�Actor�ł���Ώ������I��
		/*
		else if (!m_LockOnActor->ActorHasTag(m_LockOnTag))
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] Lock-on enable tag is not set for the Actor who tried to lock-on."));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}*/

		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;

		/*
		// ���b�N�I���Ώۂ��������g�ł���Ώ������I��
		else if (m_LockOnActor == GetPlayerCharacter())
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] The player himself is the target of lock-on"));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}*/
	}
	else
	{
		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;
	}

	return m_LockOnActor;
}


// ���b�N�I����Ԃ���������֐�
void UTPSCameraComponent::DisableLockOn(bool isResetLockOnActor/* = false*/)
{
	if (m_IsLockOn)
	{
		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;
	}
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


// ���b�N�I���ʒu�̎擾
FVector UTPSCameraComponent::GetLockOnLocation()
{
	if (m_IsLockOn && m_LockOnActor != nullptr)
	{
		FVector targetOrigin;
		FVector targetExtent;

		m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

		return targetOrigin;
	}
	return FVector::ZeroVector;
}