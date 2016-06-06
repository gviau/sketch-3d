#include "framework/AnimationState.h"

namespace Sketch3D
{

AnimationState::AnimationState()
    : m_DurationInTicks(0.0)
    , m_TicksPerSeconds(0.0)
{
}

AnimationState::AnimationState(double durationInTicks, double ticksPerSecond)
    : m_DurationInTicks(durationInTicks)
    , m_TicksPerSeconds(ticksPerSecond)
{
}

AnimationState::~AnimationState()
{
}

void AnimationState::SetPositionKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& positionKeys)
{
    m_PositionKeys[boneName] = positionKeys;
}

void AnimationState::SetRotationKeysForBone(const string& boneName, const vector<pair<double, Quaternion>>& rotationKeys)
{
    m_RotationKeys[boneName] = rotationKeys;
}

void AnimationState::SetScaleKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& scaleKeys)
{
    m_ScaleKeys[boneName] = scaleKeys;
}

bool AnimationState::HasAnimationKeys(const string& boneName) const
{
    VectorKey_t::const_iterator p_it = m_PositionKeys.find(boneName);
    if (p_it == m_PositionKeys.end())
    {
        return false;
    }

    QuaternionKey_t::const_iterator r_it = m_RotationKeys.find(boneName);
    if (r_it == m_RotationKeys.end())
    {
        return false;
    }

    VectorKey_t::const_iterator s_it = m_ScaleKeys.find(boneName);
    if (s_it == m_ScaleKeys.end())
    {
        return false;
    }

    return true;
}

size_t AnimationState::FindScalingKeyFrameIndex(const string& boneName, double time) const
{
    VectorKey_t::const_iterator it = m_ScaleKeys.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++)
    {
        if (time < it->second[i + 1].first)
        {
            return i;
        }
    }

    return 0;
}

size_t AnimationState::FindRotationKeyFrameIndex(const string& boneName, double time) const
{
    QuaternionKey_t::const_iterator it = m_RotationKeys.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++)
    {
        if (time < it->second[i + 1].first)
        {
            return i;
        }
    }

    return 0;
}

size_t AnimationState::FindTranslationKeyFrameIndex(const string& boneName, double time) const
{
    VectorKey_t::const_iterator it = m_PositionKeys.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++)
    {
        if (time < it->second[i + 1].first)
        {
            return i;
        }
    }

    return 0;
}

pair<double, Vector3> AnimationState::GetScalingValue(const string& boneName, size_t index) const
{
    if (m_ScaleKeys.at(boneName).size() == 1)
    {
        return m_ScaleKeys.at(boneName)[0];
    }

    return m_ScaleKeys.at(boneName)[index];
}

pair<double, Quaternion> AnimationState::GetRotationValue(const string& boneName, size_t index) const
{
    if (m_RotationKeys.at(boneName).size() == 1)
    {
        return m_RotationKeys.at(boneName)[0];
    }

    return m_RotationKeys.at(boneName)[index];
}

pair<double, Vector3> AnimationState::GetTranslationValue(const string& boneName, size_t index) const
{
    if (m_PositionKeys.at(boneName).size() == 1)
    {
        return m_PositionKeys.at(boneName)[0];
    }

    return m_PositionKeys.at(boneName)[index];
}

double AnimationState::GetDurationInTicks() const
{
    return m_DurationInTicks;
}

double AnimationState::GetTicksPerSeconds() const
{
    return m_TicksPerSeconds;
}

}