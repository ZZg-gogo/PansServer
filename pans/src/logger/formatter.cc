#include "logger/formatter.h"
#include <unordered_map>
#include <logger/logger_level.h>



namespace pans::detail{

constexpr std::string_view DEFAULT_DATE_FORMAT = "%Y-%m-%d %H:%M:%S";

Formatter::Formatter(std::string_view pattern)
    : m_pattern(pattern)
{
    if (parse())
    {
        throw std::invalid_argument("invalid logger  formatter parse failed");
    }
    

}



void Formatter::format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept
{

    for (const auto& item : m_items)
    {
        item->format(record, output);
    }
    
}



template<typename T>
void AppendInteger(FormattedRecordBuffer& output, T value) noexcept
{
    std::array<char, 24> buffer{};  //直接在栈上分配空间

    const auto result = std::to_chars(buffer.data(), buffer.data()+buffer.size(), value);
    ASSERT_RETNONE2(result.ec == std::errc(), " trans" << value << " to chars failed");
    output.append(buffer.data(), static_cast<std::size_t>(result.ptr - buffer.data()));
}



int Formatter::parse() noexcept
{

}


class LiteralFormatItem final : public Formatter::FormatItem
{

public:
    explicit LiteralFormatItem(std::string_view value)
        : m_value(value)
    {}

public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(m_value);
    }

private:
    std::string m_value;
};


class MessageFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_message);
    }  
};


//日志等级item
class LoggerLevelFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(LogLevel::ToString(record.m_loggerLevel));
    }  
};


//已经开服了多少时间的item
class ElapsedFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        //把 record.m_elapsed 这个时间间隔转换成“毫秒数”，然后拿到具体的数字
        AppendInteger(output, std::chrono::duration_cast<std::chrono::milliseconds>(record.m_elapsed).count());
    }    

};


//日志器的名字item
class LoggerNameFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append(record.m_loggerName);
    }    

};


//线程ID item
class ThreadIdFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        AppendInteger(output, record.m_threadId);
    }    
};


//换行 item
class NewLineFormatItem final : public Formatter::FormatItem
{
public:
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept override
    {
        output.append('\n');
    }    

};


}