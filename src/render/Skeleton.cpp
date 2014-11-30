#include "render/Skeleton.h"

#include "system/Logger.h"

namespace Sketch3D {

Skeleton::Skeleton() : root_(nullptr) {
}

Skeleton::~Skeleton() {
}

bool Skeleton::GetTransformationMatrices(double time, const AnimationState* animationState, bool isLooping,
                                         map<const Bone_t*, Matrix4x4>& transformationMatrices) const
{
    if (animationState == nullptr) {
        Logger::GetInstance()->Warning("Trying to play an animation but none is set");
        return false;
    }

    double ticksPerSeconds = animationState->GetTicksPerSeconds();
    if (ticksPerSeconds == 0.0) {
        ticksPerSeconds = 25.0;
    }

    double animationTime = ticksPerSeconds * time;
    if (isLooping) {
        animationTime = fmod(animationTime, animationState->GetDurationInTicks());
    } else if (animationTime > animationState->GetDurationInTicks()) {
        return false;
    }

    Matrix4x4 identity;
    CalculateBoneTransformation(animationTime, animationState, root_, identity, transformationMatrices);

    return true;
}

void Skeleton::AddAnimationState(const string& name, const AnimationState& animationState) {
    string animationName = name;
    if (animationName == "") {
        animationName = "Default";
    }
    animationStates_[animationName] = animationState;
}

const AnimationState* Skeleton::GetAnimationState(const string& name) const {
    map<string, AnimationState>::const_iterator it = animationStates_.find(name);
    if (it != animationStates_.end()) {
        return &(it->second);
    }

    return nullptr;
}

Bone_t* Skeleton::CreateBone(const string& name, const Matrix4x4& offsetMatrix) {
    bones_[name] = Bone_t(offsetMatrix);
    bones_[name].name = name;
    if (root_ == nullptr) {
        root_ = &bones_[name];
    }

    return &bones_[name];
}

Bone_t* Skeleton::FindBoneByName(const string& name) {
    hash_map<string, Bone_t>::iterator it = bones_.find(name);
    if (it != bones_.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Skeleton::SetGlobalInverseTransform(const Matrix4x4& globalInverseTransform) {
    globalInverseTransform_ = globalInverseTransform;
}

void Skeleton::CalculateBoneTransformation(double time, const AnimationState* animationState, const Bone_t* bone,
                                           const Matrix4x4& parentTransformation,
                                           map<const Bone_t*, Matrix4x4>& transformationMatrices) const
{
    Matrix4x4 boneTransformation;

    // First verify that this bone has any animation keys at all, maybe it is just
    // a bone in the hierarchy without any vertices attached to it
    if (animationState->HasAnimationKeys(bone->name)) {
        Vector3 scaling = CalculateInterpolatedScaling(time, animationState, bone);
        Quaternion rotation = CalculateInterpolatedRotation(time, animationState, bone);
        Vector3 translation = CalculateInterpolatedTranslation(time, animationState, bone);

        Matrix4x4 scalingMatrix, rotationMatrix, translationMatrix;
        scalingMatrix.Scale(scaling);
        rotation.ToRotationMatrix(rotationMatrix);
        translationMatrix.Translate(translation);

        boneTransformation = translationMatrix * rotationMatrix * scalingMatrix;
    }

    Matrix4x4 globalTransformation = parentTransformation * boneTransformation;
    transformationMatrices[bone] = globalTransformation * bone->offsetMatrix;

    for (size_t i = 0; i < bone->linkedBones.size(); i++) {
        CalculateBoneTransformation(time, animationState, bone->linkedBones[i], globalTransformation,
                                    transformationMatrices);
    }
}

Vector3 Skeleton::CalculateInterpolatedScaling(double time, const AnimationState* animationState, const Bone_t* bone) const {
    size_t scalingIndex = animationState->FindScalingKeyFrameIndex(bone->name, time);
    size_t nextScalingIndex = scalingIndex + 1;

    pair<double, Vector3> scaling = animationState->GetScalingValue(bone->name, scalingIndex);
    pair<double, Vector3> nextScaling = animationState->GetScalingValue(bone->name, nextScalingIndex);

    double deltaTime = nextScaling.first - scaling.first;
    double factor = 0.0;

    if (deltaTime < EPSILON) {
        factor = 0.0;
    } else {
        factor = (time - scaling.first) / deltaTime;
    }

    Vector3 deltaValue = nextScaling.second - scaling.second;
    return scaling.second + (float)factor * deltaValue;
}

Quaternion Skeleton::CalculateInterpolatedRotation(double time, const AnimationState* animationState, const Bone_t* bone) const {
    size_t rotationIndex = animationState->FindRotationKeyFrameIndex(bone->name, time);
    size_t nextRotationIndex = rotationIndex + 1;

    pair<double, Quaternion> rotation = animationState->GetRotationValue(bone->name, rotationIndex);
    pair<double, Quaternion> nextRotation = animationState->GetRotationValue(bone->name, nextRotationIndex);

    double deltaTime = nextRotation.first - rotation.first;
    double factor = 0.0;

    if (deltaTime < EPSILON) {
        factor = 0.0;
    } else {
        factor = (time - rotation.first) / deltaTime;
    }

    Quaternion interpolatedRotation = rotation.second.Slerp(nextRotation.second, (float)factor);
    interpolatedRotation.Normalize();
    return interpolatedRotation;
}

Vector3 Skeleton::CalculateInterpolatedTranslation(double time, const AnimationState* animationState, const Bone_t* bone) const {
    size_t translationIndex = animationState->FindTranslationKeyFrameIndex(bone->name, time);
    size_t nextTranslationIndex = translationIndex + 1;

    pair<double, Vector3> translation = animationState->GetTranslationValue(bone->name, translationIndex);
    pair<double, Vector3> nextTranslation = animationState->GetTranslationValue(bone->name, nextTranslationIndex);

    double deltaTime = nextTranslation.first - translation.first;
    double factor = 0.0;

    if (deltaTime < EPSILON) {
        factor = 0.0;
    } else {
        factor = (time - translation.first) / deltaTime;
    }

    Vector3 deltaValue = nextTranslation.second - translation.second;
    return translation.second + (float)factor * deltaValue;
}

}