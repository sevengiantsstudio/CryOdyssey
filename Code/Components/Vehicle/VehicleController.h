#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <limits>

#include <CryMath/Angle.h>
#include <CryPhysics/physinterface.h>

#include "Interfaces/IVehicle.h"

class CInterfaceComponent;

struct SOnFire
{
	SOnFire() = default;
};

static void ReflectType(Schematyc::CTypeDesc<SOnFire>& desc)
{
	desc.SetGUID("{5FCCDA08-DE55-4F18-AC46-287D4A37E44C}"_cry_guid);
	desc.SetLabel("OnFire");
	desc.SetDescription("...");
}

class CVehicleController final : public IEntityComponent, public IVehicle
{
public:
	struct SEngineParams
	{
		inline bool operator==(const SEngineParams& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		static void ReflectType(Schematyc::CTypeDesc<SEngineParams>& desc)
		{
			desc.SetGUID("{2493B99A-14D4-4D7C-9C3E-E6C92B5D7152}"_cry_guid);
			desc.SetLabel("Engine Parameters");
			desc.AddMember(&CVehicleController::SEngineParams::m_power, 'powe', "Power", "Power", "Power of the engine", 10000.f);
			desc.AddMember(&CVehicleController::SEngineParams::m_maxRPM, 'maxr', "MaxRPM", "Maximum RPM", "engine torque decreases to 0 after reaching this rotation speed", 1200.f);
			desc.AddMember(&CVehicleController::SEngineParams::m_minRPM, 'minr', "MinRPM", "Minimum RPM", "disengages the clutch when falling behind this limit, if braking with the engine", 60.f);
			desc.AddMember(&CVehicleController::SEngineParams::m_idleRPM, 'idle', "IdleRPM", "Idle RPM", "RPM for idle state", 120.f);
			desc.AddMember(&CVehicleController::SEngineParams::m_startRPM, 'star', "StartRPM", "Start RPM", "RPM when the engine is started", 400.f);
		}

		Schematyc::Range<0, 10000000> m_power = 50000.f;
		float m_maxRPM = 1200.f;
		float m_minRPM = 60.f;
		float m_idleRPM = 120.f;
		float m_startRPM = 400.f;
	};

	struct SGearParams
	{
		inline bool operator==(const SGearParams& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		struct SGear
		{
			inline bool operator==(const SGear& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
			inline bool operator!=(const SGear& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

			void Serialize(Serialization::IArchive& archive)
			{
				archive(m_ratio, "Ratio", "Ratio");
			}

			static void ReflectType(Schematyc::CTypeDesc<SGear>& desc)
			{
				desc.SetGUID("{251BCFD6-5FED-4E0C-923F-8C03F5029F79}"_cry_guid);
				desc.SetLabel("Gear");
				desc.AddMember(&CVehicleController::SGearParams::SGear::m_ratio, 'rati', "Ratio", "Ratio", "assumes 0-backward gear, 1-neutral, 2 and above - forward", 2.f);
			}

			Schematyc::Range<0, 2> m_ratio;
		};

		static void ReflectType(Schematyc::CTypeDesc<SGearParams>& desc)
		{
			desc.SetGUID("{77C8E80F-EDCA-4FE5-BAF1-895C04DD9312}"_cry_guid);
			desc.SetLabel("Gear Parameters");
			desc.AddMember(&CVehicleController::SGearParams::m_gears, 'gear', "Gears", "Gears", "Specifies number of gears, and their parameters", Schematyc::CArray<CVehicleController::SGearParams::SGear>());
			desc.AddMember(&CVehicleController::SGearParams::m_shiftUpRPM, 'shiu', "ShiftUpRPM", "Shift Up RPM", "RPM threshold for for automatic gear switching", 600.f);
			desc.AddMember(&CVehicleController::SGearParams::m_shiftDownRPM, 'shid', "ShiftDownRPM", "Shift Down RPM", "RPM threshold for for automatic gear switching", 240.f);
			desc.AddMember(&CVehicleController::SGearParams::m_directionSwitchRPM, 'dirs', "DirectionSwitchRPM", "Direction Switch RPM", "RPM threshold for switching back and forward gears", 10.f);
		}

		Schematyc::CArray<SGear> m_gears;

		float m_shiftUpRPM = 600.f;
		float m_shiftDownRPM = 240.f;

		float m_directionSwitchRPM = 10.f;
	};

	enum class EGear
	{
		Reverse = -1,
		Neutral,
		// First forward gear, note how there can be more specified by the user
		Forward,

		DefaultCount = Forward + 2
	};

	struct SWheelInfo
	{
		int m_boneId = 0;
		int m_slotId = 0;
	};

	struct SInteractiveHandle
	{
		string m_name = "";
		SInteractiveHandle(string name) : m_name(name)
		{};
	};



	/*struct SControlParams
	{
		inline bool operator==(const SControlParams& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

		struct SControlLever
		{
			inline bool operator==(const SControlLever& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }
			inline bool operator!=(const SControlLever& rhs) const { return 0 != memcmp(this, &rhs, sizeof(rhs)); }

			void Serialize(Serialization::IArchive& archive)
			{
				archive(m_controlName, "Name", "Name");
				archive(m_axis, "Axis", "Axis");
				archive(m_minAngle, "MinAngle", "MinAngle");
				archive(m_maxAngle, "MaxAngle", "MaxAngle");
				archive(m_sensivity, "Sensivity", "Sensivity");
				archive(m_triggerNames, "TriggerNames", "TriggerNames");
			}

			static void ReflectType(Schematyc::CTypeDesc<SControlLever>& desc)
			{
				desc.SetGUID("{F487E160-44CA-4334-B20F-7C044998DC1D}"_cry_guid);
				desc.SetLabel("Lever Control");
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_controlName, 'crn', "Name", "Name", "", "");
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_axis, 'axs', "Axis", "Axis", "", Vec3(0.0f,0.0f,1.0f));
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_minAngle, 'mang', "MinAngle", "Min Angle", "", 0.0f);
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_maxAngle, 'mxan', "MaxAngle", "Max Angle", "", 35.0f);
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_sensivity, 'sns', "Sensivity", "Sensivity", "", 1.0f);
				desc.AddMember(&CVehicleController::SControlParams::SControlLever::m_triggerNames, 'tnms', "TriggerNames", "Trigger Names (Event Name)", "", Schematyc::CArray<Schematyc::CSharedString>());
			}

			Schematyc::CSharedString m_controlName = "";
			Vec3 m_axis = Vec3(0.0f, 0.0f, 1.0f);
			float m_minAngle = 0.0f;
			float m_maxAngle = 35.0f;
			float m_sensivity = 1.0f;
			Schematyc::CArray<Schematyc::CSharedString> m_triggerNames;
		};

		static void ReflectType(Schematyc::CTypeDesc<SControlParams>& desc)
		{
			desc.SetGUID("{47B4C4D5-194A-413D-86D2-EEA970B9BDCC}"_cry_guid);
			desc.SetLabel("Interactive Control Parameters");
			desc.AddMember(&CVehicleController::SControlParams::m_leverControls, 'crls', "Lever Controls", "Lever Controls", "", Schematyc::CArray<CVehicleController::SControlParams::SControlLever>());
		}

		Schematyc::CArray<SControlLever> m_leverControls;
	};*/

	CVehicleController();
	virtual ~CVehicleController();

	static void ReflectType(Schematyc::CTypeDesc<CVehicleController>& desc)
	{
		desc.SetLabel("Vehicle Controller");
		desc.SetEditorCategory("Vehicle");
		desc.SetDescription("Vehicle controller component.");
		desc.SetGUID("{57245EE2-D09F-4E13-89AB-710AF4C10CE3}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::HideFromInspector });

		desc.AddMember(&CVehicleController::m_characterFilePath, 'file', "FilePath", "File", "Determines the animated mesh to load", "");
		desc.AddMember(&CVehicleController::m_materialPath, 'mat', "Material", "Material", "Specifies the override material for the selected object", "");

		desc.AddMember(&CVehicleController::m_engineParams, 'engn', "EngineParams", "Engine Parameters", nullptr, CVehicleController::SEngineParams());
		desc.AddMember(&CVehicleController::m_gearParams, 'gear', "GearParams", "Gear Parameters", nullptr, CVehicleController::SGearParams());

		//desc.AddMember(&CVehicleController::m_controlParams, 'ctrl', "ControlParams", "Control Parameters", nullptr, CVehicleController::SControlParams());
	}

	// IEntityComponent
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	// ~IEntityComponent

	// IVehicle
	virtual void StartInteraction(const Vec3& pt) override;
	virtual void StopInteraction() override;
	virtual bool GetClosestInteractiveEntity(const Vec3& pt) override;
	// ~IVehicle

	void SetCurrentGear(int gearId);
	int GetCurrentGear() const { return m_vehicleStatus.iCurGear - 1; }

	void GearUp() { SetCurrentGear(min(m_vehicleStatus.iCurGear + 1, (int)m_gearParams.m_gears.Size())); }
	void GearDown() { SetCurrentGear(max(m_vehicleStatus.iCurGear - 1, (int)EGear::Reverse + 1)); }

	void SetThrottle(Schematyc::Range<0, 1> ratio);

	void SetSteeringAngle(CryTransform::CAngle angle);

	void UseHandbrake(bool bSet);

	void SetBrake(Schematyc::Range<0, 1> ratio);

	void SetVelocity(const Vec3& velocity);

	Vec3 GetVelocity() const;

	void SpawnEntityAtJoint(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint);

private:
	void Reset();
	void Physicalize();
	void Update();

	// Interact
	void SetupInteractHandleObjects();
	int FindClosestInteractiveObject(const Vec3& pt);
	void UpdateObjectInteraction();


private:
	CInterfaceComponent* m_pInterfaceComponent;

	_smart_ptr<ICharacterInstance> m_pCachedCharacter = nullptr;

	Schematyc::AnyModelFileName m_characterFilePath;
	Schematyc::MaterialFileName m_materialPath;

	std::vector<float> m_gearRatios;

	SEngineParams m_engineParams;
	SGearParams m_gearParams;
	//SControlParams m_controlParams;

	pe_status_vehicle m_vehicleStatus;

	DynArray<SWheelInfo> m_wheelsInfo;
	DynArray<SInteractiveHandle> m_interactiveHandles;

	IAnimationOperatorQueuePtr m_poseWheelsModifier;
	IAnimationOperatorQueuePtr m_posePowerControlModifier;
	IAnimationOperatorQueuePtr m_poseSteeringControlModifier;
	IAnimationOperatorQueuePtr m_poseFireControlModifier;

	// test
	bool m_bIsBrake;

	bool m_bIsInteracted = false;
	int m_currentInteractObjectId = -1;

	float m_powerControlValue = 0.0f;
	float m_steeringControlValue = 0.0f;
	float m_fireControlValue = 0.0f;

	
};