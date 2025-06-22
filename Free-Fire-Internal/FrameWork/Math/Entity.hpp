#pragma once
#include <string>
#include <FrameWork/Math/Vectors/Vector3.hpp>

enum class Bool3 {
    True,
    False,
    Unknown
};

class Entity {
public:
    bool IsKnown;
    Bool3 IsTeam;
    bool IsDead;
    bool IsKnocked;
    short Health;
    float Distance;
    bool IsVisible;
    Vector3 Head;
    Vector3 Helm;
    Vector3 Root;
    Vector3 Neck;
    Vector3 OmbroRight;
    Vector3 OmbroLeft;
    Vector3 Quadril;
    Vector3 Pinto;
    Vector3 RightCotovelo;
    Vector3 LeftCotovelo;
    Vector3 RightPunho;
    Vector3 LeftPunho;
    Vector3 RightTornozelo;
    Vector3 LeftTornozelo;
    std::string Name;
};
