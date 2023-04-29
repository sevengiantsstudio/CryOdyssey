#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/Utils/SharedString.h>

class CPlayerAttachmentsComponent final : public IEntityComponent
{
public:

	CPlayerAttachmentsComponent() = default;
	virtual ~CPlayerAttachmentsComponent() = default;

	//IEntityComponent
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	//~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CPlayerAttachmentsComponent>& desc)
	{
		desc.SetLabel("PlayerAttachments");
		desc.SetEditorCategory("Player");
		desc.SetDescription("Player attachments component.");
		desc.SetGUID("{ACCEB6C1-6C1B-41BA-A643-DE136FAAE63B}"_cry_guid);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::ClientOnly, IEntityComponent::EFlags::HideFromInspector });
	}

	void SpawnAndAttachEntity(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint, Vec3 rot);
	void SpawnEntityAtJoint(Schematyc::ExplicitEntityId& entityIdOut, Schematyc::EntityClassName className, Schematyc::CSharedString joint);

private:
	void Update();

private:
	ICharacterInstance* m_pCachedCharacter = nullptr;

	IAnimationOperatorQueuePtr m_poseHeadTopModifier;
};