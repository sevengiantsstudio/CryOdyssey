//Deer Solar Games | 2021 | Lighthouse project

#include "StdAfx.h"

#include <Core/Game.h>

#include <CryMath/Cry_GeoDistance.h>
#include <CryMovie/IMovieSystem.h>

#include <CryFlowGraph/IFlowBaseNode.h>


struct SMovieSamplingFlowNodeHelper
{
    struct SSampleTimeInfo
    {
        SSampleTimeInfo() : startDate(0.0f), duration(0.0f) {}
        inline float GetTimeAt(float ratio) const { return ratio * duration + startDate; }
        float startDate;
        float duration;
    };
    typedef std::vector<Vec3> TPointContainer;
    typedef std::vector<SSampleTimeInfo> TTimeInfoContainer;

    struct SSampledTrack
    {
        inline Lineseg GetSegment(size_t i) const { return Lineseg(pointContainer[i], pointContainer[i + 1]); }
        inline size_t GetSegmentCount() const { return timeInfoContainer.size(); }
        inline void Clear() { pointContainer.clear(); timeInfoContainer.clear(); }
        TPointContainer    pointContainer;
        TTimeInfoContainer timeInfoContainer;
    };

    static bool SampleTrack(SSampledTrack& outSampledTrack, IAnimTrack& animTrack)
    {
        bool bSuccess = false;
        outSampledTrack.Clear();

        int lastKeyIndex = animTrack.GetNumKeys() - 1;
        const float endTime = animTrack.GetKeyTime(lastKeyIndex);
        const float samplesPerSecond = 5.0f;
        const float timeStep = 1.0f / samplesPerSecond;

        const int sampleCount = static_cast<int>(endTime * samplesPerSecond);

        if (sampleCount > 0)
        {
            outSampledTrack.pointContainer.reserve(sampleCount + 1);
            outSampledTrack.timeInfoContainer.resize(sampleCount);
            for (int i = 0; i < sampleCount; ++i)
            {
                const float date = i * timeStep;
                TMovieSystemValue newSample = animTrack.GetValue(date);
                if (stl::holds_alternative<Vec3>(newSample))
                {
                    outSampledTrack.pointContainer.emplace_back(stl::get<Vec3>(newSample));
                    outSampledTrack.timeInfoContainer[i].startDate = date;
                    outSampledTrack.timeInfoContainer[i].duration = timeStep;
                }
            }

            // fix last segment as we're missing the last point of the track
            SSampleTimeInfo& lastSegmentTimeInfo = outSampledTrack.timeInfoContainer.back();
            const float lastSegmentLength = endTime - lastSegmentTimeInfo.startDate;
            const float localEpsilon = timeStep * 0.05f;
            if (lastSegmentLength > localEpsilon)
            {
                lastSegmentTimeInfo.duration = lastSegmentLength;
                TMovieSystemValue lastSample = animTrack.GetValue(endTime);
                if (stl::holds_alternative<Vec3>(lastSample))
                {
                    outSampledTrack.pointContainer.emplace_back(stl::get<Vec3>(lastSample));
                }
            }
            else
            {
                // segment too short to be worth considering.
                outSampledTrack.timeInfoContainer.pop_back();
            }

            bSuccess = true;
        }
        return bSuccess;
    }

    static size_t FindClosestSegmentIndex(float& outRatio, const SSampledTrack& sampledTrack, const Vec3& worldPos)
    {
        const size_t segmentCount = sampledTrack.GetSegmentCount();

        size_t closestSegmentIndex = 0;
        float closestSquareDistanceFound = FLT_MAX;
        float closestSegmentRatio = FLT_MAX;

        for (size_t i = 0; i < segmentCount; ++i)
        {
            const Lineseg nextSegment = sampledTrack.GetSegment(i);

            float newRatio = 0.0f;
            const float newDistanceSquared = Distance::Point_LinesegSq(worldPos, nextSegment, newRatio);
            if (newDistanceSquared < closestSquareDistanceFound)
            {
                closestSegmentIndex = i;
                closestSquareDistanceFound = newDistanceSquared;
                closestSegmentRatio = newRatio;
            }
        }

        outRatio = closestSegmentRatio;
        return closestSegmentIndex;
    }

    static float FindTimeFromSampledTrack(const SSampledTrack& sampledTrack, const Vec3& worldPos)
    {
        float segmentRatio = FLT_MAX;
        const size_t closestSegmentIndex = FindClosestSegmentIndex(segmentRatio, sampledTrack, worldPos);
        const SSampleTimeInfo& closestSegmentTimeInfo = sampledTrack.timeInfoContainer[closestSegmentIndex];
        const float result = closestSegmentTimeInfo.GetTimeAt(segmentRatio);
        return result;
    }

    static IAnimNode* FindAnimNode(const char* szSequenceName, const char* szNodeName)
    {
        IAnimNode* pResult = nullptr;
        if (IAnimSequence* pSequence = gEnv->pMovieSystem->FindSequence(szSequenceName))
        {
            pResult = pSequence->FindNodeByName(szNodeName, nullptr);
        }
        return pResult;
    }

    static IAnimTrack* FindAnimTrack(const char* szSequenceName, const char* szNodeName, EAnimParamType paramType, const char* szSubTrackName = nullptr)
    {
        IAnimTrack* pResult = nullptr;
        if (IAnimNode* pNode = FindAnimNode(szSequenceName, szNodeName))
        {
            IAnimTrack* pAnimTrack = pNode->GetTrackForParameter(paramType);
            if (pAnimTrack)
            {
                if (szSubTrackName)
                {
                    for (int i = 0, subTrackCount = pAnimTrack->GetSubTrackCount(); i < subTrackCount; ++i)
                    {
                        const char* szName = pAnimTrack->GetSubTrackName(i);
                        if (0 == stricmp(szName, szSubTrackName))
                        {
                            pResult = pAnimTrack->GetSubTrack(i);
                            break;
                        }
                    }
                }
                else
                {
                    pResult = pAnimTrack;
                }
            }
        }
        return pResult;
    }
};

class CMovieGetTimeFromPosition
	: public CFlowBaseNode<eNCT_Instanced>
	, SMovieSamplingFlowNodeHelper
{
	enum INPUTS
	{
		eIP_Get = 0,
		eIP_SequenceName,
		eIP_NodeEntity,
		eIP_NodeName,
	};

	enum OUTPUTS
	{
		eOP_Time = 0,
		eOP_Failed,
	};

public:
	CMovieGetTimeFromPosition(SActivationInfo* pActInfo)
	{
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo* pActInfo) override
	{
		return new CMovieGetTimeFromPosition(pActInfo);
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig_Void("Get",                            _HELP("Get the time at which the specified node is closes to the specified target's position.")),
			InputPortConfig<string>("Sequence",                       _HELP("Name of the sequence that should be inspected."), nullptr, _UICONFIG("dt=seq")),
			InputPortConfig<EntityId>("NodeEntity", INVALID_ENTITYID,   _HELP("Entity whose name identifies the node to search in.")),
			InputPortConfig<string>("NodeName",                       _HELP("Name of the node to search in (if no NodeEntity is provided).")),
			{0}
		};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig<float>("Time",   _HELP("Triggered with the found time if succeeded.")),
			OutputPortConfig_Void("Failed", _HELP("Triggered when failed to retrieve the time properly.")),
			{0}
		};
		config.sDescription = _HELP("Searches for a time for which a node is closest to a given position.");
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		switch (event)
		{
		case eFE_Initialize:
		{
			ClearCache();
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eIP_Get))
			{
				const string& sequenceName = GetPortString(pActInfo, eIP_SequenceName);
				const char* szNodeName = GetNodeName(pActInfo);

				if (CheckCache(sequenceName, szNodeName))
				{
					Vec3 worldPos = GetTargetPosition(pActInfo);
					float time = FindTimeFromSampledTrack(m_cachedSampledTrack, worldPos);
					ActivateOutput(pActInfo, eOP_Time, time);
				}
				else
				{
					ActivateOutput(pActInfo, eOP_Failed, true);
				}
			}
		}
		}
	}

private:
	const char* GetNodeName(SActivationInfo* pActInfo)
	{
		const char* szNodeName = GetPortString(pActInfo, eIP_NodeName).c_str();
		{
			EntityId entityId = GetPortEntityId(pActInfo, eIP_NodeEntity);
			if (entityId != INVALID_ENTITYID)
			{
				if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(entityId))
				{
					szNodeName = pEntity->GetName();
				}
			}
		}
		return szNodeName;
	}

	Vec3 GetTargetPosition(SActivationInfo* pActInfo)
	{
		if (const IEntity* pPlayerEntity = Game::CGame::GetInstance()->GetPlayerEntity())
		{
			return pPlayerEntity->GetWorldPos();
		}
		return Vec3(0);
	}

	static bool FindTimeFromTrackAndPos(float& outTime, IAnimTrack& animTrack, const Vec3& worldPos)
	{
		bool bSucceeded = false;
		SSampledTrack sampledTrack;
		if (SampleTrack(sampledTrack, animTrack))
		{
			outTime = FindTimeFromSampledTrack(sampledTrack, worldPos);
			bSucceeded = true;
		}
		return bSucceeded;
	}

	static bool FindTimeFromSquenceNodeAndPos(float& outTime, const string& sequenceName, const char* szNodeName, const Vec3& worldPos)
	{
		bool bSucceeded = false;
		if (IAnimTrack* pAnimTrack = FindAnimTrack(sequenceName, szNodeName, eAnimParamType_Position))
		{
			bSucceeded = FindTimeFromTrackAndPos(outTime, *pAnimTrack, worldPos);
		}
		return bSucceeded;
	}

	void ClearCache()
	{
		m_cachedSequenceName.clear();
		m_cachedNodeName.clear();
		m_cachedSampledTrack.Clear();
	}

	bool CheckCache(const string& sequenceName, const char* szNodeName)
	{
		bool bCacheOk = true;
		if ((m_cachedSequenceName != sequenceName) || (m_cachedNodeName != szNodeName))
		{
			if (IAnimTrack* pAnimTrack = FindAnimTrack(sequenceName.c_str(), szNodeName, eAnimParamType_Position))
			{
				SampleTrack(m_cachedSampledTrack, *pAnimTrack);
				m_cachedSequenceName = sequenceName;
				m_cachedNodeName = szNodeName;
			}
			else
			{
				bCacheOk = false;
			}
		}
		return bCacheOk;
	}

private:
	string m_cachedSequenceName;
	string m_cachedNodeName;
	SSampledTrack m_cachedSampledTrack;
};

REGISTER_FLOW_NODE("Animations:GetTimeFromPosition", CMovieGetTimeFromPosition);