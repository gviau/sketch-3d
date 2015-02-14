#ifndef SKETCH_3D_ANIMATION_STATE_H
#define SKETCH_3D_ANIMATION_STATE_H

#include "math/Quaternion.h"
#include "math/Vector3.h"

#include "system/Platform.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

/**
 * @class AnimationState
 * This class represents a set of keyframes for an animation that will be used on a skeleton
 * to transform its bones
 */
class SKETCH_3D_API AnimationState {
    typedef map<string, vector<pair<double, Vector3>>> VectorKey_t;
    typedef map<string, vector<pair<double, Quaternion>>> QuaternionKey_t;

    public:
        /**
         * Constructor
         */
                                    AnimationState();

        /**
         * Constructor
         * @param durationInTicks Specifies the duration of the animation in ticks
         * @param ticksPerSecond Specifies how many ticks occur in a second
         */
                                    AnimationState(double durationInTicks, double ticksPerSecond);

        /**
         * Destructor
         */
                                   ~AnimationState();

        /**
         * Set the position keys for the specified bone
         * @param boneName The name of the bone for which to set the keys
         * @param positionKeys The keys to set for that bone
         */
        void                        SetPositionKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& positionKeys);

        /**
         * Set the rotation keys for the specified bone
         * @param boneName The name of the bone for which to set the keys
         * @param rotationKeys The keys to set for that bone
         */
        void                        SetRotationKeysForBone(const string& boneName, const vector<pair<double, Quaternion>>& rotationKeys);

        /**
         * Set the scale keys for the specified bone
         * @param boneName The name of the bone for which to set the keys
         * @param scaleKeys The keys to Set for that bone
         */
        void                        SetScaleKeysForBone(const string& boneName, const vector<pair<double, Vector3>>& scaleKeys);

        /**
         * Return true if the bone has at least one animation key (scaling, rotation, translation)
         * @param boneName The bone name to check for the keys
         */
        bool                        HasAnimationKeys(const string& boneName) const;

        /**
         * Return the index for the key frame that matches the current scaling value
         * @param boneName The name of the bone for which to find the key frame index
         * @param time The current time in seconds
         */
        size_t                      FindScalingKeyFrameIndex(const string& boneName, double time) const;

        /**
         * Return the index for the key frame that matches the current rotation value
         * @param boneName The name of the bone for which to find the key frame index
         * @param time The current time in seconds
         */
        size_t                      FindRotationKeyFrameIndex(const string& boneName, double time) const;

        /**
         * Return the index for the key frame that matches the current translation value
         * @param boneName The name of the bone for which to find the key frame index
         * @param time The current time in seconds
         */
        size_t                      FindTranslationKeyFrameIndex(const string& boneName, double time) const;

        /**
         * Get the specified scaling vector
         * @param boneName The name of the bone for which to get the value
         * @param index The key frame index at which we want the value
         */
        pair<double, Vector3>       GetScalingValue(const string& boneName, size_t index) const;

        /**
         * Get the specified rotation quaternion
         * @param boneName The name of the bone for which to get the value
         * @param index The key frame index at which we want the value
         */
        pair<double, Quaternion>    GetRotationValue(const string& boneName, size_t index) const;

        /**
         * Get the specified translation vector
         * @param boneName The name of the bone for which to get the value
         * @param index The key frame index at which we want the value
         */
        pair<double, Vector3>       GetTranslationValue(const string& boneName, size_t index) const;

        double                      GetDurationInTicks() const;
        double                      GetTicksPerSeconds() const;

    private:
        double                      durationInTicks_;   /**< The duration in ticks of the animation */  
        double                      ticksPerSeconds_;   /**< How many ticks occur per second */
        VectorKey_t                 positionKeys_;  /**< Position keys for this animation stored per bones */
        QuaternionKey_t             rotationKeys_;  /**< Rotation keys for this animation stored per bones */
        VectorKey_t                 scaleKeys_;     /**< Scale keys for this animation stored per bones */
};

}

#endif