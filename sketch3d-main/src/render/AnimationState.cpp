#include "render/AnimationState.h"

namespace Sketch3D {

AnimationState::AnimationState() : durationInTicks_(0.0), ticksPerSeconds_(0.0) {
}

AnimationState::AnimationState(double durationInTicks, double ticksPerSecond) : durationInTicks_(durationInTicks), ticksPerSeconds_(ticksPerSecond) {
}

AnimationState::~AnimationState() {
}

void AnimationState::SetPositionKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& positionKeys) {
    positionKeys_[boneName] = positionKeys;
}

void AnimationState::SetRotationKeysForBone(const string& boneName, const vector<pair<double, Quaternion>>& rotationKeys) {
    rotationKeys_[boneName] = rotationKeys;
}

void AnimationState::SetScaleKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& scaleKeys) {
    scaleKeys_[boneName] = scaleKeys;
}

bool AnimationState::HasAnimationKeys(const string& boneName) const {
    VectorKey_t::const_iterator p_it = positionKeys_.find(boneName);
    if (p_it == positionKeys_.end()) {
        return false;
    }

    QuaternionKey_t::const_iterator r_it = rotationKeys_.find(boneName);
    if (r_it == rotationKeys_.end()) {
        return false;
    }

    VectorKey_t::const_iterator s_it = scaleKeys_.find(boneName);
    if (s_it == scaleKeys_.end()) {
        return false;
    }

    return true;
}

size_t AnimationState::FindScalingKeyFrameIndex(const string& boneName, double time) const {
    VectorKey_t::const_iterator it = scaleKeys_.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++) {
        if (time < it->second[i + 1].first) {
            return i;
        }
    }

    return 0;
}

size_t AnimationState::FindRotationKeyFrameIndex(const string& boneName, double time) const {
    QuaternionKey_t::const_iterator it = rotationKeys_.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++) {
        if (time < it->second[i + 1].first) {
            return i;
        }
    }

    return 0;
}

size_t AnimationState::FindTranslationKeyFrameIndex(const string& boneName, double time) const {
    VectorKey_t::const_iterator it = positionKeys_.find(boneName);

    // We don't check the last one since this will be used for interpolation and we need a second
    // value
    for (size_t i = 0; i < it->second.size() - 1; i++) {
        if (time < it->second[i + 1].first) {
            return i;
        }
    }

    return 0;
}

pair<double, Vector3> AnimationState::GetScalingValue(const string& boneName, size_t index) const {
    if (scaleKeys_.at(boneName).size() == 1) {
        return scaleKeys_.at(boneName)[0];
    }

    return scaleKeys_.at(boneName)[index];
}

pair<double, Quaternion> AnimationState::GetRotationValue(const string& boneName, size_t index) const {
    if (rotationKeys_.at(boneName).size() == 1) {
        return rotationKeys_.at(boneName)[0];
    }

    return rotationKeys_.at(boneName)[index];
}

pair<double, Vector3> AnimationState::GetTranslationValue(const string& boneName, size_t index) const {
    if (positionKeys_.at(boneName).size() == 1) {
        return positionKeys_.at(boneName)[0];
    }

    return positionKeys_.at(boneName)[index];
}

double AnimationState::GetDurationInTicks() const {
    return durationInTicks_;
}

double AnimationState::GetTicksPerSeconds() const {
    return ticksPerSeconds_;
}

}