#pragma once

#include <iostream>
#include <FrameWork/Math/Quaternion.hpp>
#include <FrameWork/FrameWork.hpp>
#include <FrameWork/Math/Vectors/Vector4.hpp>

struct TMatrix {
    Vector4 position;
    Quaternion rotation;
    Vector4 scale;

    // Sobrecarga do operador << para exibição
    friend std::ostream& operator<<(std::ostream& os, const TMatrix& matrix) {
        os << XorStr("TMatrix {\n")
            << XorStr("  Position: ") << matrix.position << ",\n"
            << XorStr("  Rotation: ") << matrix.rotation << ",\n"
            << XorStr("  Scale: ") << matrix.scale << "\n"
            << XorStr("}");
        return os;
    }
};
