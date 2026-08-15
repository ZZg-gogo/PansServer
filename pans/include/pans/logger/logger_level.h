#ifndef __PANS_INCLUDE_PANS_LOGGER_LOGGER_LEVEL_H__
#define __PANS_INCLUDE_PANS_LOGGER_LOGGER_LEVEL_H__

#include <cstdint>
#include <string_view>

#include <pans/export.h>



namespace pans{

class PANS_API LogLevel final
{
public :

    enum class Level : std::uint8_t
    {
        LOG_LV_DEBUG = 1,       //记录调试细节 如变量值 程序执行流程
        LOG_LV_INFO = 2,        //记录正常的运行的信息 如服务启动 数据库连接成功 玩家连接成功
        LOG_LV_WARN = 3,        //可能存在问题 但是程序仍然可以继续运行下去
        LOG_LV_ERROR = 4,       //操作失败或功能异常 需要检查处理
        LOG_LV_FATAL = 5,       //严重错误 程序继续执行可能存在较大问题
        LOG_LV_OFF = 6,         //关闭日志输出
    };


    [[nodiscard]] static std::string_view ToString(Level level) noexcept;
    [[nodiscard]] static LogLevel::Level FromString(std::string_view value) noexcept;
};



}




#endif // __PANS_INCLUDE_PANS_LOGGER_LOGGER_LEVEL_H__