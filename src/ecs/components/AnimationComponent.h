#pragma once

namespace blot { namespace components {

struct Animation {
    float duration = 1.0f;
    float currentTime = 0.0f;
    bool isPlaying = false;
    bool loop = false;
    // Animation curves
    enum Curve {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        Bounce,
        Elastic
    };
    Curve curve = Linear;
};

} } // namespace blot::components 