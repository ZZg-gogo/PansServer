#ifndef __PANS_LOGGER_LOG_RECORD_H__
#define __PANS_LOGGER_LOG_RECORD_H__

#include <chrono>
#include <cstdint>
#include <source_location>
#include <string_view>

#include <pans/logger/logger_level.h>
#include <pans/macros.h>



namespace pans::detail {
//日志事件所需要的数据
struct LogRecordView
{
    LogLevel::Level m_loggerLevel = LogLevel::Level::LOG_LV_DEBUG;  //默认的日志级别
    std::string_view m_loggerName = "";     //日志器的名字 可能会有多个日志器
    std::string_view m_message = "";    //日志的内容
    std::chrono::system_clock::time_point m_timestamp;  //时间戳
    std::chrono::steady_clock::duration m_elapsed;  //起服到现在已经过去的时间
    u64 m_threadId = 0; //线程ID
    u64 m_fiberId = 0;  //协程ID
    std::string_view m_threadName = ""; //线程名
    std::string_view m_fileName = "";   //文件名
    std::string_view m_functionName = "";   //函数名
    std::source_location m_location;    //C++20标准 用于表达文件位置 编译期由编译器自动完成 运行期间0消耗
};





}








#endif