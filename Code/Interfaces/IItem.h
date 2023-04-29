#pragma once

#include <Interfaces/IInterfaceBase.h>

struct IItem : public IInterfaceBase
{
	virtual void StartUseAction() = 0;
	virtual bool StopUseAction() = 0;
	virtual bool IsCanUseAction() const = 0;
};