// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemShootComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UItemShootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemShootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool m_IsShoot;

public:
	bool GetIsShoot()
	{
		return m_IsShoot;
	}

public:
	// �������Ƃ��̊֐�
	void SetIsShoot(bool _status);
};
