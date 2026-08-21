#ifndef __PANS_LOGGER_FORMATTER_H__
#define __PANS_LOGGER_FORMATTER_H__


#include <memory>
#include <string>
#include <vector>


#include "logger/buffer.h"
#include "logger/buffer_config.h"
#include "logger/log_record.h"


namespace pans::detail {
//这个日志记录的默认缓冲区大小是FORMATTED_RECORD_INLINE_CAPACITY
using FormattedRecordBuffer = InlineBuffer<FORMATTED_RECORD_INLINE_CAPACITY>;


class Formatter final
{
public:
    class FormatItem   //所有formatitem的父类
    {
    public:
        virtual ~FormatItem() = default;
        //record里面的内容以什么格式输出到output
        virtual void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept = 0;
    };


public:
    explicit Formatter(std::string_view pattern);
    void format(const LogRecordView& record, FormattedRecordBuffer& output) const noexcept;
    [[nodiscard]] const std::string& getPattern() const noexcept {return m_pattern;}

private:
    int parse() noexcept;
    void addLiteral(std::string& literal) noexcept;
private:
    std::string m_pattern = "";
    std::vector<std::unique_ptr<FormatItem>> m_items;
};



}








#endif