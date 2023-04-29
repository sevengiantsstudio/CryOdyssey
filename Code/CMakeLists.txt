cmake_minimum_required (VERSION 3.14)
set(CRYENGINE_DIR "C:/CRYENGINE57_SG" CACHE STRING "CRYENGINE root directory.")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../")

include("${TOOLS_CMAKE_DIR}/InitialSetup.cmake")
include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components"
		"Components/ClearAllComponents.cpp"
		"Components/ControllableTimer.cpp"
		"Components/EntityLinks.cpp"
		"Components/InterfaceComponent.cpp"
		"Components/ClearAllComponents.h"
		"Components/ControllableTimer.h"
		"Components/EntityLinks.h"
		"Components/InterfaceComponent.h"
)
add_sources("Camera_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Camera"
		"Components/Camera/CameraController.cpp"
		"Components/Camera/CameraController.h"
)
add_sources("Environment_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Environment"
		"Components/Environment/Door.cpp"
		"Components/Environment/GrabObject.cpp"
		"Components/Environment/Lever.cpp"
		"Components/Environment/Light.cpp"
		"Components/Environment/Note.cpp"
		"Components/Environment/Switch.cpp"
		"Components/Environment/ViewPlace.cpp"
		"Components/Environment/Door.h"
		"Components/Environment/GrabObject.h"
		"Components/Environment/Lever.h"
		"Components/Environment/Light.h"
		"Components/Environment/Note.h"
		"Components/Environment/Switch.h"
		"Components/Environment/ViewPlace.h"
)
add_sources("Game_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Game"
		"Components/Game/AreaTrigger.cpp"
		"Components/Game/BodyDamageComponent.cpp"
		"Components/Game/CinematicTrigger.cpp"
		"Components/Game/HealthComponent.cpp"
		"Components/Game/SpawnPoint.cpp"
		"Components/Game/AreaTrigger.h"
		"Components/Game/BodyDamageComponent.h"
		"Components/Game/CinematicTrigger.h"
		"Components/Game/HealthComponent.h"
		"Components/Game/SpawnPoint.h"
)
add_sources("Items_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Items"
		"Components/Items/ItemComponent.cpp"
		"Components/Items/RifleComponent.cpp"
		"Components/Items/ItemComponent.h"
		"Components/Items/RifleComponent.h"
)
add_sources("Physics_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Physics"
		"Components/Physics/CollisionMask.cpp"
		"Components/Physics/ExplosionComponent.cpp"
		"Components/Physics/CollisionMask.h"
		"Components/Physics/ExplosionComponent.h"
)
add_sources("Player_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Player"
		"Components/Player/PlayerAnimationComponent.cpp"
		"Components/Player/PlayerComponent.cpp"
		"Components/Player/PlayerInteractionComponent.cpp"
		"Components/Player/PlayerAnimationComponent.h"
		"Components/Player/PlayerComponent.h"
		"Components/Player/PlayerInteractionComponent.h"
)
add_sources("Vehicle_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components\\\\Vehicle"
		"Components/Vehicle/VehicleController.cpp"
		"Components/Vehicle/VehicleController.h"
)
add_sources("Core_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Core"
		"Core/Game.cpp"
		"Core/Game.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Core\\\\Components"
		"Core/Components/EventSubscriber.cpp"
		"Core/Components/InputEventListener.cpp"
		"Core/Components/EventSubscriber.h"
		"Core/Components/InputEventListener.h"
)
add_sources("GameEvents_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Core\\\\GameEvents"
		"Core/GameEvents/GameEvents.cpp"
		"Core/GameEvents/GameEvents.h"
		"Core/GameEvents/IGameEvents.h"
)
add_sources("GameInput_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Core\\\\GameInput"
		"Core/GameInput/GameInput.cpp"
		"Core/GameInput/InputEvents.cpp"
		"Core/GameInput/GameInput.h"
		"Core/GameInput/IInputEvents.h"
		"Core/GameInput/InputEvents.h"
)
add_sources("Env_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Env"
		"Env/GameEnv.cpp"
		"Env/GameEnv.h"
)
add_sources("Nodes_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Env\\\\Nodes"
		"Env/Nodes/GameFunctions.cpp"
		"Env/Nodes/GameFunctions.h"
)
add_sources("FlowNodes_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "FlowNodes"
		"FlowNodes/EntityFlowNode.cpp"
		"FlowNodes/FlowCameraNodes.cpp"
		"FlowNodes/FlowGameNodes.cpp"
		"FlowNodes/FlowLocalizationNodes.cpp"
		"FlowNodes/FlowMovieCustomNodes.cpp"
		"FlowNodes/FlowPlayerNodes.cpp"
		"FlowNodes/EntityFlowNode.h"
)
add_sources("Interfaces_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Interfaces"
		"Interfaces/IDoor.h"
		"Interfaces/IGrabObject.h"
		"Interfaces/IHandObject.h"
		"Interfaces/IInteractAction.h"
		"Interfaces/IInterfaceBase.h"
		"Interfaces/IItem.h"
		"Interfaces/ILever.h"
		"Interfaces/IPawn.h"
		"Interfaces/ITakeDamage.h"
		"Interfaces/IVehicle.h"
		"Interfaces/IWeapon.h"
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CVarOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "CVarOverrides.h"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineDefineOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "EngineDefineOverrides.h"
    )
endif()
end_sources()

CryGameModule(Game FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
    "${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
    "${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-variable
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-dynamic-class-memaccess
        -Wno-unused-private-field
        -Wno-unused-value
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-null-conversion
        -Wno-empty-body
        -Wno-unused-lambda-capture
        -Wno-unused-function
        -Wno-tautological-constant-out-of-range-compare
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-function
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-aligned-new
        -Wno-int-in-bool-context
        -Wno-invalid-offsetof
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-but-set-variable
        -Wno-maybe-uninitialized
        -Wno-misleading-indentation
        -Wno-unused-result
        -Wno-unknown-pragmas
        -Wno-multichar
        -Wno-strict-aliasing
        -Wno-char-subscripts
        -Wno-conversion-null
        -Wno-reorder
        -Wno-parentheses
        -Wno-format
        -Wno-switch
    )
endif()


if(OPTION_ENGINE)
    if(NOT EXISTS "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
        add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
        set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
        if (WIN32)
            set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/Game.cryproject\"")
        endif()
    endif()
else()
    add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/Game.cryproject\"")
    endif()

    add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/Game.cryproject\"")
    endif()

    add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"${PROJECT_DIR}/Game.cryproject\"")
    endif()
endif()

# Set StartUp project in Visual Studio
set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/Game.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
#END-CUSTOM