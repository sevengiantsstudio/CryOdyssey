#pragma once

#include <Interfaces/IInterfaceBase.h>

struct IWeapon : public IInterfaceBase
{
	int type = 0;

	virtual void StartFire() = 0;
	virtual void StopFire() = 0;
};