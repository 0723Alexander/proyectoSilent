#ifndef MEMORY_HPP
#define MEMORY_HPP

#pragma once
#include <Windows.h>
#include <unordered_map>

#include <string>
#include <vector>
#include <cstring>
#include <span> 

#include <Cheat/FreeFireSDK/Context.hpp>
#include <FrameWork/Utilities/Misc/Misc.hpp>
#include <FrameWork/FrameWork.hpp>
#include <Cheat/Offsets/Offsets.hpp>

namespace FrameWork {

    class Memory {
    private:
        static inline void* vmInstancePtr = nullptr;
        static int __cdecl InteropPhysRead(void* vmInstance, uintptr_t physAddress, void* buffer, size_t bufferSize);
    public:
        bool Interop();
        static bool Convert(uintptr_t address, uintptr_t& phys);

        template<typename T>
        static T Read(uintptr_t address) {
            T result{};
            uintptr_t physAddress;

            if (Convert(address, physAddress)) {
                g_FreeFireContext.VirtualMemory.physRead(vmInstancePtr, physAddress, &result, sizeof(T));
            }

            return result;
        }

        template<typename T>
        static bool Read1(uintptr_t address, T& data) {
            uintptr_t physAddress;
            if (!Convert(address, physAddress)) return false;

            int status = g_FreeFireContext.VirtualMemory.physRead(vmInstancePtr, physAddress, &data, sizeof(T));
            return (status == 0);
        }

        static bool Write(uintptr_t address, const void* value, size_t size) {
            uintptr_t physAddress;

            if (!Convert(address, physAddress)) {
                return false;
            }

            int status = g_FreeFireContext.VirtualMemory.physWrite(vmInstancePtr, physAddress, const_cast<void*>(value), size);
            return status == 0;
        }

        template <typename T>
        static bool WriteToMemory(uintptr_t address, const T& value) {
            size_t size = sizeof(T);
            return Write(address, &value, size);
        }

        template<typename T>
        static bool ReadArray(uintptr_t address, std::vector<T>& array)
        {
            uintptr_t convertedAddress;
            bool result = Convert(address, convertedAddress);

            if (!result)
                return false;

            size_t size = sizeof(T) * array.size();
            DWORD status = g_FreeFireContext.VirtualMemory.physRead(vmInstancePtr, convertedAddress, array.data(), size);

            return status == 0;
        }
        static std::u8string ReadUTF8String(uintptr_t address, int maxLength) {
            // Inicializa o buffer de leitura
            std::vector<uint8_t> buffer(maxLength, 0);

            // Tenta ler os bytes da memória
            if (!ReadArray(address, buffer)) {
                // Log de erro opcional para depuração
                // std::cerr << "Falha ao ler a memória no endereço: " << std::hex << address << std::endl;
                return u8"Erro"; // Retorna string indicando falha na leitura
            }

            // Limita a leitura até o primeiro caractere nulo
            auto nullTerminator = std::find(buffer.begin(), buffer.end(), '\0');
            if (nullTerminator == buffer.end()) {
                buffer.push_back('\0'); // Garante um terminador nulo
            }

            // Substitui caracteres inválidos (opcional)
            for (auto& byte : buffer) {
                if (byte > 127) { // Considera bytes não ASCII como inválidos
                    byte = '?';
                }
            }

            // Converte os bytes para uma std::u8string
            return std::u8string(buffer.begin(), nullTerminator);
        }


        static std::string ReadString(uintptr_t address, int size, bool unicode = true)
        {
            std::vector<uint8_t> stringBytes(size);

            bool read = ReadArray(address, stringBytes);

            if (!read) return "";

            std::string readString;
            if (unicode)
            {
                std::wstring wideString(reinterpret_cast<wchar_t*>(stringBytes.data()), size / 2);
                readString = std::string(wideString.begin(), wideString.end());
            }
            else
            {
                readString = std::string(stringBytes.begin(), stringBytes.end());
            }

            auto nullTerminator = readString.find('\0');
            if (nullTerminator != std::string::npos)
                readString = readString.substr(0, nullTerminator);

            return readString;
        }

    };
    inline Memory g_FreeFireMemory;
}

#endif
