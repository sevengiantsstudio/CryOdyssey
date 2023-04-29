#pragma once

#include <Interfaces/IInterfaceBase.h>
#include <CryMath/Cry_Math.h>

struct IVehicle : public IInterfaceBase
{
	virtual void StartInteraction(const Vec3& pt) = 0;
	virtual void StopInteraction() = 0;
	virtual bool GetClosestInteractiveEntity(const Vec3& pt) = 0;
};