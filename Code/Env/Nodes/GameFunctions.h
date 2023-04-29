#pragma once

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryPhysics/physinterface.h>

namespace Schematyc
{

	void RegisterGameFunctions(Schematyc::IEnvRegistrar& registrar);

	namespace Game
	{
		void GetInputActivationMode(const int& activationMode, bool& press, bool& release);

		void ExecuteConsoleCommand(const Schematyc::CSharedString& command, const bool silent, const bool deferredExec);
	} // Game

	// Entity Node Extensions
	namespace Entity
	{

		bool IsValid(const ExplicitEntityId& entity);
		bool IsEqual(const ExplicitEntityId& left, const ExplicitEntityId& right);
		bool IsNotEqual(const ExplicitEntityId& left, const ExplicitEntityId& right);
		bool IsClassnameValid(const EntityClassName& classname);
		bool IsClassnameEqual(const EntityClassName& left, const EntityClassName& right);
		bool IsClassnameNotEqual(const EntityClassName& left, const EntityClassName& right);
		EntityClassName GetClassName(const ExplicitEntityId& entityId);
		CSharedString ClassnameToString(const EntityClassName& classname);

	} // Entity
} // Schematyc