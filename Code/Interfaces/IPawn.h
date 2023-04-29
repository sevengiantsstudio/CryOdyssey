#pragma once

#include <Interfaces/IInterfaceBase.h>

struct IPawn : public IInterfaceBase
{
	virtual void EnableInput() = 0;
	virtual void DisableInput() = 0;
	virtual void ActivateView() = 0;
};