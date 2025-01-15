#include <iostream>
#include <fstream>
#include <memory>
// #include <coroutine>

#ifdef WIN32

#else
#include <sys/prctl.h>
#include <sys/resource.h>
#endif

#ifdef _WIN32
#pragma comment(lib, "Iphlpapi.lib")
// 解决在宇宙编译器(Visual Studio)下 libevent evutil.c找不到符号的问题
#endif

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/hourly_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#if defined(__GNUC__)
// GCC 编译器相关的代码
#elif defined(_MSC_VER)
// Visual Studio 编译器相关的代码
// #define YAML_CPP_STATIC_DEFINE
#include <direct.h> // 包含 _chdir 函数的头文件
#elif defined(__clang__)
// Clang 编译器相关的代码
#else
// 其他编译器的代码
#endif

#include "yaml-cpp/yaml.h"

#include "version.h"
#include "ntrip_caster.h"

#define CONF_PATH "conf/"

// trace：最详细的日志级别，提供追踪程序执行流程的信息。
// debug：调试级别的日志信息，用于调试程序逻辑和查找问题。
// info：通知级别的日志信息，提供程序运行时的一般信息。
// warn：警告级别的日志信息，表明可能发生错误或不符合预期的情况。
// error：错误级别的日志信息，表明发生了某些错误或异常情况。
// critical：严重错误级别的日志信息，表示一个致命的或不可恢复的错误。

json load_Caster_Conf(const char *conf_directory)
{
    json conf;

    std::string Path = conf_directory;
    YAML::Node Conf = YAML::LoadFile(Path + "Service_Setting.yml");
    // std::cout << Conf << std::endl;

    // 配置转json
    auto Ntrip_Listener_Setting = Conf["Ntrip_Listener_Setting"];
    conf["Ntrip_Listener"]["Listen_Port"] = Ntrip_Listener_Setting["Listen_Port"].as<int>();
    conf["Ntrip_Listener"]["Connect_Timeout"] = Ntrip_Listener_Setting["Connect_Timeout"].as<int>();
    conf["Ntrip_Listener"]["Enable_Source_Login"] = Ntrip_Listener_Setting["Enable_Source_Login"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Server_Login"] = Ntrip_Listener_Setting["Enable_Server_Login"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Client_Login"] = Ntrip_Listener_Setting["Enable_Client_Login"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Nearest_MPT"] = Ntrip_Listener_Setting["Enable_Nearest_MPT"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Virtual_MPT"] = Ntrip_Listener_Setting["Enable_Virtual_MPT"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Common_MPT"] = Ntrip_Listener_Setting["Enable_Common_MPT"].as<bool>();
    conf["Ntrip_Listener"]["Enable_Header_No_CRLF"] = Ntrip_Listener_Setting["Enable_Header_No_CRLF"].as<bool>();

    auto Server_Setting = Conf["Server_Setting"];
    conf["Server_Setting"]["Connect_Timeout"] = Server_Setting["Connect_Timeout"].as<int>();
    conf["Server_Setting"]["Unsend_Byte_Limit"] = Server_Setting["Unsend_Byte_Limit"].as<int>();
    conf["Server_Setting"]["Heart_Beat_Interval"] = Server_Setting["Heart_Beat_Interval"].as<int>();
    conf["Server_Setting"]["Heart_Beat_Msg"] = Server_Setting["Heart_Beat_Msg"].as<std::string>();

    auto Client_Setting = Conf["Client_Setting"];
    conf["Client_Setting"]["Connect_Timeout"] = Client_Setting["Connect_Timeout"].as<int>();
    conf["Client_Setting"]["Unsend_Byte_Limit"] = Client_Setting["Unsend_Byte_Limit"].as<int>();

    auto Common_Setting = Conf["Common_Setting"];
    conf["Common_Setting"]["Refresh_State_Interval"] = Common_Setting["Refresh_State_Interval"].as<int>();
    conf["Common_Setting"]["Output_State"] = Common_Setting["Output_State"].as<bool>();

    auto Log_Setting = Conf["Log_Setting"];
    conf["Log_Setting"]["Output_STD"] = Log_Setting["Output_STD"].as<bool>();
    conf["Log_Setting"]["Output_File"] = Log_Setting["Output_File"].as<bool>();
    conf["Log_Setting"]["Output_File_Daily"] = Log_Setting["Output_File_Daily"].as<bool>();
    conf["Log_Setting"]["Output_File_Hourly"] = Log_Setting["Output_File_Hourly"].as<bool>();
    conf["Log_Setting"]["Output_File_Rotate"] = Log_Setting["Output_File_Rotate"].as<bool>();
    conf["Log_Setting"]["File_Rotating_Size"] = Log_Setting["File_Rotating_Size"].as<int>();
    conf["Log_Setting"]["File_Rotating_Quata"] = Log_Setting["File_Rotating_Quata"].as<int>();
    conf["Log_Setting"]["File_Save_Path"] = Log_Setting["File_Save_Path"].as<std::string>();

    auto Debug_Mode = Conf["Debug_Mode"];
    conf["Debug_Mode"]["Core_Dump"] = Debug_Mode["Core_Dump"].as<bool>();
    conf["Debug_Mode"]["Output_Debug"] = Debug_Mode["Output_Debug"].as<bool>();

    return conf;
}

json load_Core_Conf(const char *conf_directory)
{
    json conf;
    std::string Path = conf_directory;
    YAML::Node Conf = YAML::LoadFile(Path + "Caster_Core.yml");

    auto Caster_Setting = Conf["Caster_Setting"];
    conf["Update_Intv"] = Caster_Setting["Update_Intv"].as<int>();
    conf["Unactive_Time"] = Caster_Setting["Unactive_Time"].as<int>();

    auto Base_Setting = Conf["Base_Setting"];
    conf["Base_Enable_Mult"] = Base_Setting["Enable_Mult"].as<bool>();
    conf["Base_Keep_Early"] = Base_Setting["Keep_Early"].as<bool>();

    auto Rover_Setting = Conf["Rover_Setting"];
    conf["Rover_Enable_Mult"] = Rover_Setting["Enable_Mult"].as<bool>();
    conf["Rover_Keep_Early"] = Rover_Setting["Keep_Early"].as<bool>();

    auto Redis_Setting = Conf["Reids_Connect_Setting"];
    conf["Redis_IP"] = Redis_Setting["IP"].as<std::string>();
    conf["Redis_Port"] = Redis_Setting["Port"].as<int>();
    conf["Redis_Requirepass"] = Redis_Setting["Requirepass"].as<std::string>();

    return conf;
}

json load_Auth_Conf(const char *conf_directory)
{
    json conf;
    std::string Path = conf_directory;
    YAML::Node Conf = YAML::LoadFile(Path + "Auth_Verify.yml");

    auto Redis_Setting = Conf["Reids_Connect_Setting"];
    conf["Redis_IP"] = Redis_Setting["IP"].as<std::string>();
    conf["Redis_Port"] = Redis_Setting["Port"].as<int>();
    conf["Redis_Requirepass"] = Redis_Setting["Requirepass"].as<std::string>();

    return conf;
}

int switch_Working_Dir(std::string exe_path)
{
    std::string exepath = exe_path;

    // 找到路径中最后一个斜杠的位置
    size_t lastSlashPos = exepath.find_last_of("/\\");
    if (lastSlashPos == std::string::npos)
    {
        spdlog::error("Unable to extract directory from executable path.");
        return 1;
    }
    // 提取路径
    std::string exeDir = exepath.substr(0, lastSlashPos);

    // 切换工作目录
#if defined(_MSC_VER)
    if (_chdir(exeDir.c_str()) != 0)
    {
        spdlog::error("Failed to change working directory.");
        return 1;
    }
#else
    if (chdir(exeDir.c_str()) != 0)
    {
        spdlog::error("Failed to change working directory.");
        return 1;
    }
#endif

    spdlog::info("Switch Working directory: {}", exeDir);
    return 0;
}

int main(int argc, char **argv)
{
#ifdef WIN32

#else
    // 启用dump
    prctl(PR_SET_DUMPABLE, 1);

    // 设置core dump大小
    struct rlimit rlimit_core;
    rlimit_core.rlim_cur = RLIM_INFINITY; // 设置大小为无限
    rlimit_core.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &rlimit_core);
#endif

    // 程序启动
    spdlog::info("Software: {}-{}", PROJECT_SET_NAME, PROJECT_SET_VERSION);
    spdlog::info("Tag Version: {}", PROJECT_TAG_VERSION);
    spdlog::info("Git Version: {}", PROJECT_GIT_VERSION);

    // 解析输入：
    std::string conf_path = CONF_PATH;
    int listen_port = -1;

    if (argc < 1)
    {
        return 1;
    }

    if (argc == 2)
    {
        if (!strcmp(argv[1], "-info")) // 监听端口
        {
            exit(0);
        }
    }
    if (argc > 2)
    {
        for (int i = 1; i < argc; i += 2)
        {
            if (!strcmp(argv[i], "-port")) // 监听端口
            {
                listen_port = atoi(argv[i + 1]);
            }
            else if (!strcmp(argv[i], "-conf")) // 配置文件路径
            {
                conf_path = argv[i + 1];
            }
        }
    }

    switch_Working_Dir(argv[0]); // 切换工作路径到可执行目录下

    // 打开配置文件
    spdlog::info("Conf Path:{}", conf_path);
    // 读取全局配置
    spdlog::info("Load Conf...");
    json cfg;
    cfg["Service_Setting"] = load_Caster_Conf(conf_path.c_str());
    cfg["Core_Setting"] = load_Core_Conf(conf_path.c_str());
    cfg["Auth_Setting"] = load_Auth_Conf(conf_path.c_str());

    if (listen_port > 0)
    {
        cfg["Service_Setting"]["Ntrip_Listener"]["Port"] = listen_port;
    }

    // 日志输出选项
    bool log_to_std = cfg["Service_Setting"]["Log_Setting"]["Output_STD"];
    bool log_to_file = cfg["Service_Setting"]["Log_Setting"]["Output_File"];
    bool log_file_daily = cfg["Service_Setting"]["Log_Setting"]["Output_File_Daily"];
    bool log_file_hourly = cfg["Service_Setting"]["Log_Setting"]["Output_File_Hourly"];
    bool log_file_rotate = cfg["Service_Setting"]["Log_Setting"]["Output_File_Rotate"];
    int log_rotating_size = cfg["Service_Setting"]["Log_Setting"]["File_Rotating_Size"];
    int log_rotating_quata = cfg["Service_Setting"]["Log_Setting"]["File_Rotating_Quata"];
    std::string log_save_path = cfg["Service_Setting"]["Log_Setting"]["File_Save_Path"];

    // 开发者模式相关
    bool Core_Dump = cfg["Service_Setting"]["Debug_Mode"]["Core_Dump"];
    bool log_debug = cfg["Service_Setting"]["Debug_Mode"]["Output_Debug"];

    if (!Core_Dump) // 是否需要关闭 coredump
    {
#ifdef WIN32

#else
        prctl(PR_SET_DUMPABLE, 0);
#endif
    }

    // 初始化日志系统
    std::vector<spdlog::sink_ptr> sinks;
    if (log_to_std) // 输出到控制台
    {
        spdlog::info("Write log to Std...");
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
    }
    if (log_to_file) // 输出到文件
    {
        if (log_file_daily)
        {
            spdlog::info("Write log to File Daily...");
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(log_save_path, 0, 0));
        }
        if (log_file_hourly)
        {
            spdlog::info("Write log to File Hourly...");
            sinks.push_back(std::make_shared<spdlog::sinks::hourly_file_sink_st>(log_save_path));
        }
        if (log_file_rotate)
        {
            spdlog::info("Write log to File Rotate...");
            size_t max_file_size = log_rotating_size * 1024 * 1024; // 单个日志文件大小：X MB
            size_t max_files = log_rotating_quata;                   // 最多保留 X 个文件
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>(log_save_path,max_file_size,max_files));
        }
    }
    // 把所有sink放入logger
    auto logger = std::make_shared<spdlog::logger>("log", begin(sinks), end(sinks));
    spdlog::set_default_logger(logger);
    spdlog::flush_on(spdlog::level::info); // 立即刷新日志
    if (log_debug)                         // 设置输出日志的级别
    {
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::debug);
    }

    // 初始化完成，开始进入正式流程
    spdlog::info("Software: {}-{}", PROJECT_SET_NAME, PROJECT_SET_VERSION);
    spdlog::info("Tag Version: {}", PROJECT_TAG_VERSION);
    spdlog::info("Git Version: {}", PROJECT_GIT_VERSION);

#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        spdlog::info("WSAStartup failed! exit.");
        return 1;
    }
#endif

    // auto x = new ntrip_caster(cfg);
    // delete x;
    // x->start();

    spdlog::info("Init Server...");

    ntrip_caster a(cfg); // 创建一个对象，传入config

    // auto str1 = a._license_check.gen_register_file();
    // a._license_check.load_license_file();
    // a._license_check.fresh_license_file();

    spdlog::info("Start Server...");
    a.start(); // 原神，启动！

#ifdef WIN32
    WSACleanup();
#endif

    spdlog::info("Exit!");
    return 0;
}
