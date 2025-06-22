#ifndef ADB_HPP
#define ADB_HPP

#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <FrameWork/FrameWork.hpp>

namespace FrameWork {
    class Adb {
    public:
        Adb(const std::string& path);
        void Kill();
        uint32_t Start(std::string processName, std::string moduleName);
    private:
        void ExecuteAdbCommand(const std::string& command);
        void KillProcessesByName(const std::wstring& processName);
        std::vector<std::string> split(const std::string& s, char delimiter);
        std::string _path;
    };
}
#endif