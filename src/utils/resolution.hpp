#ifndef RESOLUTION_HPP
#define RESOLUTION_HPP

#include <sys/types.h>
enum class Resolution {
    QHD,
    FHD,
    HD,
};

enum Aspect { R16_9, R16_10, R4_3 };

// returns screen width
inline uint getResolution(Resolution res) {
    switch (res) {
    case Resolution::QHD: {
        return 2560;
    };

    case Resolution::FHD: {
        return 1920;
    };

    case Resolution::HD: {
        return 1280;
    };
    }

    return 1440;
}

inline float getAspectRatio(Aspect aspect) {
    switch (aspect) {
    case Aspect::R16_9: {
        return 16.0 / 9.0;
    };

    case Aspect::R16_10: {
        return 16.0 / 10.0;
    };

    case Aspect::R4_3: {
        return 4.0 / 3.0;
    };
    }

    return 16.0 / 9.0;
}

#endif // !RESOLUTION_HPP
