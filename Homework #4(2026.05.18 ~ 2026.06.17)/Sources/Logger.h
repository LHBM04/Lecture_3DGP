#pragma once

#include <format>
#include <print>
#include <utility>

/** DEBUG 여부에 따른 매크로 */
#ifdef _DEBUG
    #define LOGTRACE(...) Logger::LogTrace(__VA_ARGS__)
    #define LOGINFO(...) Logger::LogInfo(__VA_ARGS__)
    #define LOGWARNING(...) Logger::LogWarning(__VA_ARGS__)
    #define LOGERROR(...) Logger::LogError(__VA_ARGS__)
    #define LOGCRITICAL(...) Logger::LogCritical(__VA_ARGS__)
#else
    #define LOGTRACE(...) ((void)0)
    #define LOGINFO(...) ((void)0)
    #define LOGWARNING(...) ((void)0)
    #define LOGERROR(...) ((void)0)
    #define LOGCRITICAL(...) ((void)0)
#endif

namespace Logger
{
    /** 추적용 로그 */
    template<class... Args>
    void LogTrace(std::format_string<Args...> format, Args&&... args)
    {
        std::println(format, std::forward<Args>(args)...);
    }

    /** 정보용 로그 */
    template<class... Args>
    void LogInfo(std::format_string<Args...> format, Args&&... args)
    {
        std::println(format, std::forward<Args>(args)...);
    }

    /** 경고용 로그 */
    template<class... Args>
    void LogWarning(std::format_string<Args...> format, Args&&... args)
    {
        std::println(format, std::forward<Args>(args)...);
    }

	/** 오류용 로그 */
    template<class... Args>
    void LogError(std::format_string<Args...> format, Args&&... args)
    {
        std::println(format, std::forward<Args>(args)...);
    }

	/** 치명적인 오류용 로그 */
    template<class... Args>
    void LogCritical(std::format_string<Args...> format, Args&&... args)
    {
        std::println(format, std::forward<Args>(args)...);
    }
}
