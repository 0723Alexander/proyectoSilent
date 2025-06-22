#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <cstdint>
#include <FrameWork/Utilities/Memory/Memory.hpp>
#include <FrameWork/Math/Vectors/Vector3.hpp>
#include <FrameWork/Math/TMatrix.hpp>

class Transform {
public:

    static bool GetNodePosition(uint32_t nodeTransform, Vector3& result) {
        uint32_t transformValue = 0;
        bool transform = FrameWork::Memory::Read1(nodeTransform + 0x8, transformValue);
        if (!transform) {
            result = Vector3();
            return false;
        }
        return GetPosition(transformValue, result);
    }

    static bool GetPosition(uint32_t transform, Vector3& pos) {
        pos = Vector3();

        uint32_t transformObjValue = 0;
        bool transformObj = FrameWork::Memory::Read1(transform + 0x8, transformObjValue);
        if (!transformObj) {
            return false;
        }

        uint32_t indexValue = 0;
        bool index = FrameWork::Memory::Read1(transformObjValue + 0x24, indexValue);
        uint32_t matrixValue = 0;
        bool matrix = FrameWork::Memory::Read1(transformObjValue + 0x20, matrixValue);
        if (!index || !matrix) {
            return false;
        }

        uint32_t matrixListValue = 0;
        bool matrixList = FrameWork::Memory::Read1(matrixValue + 0x10, matrixListValue);
        uint32_t matrixIndicesValue = 0;
        bool matrixIndices = FrameWork::Memory::Read1(matrixValue + 0x14, matrixIndicesValue);
        if (!matrixList || !matrixIndices) {
            return false;
        }

        Vector3 resultValue;
        bool result = FrameWork::Memory::Read1(indexValue * 0x30 + matrixListValue, resultValue);
        if (!result) {
            return false;
        }

        int maxTries = 50;
        int tries = 0;

        int transformIndexValue = 0;
        bool transformIndex = FrameWork::Memory::Read1((uint32_t)((indexValue * 0x4) + matrixIndicesValue), transformIndexValue);
        if (!transformIndex) {
            return false;
        }

        while (transformIndexValue >= 0) {
            tries++;

            if (tries == maxTries) break;

            TMatrix tMatrixValue;
            bool tMatrix = FrameWork::Memory::Read1((uint32_t)(0x30 * transformIndexValue + matrixListValue), tMatrixValue);
            if (!tMatrix) {
                return false;
            }

            float rotX = tMatrixValue.rotation.X;
            float rotY = tMatrixValue.rotation.Y;
            float rotZ = tMatrixValue.rotation.Z;
            float rotW = tMatrixValue.rotation.W;

            float scaleX = resultValue.X * tMatrixValue.scale.x;
            float scaleY = resultValue.Y * tMatrixValue.scale.y;
            float scaleZ = resultValue.Z * tMatrixValue.scale.z;

            resultValue.X = tMatrixValue.position.x + scaleX +
                (scaleX * ((rotY * rotY * -2.0f) - (rotZ * rotZ * 2.0f))) +
                (scaleY * ((rotW * rotZ * -2.0f) - (rotY * rotX * -2.0f))) +
                (scaleZ * ((rotZ * rotX * 2.0f) - (rotW * rotY * -2.0f)));

            resultValue.Y = tMatrixValue.position.y + scaleY +
                (scaleX * ((rotX * rotY * 2.0f) - (rotW * rotZ * -2.0f))) +
                (scaleY * ((rotZ * rotZ * -2.0f) - (rotX * rotX * 2.0f))) +
                (scaleZ * ((rotW * rotX * -2.0f) - (rotZ * rotY * -2.0f)));

            resultValue.Z = tMatrixValue.position.z + scaleZ +
                (scaleX * ((rotW * rotY * -2.0f) - (rotX * rotZ * -2.0f))) +
                (scaleY * ((rotY * rotZ * 2.0f) - (rotW * rotX * -2.0f))) +
                (scaleZ * ((rotX * rotX * -2.0f) - (rotY * rotY * 2.0f)));

            bool transformResult = FrameWork::Memory::Read1((uint32_t)(transformIndexValue * 0x4 + matrixIndicesValue), transformIndexValue);
            if (!transformResult) {
                return false;
            }
        }

        pos = resultValue;

        return tries != maxTries;
    }

};

#endif // TRANSFORM_HPP