// �ǂ𐶐�����N���X
//
// 2021/07/14 �ǂ��v���C���[�̑O�ɐ�������@�\�̂ݎ���
// 2021/07/28 �񐔐�������������

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallSpawnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UWallSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWallSpawnComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AActor* SpawnActor(const TSubclassOf<AActor> _Actor, const FVector _Pos, const FRotator _Rot);

	int m_SpawnCount;

public:
	UPROPERTY(EditAnyWhere)
		TSubclassOf<AActor> m_WallActor;

	UPROPERTY(EditAnyWhere)
		int m_SpawnNum;

	UFUNCTION(BlueprintCallable)
		AActor* SpawnWall(const FVector pos, const FRotator rot);

	UFUNCTION(BlueprintCallable)
		void DisappearWall()
	{
		m_SpawnCount--;
	}
};
