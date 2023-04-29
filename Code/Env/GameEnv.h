#pragma once

#include <CryExtension/CryGUID.h>

namespace Schematyc
{
static constexpr CryGUID g_gameModuleGUID = "{EA85C3F0-5865-452B-B134-8F80B3F6DEC3}"_cry_guid;

struct IEnvRegistrar;
void RegisterGameEnvPackage(IEnvRegistrar& registrar);

} // Schematyc