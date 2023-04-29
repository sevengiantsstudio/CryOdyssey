#include "StdAfx.h"
#include "PlayerAttachments.h"

#include <CryEntitySystem/IEntitySystem.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>

#include "PlayerAnimationComponent.h"

static void RegisterPlayerAttachmentsComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerAttachmentsComponent));
		// Functions
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerAttachmentsComponent::SpawnAndAttachEntity, "{32928414-FE25-452B-80BB-A2C5D21B863F}"_cry_guid, "SpawnAndAttachEntity");
			pFunction->BindInput(2, 'clas', "Entity Class");
			pFunction->BindInput(4, 'ang', "Angle");
			pFunction->BindInput(3, 'tran', "JointName");
			pFunction->BindOutput(1, 'ent', "Entity");
			componentScope.Register(pFunction);
		}
		{
			auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CPlayerAttachmentsComponent::SpawnEntityAtJoint, "{C9F98E7E-F132-4715-9ACF-0A4315480F37}"_cry_guid, "SpawnEntityAtJoint");
			pFunction->BindInput(2, 'clas', "Entity Class");
			pFunction->BindInput(3, 'tran', "JointName");
			pFunction->BindOutput(1, 'ent', "Entity");
			componentScope.Register(pFunction);
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerAttachmentsComponent)

void CPlayerAttachmentsComponent::Initialize()
{

	CryCreateClassInstanceForInterface(cryiidof<IAnimationOperatorQueue>(), m_poseHeadTopModifier);
}

Cry::Entity::EventFlags CPlayerAttachmentsComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
}

void CPlayerAttachmentsComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Reset:
	{
		if (gEnv->IsEditor())
		{
			if (event.nParam[0] == 1)
			{

			}
			if (event.nParam[0] == 0)
			{
			}
		}
		else if (!gEnv->IsEditor())
		{

		}
	}
	break;
	case Cry::Entity::EEvent::Update:
	{
		Update();
	}
	break;
	}
}

void CPlayerAttachmentsComponent::SpawnAndAttachEntity(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint, Vec3 rot)
{
	if (CPlayerAnimationComponent* pPlayerAnim = m_pEntity->GetComponent<CPlayerAnimationComponent>())
	{
		m_pCachedCharacter = pPlayerAnim->GetPlayerCharacter();
	}

	if (m_pCachedCharacter)
	{
		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

		int jointId = rIDefaultSkeleton.GetJointIDByName(joint.c_str());
		QuatT loc = QuatT(m_pEntity->GetWorldTM()) * pSkeleton->GetAbsJointByID(jointId);

		SEntitySpawnParams spawnParams;
		spawnParams.pClass = className.value.size() > 0 ? gEnv->pEntitySystem->GetClassRegistry()->FindClass(className.value) : nullptr;
		spawnParams.vPosition = loc.t;
		spawnParams.vScale = Vec3(1, 1, 1);

		if (rot == ZERO)
		{
			spawnParams.qRotation = loc.q;
		}
		else
		{
			spawnParams.qRotation = Quat::CreateRotationXYZ(Ang3(rot));
		}

		if (const IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			if (IAttachment* pAttachment = m_pCachedCharacter->GetIAttachmentManager()->GetInterfaceByName(joint.c_str()))
			{
				CEntityAttachment* pAttachObject = new CEntityAttachment();
				pAttachObject->SetEntityId(pEntity->GetId());
				pAttachment->AddBinding(pAttachObject);
				entityIdOut = Schematyc::ExplicitEntityId(pEntity->GetId());
			}
		}
		else
		{
			entityIdOut = Schematyc::ExplicitEntityId(INVALID_ENTITYID);
		}
	}
}

void CPlayerAttachmentsComponent::SpawnEntityAtJoint(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint)
{
	if (m_pCachedCharacter)
	{
		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

		int jointId = rIDefaultSkeleton.GetJointIDByName(joint.c_str());
		QuatT loc = QuatT(m_pEntity->GetWorldTM()) * pSkeleton->GetAbsJointByID(jointId);

		SEntitySpawnParams spawnParams;
		spawnParams.pClass = className.value.size() > 0 ? gEnv->pEntitySystem->GetClassRegistry()->FindClass(className.value) : nullptr;
		spawnParams.vPosition = loc.t;
		spawnParams.vScale = Vec3(1, 1, 1);
		spawnParams.qRotation = loc.q;

		if (const IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			entityIdOut = Schematyc::ExplicitEntityId(pEntity->GetId());
		}
		else
		{
			entityIdOut = Schematyc::ExplicitEntityId(INVALID_ENTITYID);
		}
	}
}

void CPlayerAttachmentsComponent::Update()
{
	if (m_pCachedCharacter)
	{
		m_pCachedCharacter->GetISkeletonAnim()->PushPoseModifier(3, m_poseHeadTopModifier, "Head Top Mod");

		ISkeletonPose* pSkeleton = m_pCachedCharacter->GetISkeletonPose();
		IDefaultSkeleton& rIDefaultSkeleton = m_pCachedCharacter->GetIDefaultSkeleton();

		int headTopId = rIDefaultSkeleton.GetJointIDByName("mixamorig7:HeadTop_End");
		QuatT loc = QuatT(m_pEntity->GetWorldTM()) * pSkeleton->GetAbsJointByID(headTopId);

		const CCamera& camera = gEnv->pSystem->GetViewCamera();

		loc.q = Quat::CreateRotationVDir(camera.GetViewdir());

		m_poseHeadTopModifier->PushOrientation(headTopId, IAnimationOperatorQueue::eOp_OverrideWorld, loc.q);
	}
	else
	{
		if (CPlayerAnimationComponent* pPlayerAnim = m_pEntity->GetComponent<CPlayerAnimationComponent>())
		{
			m_pCachedCharacter = pPlayerAnim->GetPlayerCharacter();
		}
	}
}
