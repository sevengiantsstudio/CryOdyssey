#include "StdAfx.h"
#include "PlayerAnimationComponent.h"

#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>

#include <CryGame/IGameFramework.h>

#include <CryPhysics\physinterface.h>
#include <CryPhysics/IPhysics.h>

#include <Animation/PoseAligner/PoseAligner.h>

static void RegisterPlayerAnimationControllerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerAnimationComponent));
		// Functions
		{
		}
		// Signals
		{
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerAnimationControllerComponent)


CPlayerAnimationComponent::~CPlayerAnimationComponent()
{
	SAFE_RELEASE(m_pActionController);
}

void CPlayerAnimationComponent::Initialize()
{
	m_pFirstPersonCachedCharacter = gEnv->pCharacterManager->CreateInstance("objects/characters/adam/adam_fp.cdf");
	m_pThirdPersonCachedCharacter = gEnv->pCharacterManager->CreateInstance("objects/characters/adam/adam_tp.cdf");

	if (m_pFirstPersonCachedCharacter)
	{
		if (m_pPoseAlignerFp == nullptr)
		{
			CryCreateClassInstance(CPoseAlignerC3::GetCID(), m_pPoseAlignerFp);
		}
		m_pPoseAlignerFp->Clear();
	}
	else
	{
		m_pPoseAlignerFp.reset();
	}

	if (m_pThirdPersonCachedCharacter)
	{
		if (m_pPoseAlignerTp == nullptr)
		{
			CryCreateClassInstance(CPoseAlignerC3::GetCID(), m_pPoseAlignerTp);
		}
		m_pPoseAlignerTp->Clear();
	}
	else
	{
		m_pPoseAlignerTp.reset();
	}


	IMannequin& mannequinInterface = gEnv->pGameFramework->GetMannequinInterface();
	IAnimationDatabaseManager& animationDatabaseManager = mannequinInterface.GetAnimationDatabaseManager();
	m_pControllerDefinition = animationDatabaseManager.LoadControllerDef("Animations/Mannequin/ADB/Adam/AdamControllerDefinition.xml");
	m_animDatabase = animationDatabaseManager.Load("Animations/Mannequin/ADB/Adam/Adam.adb");

	m_pAnimationContext.reset();

	m_pEntity->SetCharacter(m_pFirstPersonCachedCharacter, 1, false);
	m_pEntity->SetCharacter(m_pThirdPersonCachedCharacter, 5, false);

	m_pAnimationContext = stl::make_unique<SAnimationContext>(*m_pControllerDefinition);
	m_pActionController = mannequinInterface.CreateActionController(GetEntity(), *m_pAnimationContext);


	SetAnimationDrivenMotion(true);

	const TagID scopeFirstPersonContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("FirstPersonCharacter");
	const TagID scopeThirdPersonContextId = m_pAnimationContext->controllerDef.m_scopeContexts.Find("ThirdPersonCharacter");


	m_pActionController->SetScopeContext(scopeFirstPersonContextId, *m_pEntity, m_pFirstPersonCachedCharacter, m_animDatabase);
	m_pActionController->SetScopeContext(scopeThirdPersonContextId, *m_pEntity, m_pThirdPersonCachedCharacter, m_animDatabase);

	if (m_pActiveAction)
	{
		m_pActiveAction->Stop();
	}

	m_idleFragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find("Idle");
	m_walkFragmentId = m_pAnimationContext->controllerDef.m_fragmentIDs.Find("Walk");
	m_pActiveAction = new TAction<SAnimationContext>(0, m_idleFragmentId);
	m_pActionController->Queue(*m_pActiveAction);
	
	
	m_pEntity->UpdateComponentEventMask(this);

}

EntityEventMask CPlayerAnimationComponent::GetEventMask() const
{
	return
		Cry::Entity::EEvent::Initialize | Cry::Entity::EEvent::Update |
		Cry::Entity::EEvent::AnimationEvent;
}

IEntityComponent::ComponentEventPriority CPlayerAnimationComponent::GetEventPriority() const
{
	return 15;
}

void CPlayerAnimationComponent::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case Cry::Entity::EEvent::Initialize:
	{
		CharacterSetup();
	}
	break;

	case Cry::Entity::EEvent::AnimationEvent:
	{
	}
	break;

	case Cry::Entity::EEvent::Update:
	{

		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		if (m_pFirstPersonCachedCharacter != nullptr && m_pThirdPersonCachedCharacter != nullptr)
		{
			m_pActionController->Update(pCtx->fFrameTime);
		}

		Matrix34 characterTransform = GetWorldTransformMatrix();

		m_turnAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), m_prevForwardDir) / pCtx->fFrameTime;
		m_prevForwardDir = characterTransform.GetColumn1();

		if (m_pFirstPersonCachedCharacter != nullptr && m_pThirdPersonCachedCharacter != nullptr)
		{
			if (IPhysicalEntity* pPhysicalEntity = m_pEntity->GetPhysicalEntity())
			{
				pe_status_dynamics dynStatus;
				if (pPhysicalEntity->GetStatus(&dynStatus))
				{
					float travelAngle = Ang3::CreateRadZ(characterTransform.GetColumn1(), dynStatus.v.GetNormalized());
					float travelSpeed = dynStatus.v.GetLength2D();

					// Set the travel speed based on the physics velocity magnitude
					// Keep in mind that the maximum number for motion parameters is 10.
					// If your velocity can reach a magnitude higher than this, divide by the maximum theoretical account and work with a 0 - 1 ratio.
					if (!m_overriddenMotionParams.test(eMotionParamID_TravelSpeed))
					{
						m_pFirstPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelSpeed, travelSpeed, 0.f);
						m_pThirdPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelSpeed, travelSpeed, 0.f);
					}

					// Update the turn speed in CryAnimation, note that the maximum motion parameter (10) applies here too.
					if (!m_overriddenMotionParams.test(eMotionParamID_TurnAngle))
					{
						m_pFirstPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TurnAngle, m_turnAngle, 0.f);
						m_pThirdPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TurnAngle, m_turnAngle, 0.f);
					}

					if (!m_overriddenMotionParams.test(eMotionParamID_TravelAngle))
					{
						m_pFirstPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelAngle, travelAngle, 0.f);
						m_pThirdPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(eMotionParamID_TravelAngle, travelAngle, 0.f);
					}
				}
			}

			if (m_pPoseAlignerFp != nullptr && m_pPoseAlignerFp->Initialize(*m_pEntity, m_pFirstPersonCachedCharacter))
			{
				m_pPoseAlignerFp->SetBlendWeight(1.f);
				m_pPoseAlignerFp->Update(m_pFirstPersonCachedCharacter, QuatT(characterTransform), pCtx->fFrameTime);
			}

			if (m_pPoseAlignerTp != nullptr && m_pPoseAlignerTp->Initialize(*m_pEntity, m_pThirdPersonCachedCharacter))
			{
				m_pPoseAlignerTp->SetBlendWeight(1.f);
				m_pPoseAlignerTp->Update(m_pThirdPersonCachedCharacter, QuatT(characterTransform), pCtx->fFrameTime);
			}
		}

		m_overriddenMotionParams.reset();


		const FragmentID& desiredFragmentId = m_bPlayerIsMovement ? m_walkFragmentId : m_idleFragmentId;
		if (m_activeFragmentId != desiredFragmentId)
		{
			m_activeFragmentId = desiredFragmentId;
			QueueFragmentWithId(m_activeFragmentId);

			if (m_activeFragmentId == m_idleFragmentId)
			{
				gEnv->pConsole->ExecuteString("a_poseAlignerForceWeightOne 1");
			}
			else
			{
				gEnv->pConsole->ExecuteString("a_poseAlignerForceWeightOne 0");
			}
		}

	}
	break;
	}
}

const QuatT CPlayerAnimationComponent::GetHeadBoneLocation()
{
	QuatT headLoc;
	if (m_pFirstPersonCachedCharacter != nullptr)
	{
		int cameraJointId = m_pFirstPersonCachedCharacter->GetIDefaultSkeleton().GetJointIDByName("mixamorig7:HeadTop_End");		

		headLoc = m_pFirstPersonCachedCharacter->GetISkeletonPose()->GetAbsJointByID(cameraJointId);

	}
	return headLoc;
}

void CPlayerAnimationComponent::SetAnimationDrivenMotion(bool bSet)
{
	m_bAnimationDrivenMotion = bSet;

	if (m_pFirstPersonCachedCharacter && m_pThirdPersonCachedCharacter)
	{
		m_pFirstPersonCachedCharacter->GetISkeletonAnim()->SetAnimationDrivenMotion(m_bAnimationDrivenMotion);
		m_pThirdPersonCachedCharacter->GetISkeletonAnim()->SetAnimationDrivenMotion(m_bAnimationDrivenMotion);
	}
}

void CPlayerAnimationComponent::SetMotionParameter(EMotionParamID motionParam, float value)
{
	CRY_ASSERT(m_pFirstPersonCachedCharacter != nullptr);
	CRY_ASSERT(m_pThirdPersonCachedCharacter != nullptr);

	if (m_pFirstPersonCachedCharacter != nullptr && m_pThirdPersonCachedCharacter != nullptr)
	{
		m_pFirstPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(motionParam, value, 0.f);
		m_pThirdPersonCachedCharacter->GetISkeletonAnim()->SetDesiredMotionParam(motionParam, value, 0.f);
		m_overriddenMotionParams.set(motionParam);
	}
}

void CPlayerAnimationComponent::QueueFragmentWithId(const FragmentID& fragmentId)
{
	if (m_pAnimationContext == nullptr)
	{
		return;
	}

	if (m_pActiveAction)
	{
		m_pActiveAction->Stop();
	}

	const int priority = 0;
	m_pActiveAction = new TAction<SAnimationContext>(priority, fragmentId);
	m_pActionController->Queue(*m_pActiveAction);
}

void CPlayerAnimationComponent::CharacterSetup()
{
	uint32 flags = m_pEntity->GetSlotFlags(1);
	//flags |= ENTITY_SLOT_RENDER_NEAREST;
	flags &= ~ENTITY_SLOT_CAST_SHADOW;
	m_pEntity->SetSlotFlags(1, flags);


	if (ICharacterInstance* mainChar = m_pFirstPersonCachedCharacter)
	{
		IAttachmentManager* attachmentMan = mainChar->GetIAttachmentManager();
		IAttachmentManager* attachmentManShadow = NULL;

		if (ICharacterInstance* shadowChar = m_pEntity->GetCharacter(5))
		{
			attachmentManShadow = shadowChar->GetIAttachmentManager();

			if (attachmentManShadow)
			{
				for (int32 i = 0; i < attachmentManShadow->GetAttachmentCount(); i++)
				{
					attachmentManShadow->GetInterfaceByIndex(i)->HideAttachment(1);
				}
			}

			HideAllAttachmentsInShadowAndRecursion(attachmentMan, true, shadowChar != nullptr);
			HideAllAttachmentsInShadowAndRecursion(attachmentManShadow, false, false);

			SetPlayerEntityShadow(m_pEntity, false, 1, 5);
		}
	}
}

void CPlayerAnimationComponent::SetPlayerEntityShadow(IEntity* pEntity, bool isThirdPerson, int characterSlot, int shadowCharacterSlot)
{
	pEntity->SetFlags(pEntity->GetFlags() & ~ENTITY_FLAG_CASTSHADOW);
	if (isThirdPerson)
	{
		pEntity->SetSlotFlags(characterSlot, pEntity->GetSlotFlags(characterSlot) | ENTITY_SLOT_CAST_SHADOW);
		pEntity->SetSlotFlags(shadowCharacterSlot, pEntity->GetSlotFlags(shadowCharacterSlot) & ~ENTITY_SLOT_CAST_SHADOW);
	}
	else
	{
		pEntity->SetSlotFlags(characterSlot, pEntity->GetSlotFlags(characterSlot) & ~ENTITY_SLOT_CAST_SHADOW);
		pEntity->SetSlotFlags(shadowCharacterSlot, pEntity->GetSlotFlags(shadowCharacterSlot) | ENTITY_SLOT_CAST_SHADOW);
	}
}

void CPlayerAnimationComponent::HideAllAttachmentsInShadowAndRecursion(IAttachmentManager* attachmentManager, bool hideInShadow, bool hideInRecursion)
{
	if (attachmentManager)
	{
		for (int32 i = 0; i < attachmentManager->GetAttachmentCount(); i++)
		{
			IAttachment* attachment = attachmentManager->GetInterfaceByIndex(i);
			attachment->HideInShadow(hideInShadow);
			attachment->HideInRecursion(hideInRecursion);
		}
	}
}
