#include "StdAfx.h"
#include "GameFunctions.h"
#include "../GameEnv.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvDataType.h>

#include <IActionMapManager.h>

// Copied from Code\CryEngine\CrySchematyc\Core\Impl\CoreEnv\CoreEnv.h
static constexpr CryGUID g_mathModuleGUID = "f3800127-ed71-4d1e-a178-87aac8ee74ed"_cry_guid;

namespace Schematyc
{

	namespace Game
	{
		void GetInputActivationMode(const int& activationMode, bool& press, bool& release)
		{
			press = release = false;
			if (activationMode == eAAM_OnPress)
			{
				press = true;
			}
			if (activationMode == eAAM_OnRelease)
			{
				release = true;
			}
		}

		void ExecuteConsoleCommand(const Schematyc::CSharedString& command, const bool silent, const bool deferredExec)
		{
			gEnv->pConsole->ExecuteString(command.c_str(), silent, deferredExec);
		}

	} // Game

	namespace Entity
	{

		bool IsValid(const ExplicitEntityId& entity)
		{
			return entity != ExplicitEntityId::Invalid;
		}

		bool IsEqual(const ExplicitEntityId& left, const ExplicitEntityId& right)
		{
			return left == right;
		}

		bool IsNotEqual(const ExplicitEntityId& left, const ExplicitEntityId& right)
		{
			return !IsEqual(left, right);
		}

		bool IsClassnameValid(const EntityClassName& classname)
		{
			return gEnv->pEntitySystem->GetClassRegistry()->FindClass(classname.value) != nullptr;
		}

		bool IsClassnameEqual(const EntityClassName& left, const EntityClassName& right)
		{
			return left == right;
		}

		bool IsClassnameNotEqual(const EntityClassName& left, const EntityClassName& right)
		{
			return !IsClassnameEqual(left, right);
		}

		static EntityClassName GetClassName(const ExplicitEntityId& entityId)
		{
			if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(static_cast<EntityId>(entityId)))
			{
				return pEntity->GetClass()->GetName();
			}

			return "";
		}

		static CSharedString ClassnameToString(const EntityClassName& classname)
		{
			return classname.value;
		}
	} // Entity

	void RegisterGameFunctions(Schematyc::IEnvRegistrar& registrar)
	{
		{
			Schematyc::CEnvRegistrationScope scope = registrar.Scope(g_gameModuleGUID);
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Game::GetInputActivationMode, "{F5375043-5C1C-4F59-91F0-B2C984DA216F}"_cry_guid, "GetInputActivationMode");
				pFunction->SetDescription("...");
				pFunction->BindInput(1, 'avm', "ActivationMode", "Activation mode.");
				pFunction->BindOutput(2, 'prs', "Press", "Press.");
				pFunction->BindOutput(3, 'rls', "Release", "Release.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Game::ExecuteConsoleCommand, "{CCC9F1E6-2A46-4C14-8F06-6B3568736DE9}"_cry_guid, "ExecuteConsoleCommand");
				pFunction->SetDescription("Executes the string provided to the engine console.");
				pFunction->BindInput(1, 'cmnd', "Command", "The string to execute in the engine console.");
				pFunction->BindInput(2, 'silt', "Silent", "Whether the activation of this command appears in the console or logs.");
				pFunction->BindInput(3, 'defr', "Deferred", "True to execute this command on the next engine tick. False to execute immediately.");
				scope.Register(pFunction);
			}
		}

		// Entity Type Extensions
		{
			Schematyc::CEnvRegistrationScope scope = registrar.Scope(Schematyc::GetTypeDesc<ExplicitEntityId>().GetGUID());
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsEqual, "{9C40A312-8304-4F5D-962F-A22CFBBE3248}"_cry_guid, "IsEqual");
				pFunction->SetDescription("Returns true if the entities A & B are the same entity.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entities provided are the same entity.");
				pFunction->BindInput(1, 'enta', "Entity A", "The first entity to match with.");
				pFunction->BindInput(2, 'entb', "Entity B", "The next entity to match against.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsNotEqual, "{F9DB57CA-5C65-4B0F-B9EB-C622D8CF5DA9}"_cry_guid, "IsNotEqual");
				pFunction->SetDescription("Returns true if the entities A & B are the different entities.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entities provided are not the same entity.");
				pFunction->BindInput(1, 'enta', "Entity A", "The first entity to match with.");
				pFunction->BindInput(2, 'entb', "Entity B", "The next entity to match against.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsValid, "{91DE546E-DEE6-484D-BB9A-A207A0628E22}"_cry_guid, "IsValid");
				pFunction->SetDescription("Returns true if the entity provided is a valid entity. Note: Only checks if the Id is a valid Id. Not if the entity associated to this Id exists.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entity is valid.");
				pFunction->BindInput(1, 'ent', "Entity", "The entity to check validity.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::GetClassName, "{7D1EDA77-D951-4F19-9F18-EF64C1317CCC}"_cry_guid, "GetClassName");
				pFunction->SetDescription("Gets the classname of the given entity.");
				pFunction->BindOutput(0, 'ecls', "Entity Classname", "The classname of the entity.");
				pFunction->BindInput(1, 'eid', "Entity", "The entity to get the classname of.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsClassnameEqual, "{C23D5C3A-ADA5-444B-AA00-9FFB59FB28E9}"_cry_guid, "IsClassEqual");
				pFunction->SetDescription("Returns true if the entity classes A & B are the same type.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entity classes A & B are the same type.");
				pFunction->BindInput(1, 'clsa', "Classname A", "The first entity classname to match with.");
				pFunction->BindInput(2, 'clsb', "Classname B", "The next entity classname to match against.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsClassnameNotEqual, "{92C6FE83-4B10-4F17-A07F-9F87929897B5}"_cry_guid, "IsClassNotEqual");
				pFunction->SetDescription("Returns true if the entity classes A & B are different types.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entity classes provided are not the same type.");
				pFunction->BindInput(1, 'clsa', "Classname A", "The first entity classname to match with.");
				pFunction->BindInput(2, 'clsb', "Classname B", "The next entity classname to match against.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::IsClassnameValid, "{753001E8-5066-428B-9A23-62487AFCACB3}"_cry_guid, "IsClassValid");
				pFunction->SetDescription("Returns true if the entity classname provided is a valid type.");
				pFunction->BindOutput(0, 'res', "Result", "True if the entity classname is valid.");
				pFunction->BindInput(1, 'ent', "Classname", "The entity classname to check validity.");
				scope.Register(pFunction);
			}
			{
				auto const pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&Entity::ClassnameToString, "{E2E90398-468D-42DE-9537-F46555D8731F}"_cry_guid, "ClassnameToString");
				pFunction->SetDescription("Gets the string representation of the entity class.");
				pFunction->BindOutput(0, 'cstr', "Output", "The string representation of the classname.");
				pFunction->BindInput(1, 'cls', "Classname", "The classname to convert to string.");
				scope.Register(pFunction);
			}
		}
	}

} // Schematyc