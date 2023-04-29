#include "StdAfx.h"
#include "GameEnv.h"
#include "Nodes/GameFunctions.h"

#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvModule.h>

namespace Schematyc
{
void RegisterGameEnvPackage(IEnvRegistrar& registrar)
{
	registrar.RootScope().Register(SCHEMATYC_MAKE_ENV_MODULE(g_gameModuleGUID, "Game"));
	RegisterGameFunctions(registrar);
}
} // Schematyc