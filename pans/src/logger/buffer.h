#ifndef __PANS_SRC_LOGGER_BUFFER_H__
#define __PANS_SRC_LOGGER_BUFFER_H__


#include <algorithm>
#include <array>
#include <cstring>
#include <cstddef>
#include <streambuf>
#include <string_view>
#include <vector>



/*
================================================================================
                        std::streambuf 工作机制
================================================================================

一、std::ostream 和 std::streambuf 的关系
------------------------------------------

std::ostream 主要负责：

    1. 提供 <<、put() 等高级输出接口
    2. 处理各种类型的数据格式化
       例如：
           os << 123;
           os << "hello";
           os << 3.14;

但是 ostream 本身并不关心：

    “这些数据最终写到哪里？”

真正负责“数据最终怎么被写出去”的，是它内部关联的 std::streambuf。


整体关系：

                    std::ostream
                         |
                         | 负责格式化
                         |
                         v
                    std::streambuf
                         |
                         | 负责实际输出
                         |
              +----------+----------+
              |                     |
          单个字符               多个字符
              |                     |
            sputc()               sputn()
              |                     |
              v                     v
          overflow()             xsputn()
              |                     |
              +----------+----------+
                         |
                         v
                  实际的数据存储
                  / 文件 / 网络等


二、std::streambuf 内部的缓冲区
--------------------------------

std::streambuf 本身支持两套区域：

    1. get area  : 输入缓冲区
    2. put area  : 输出缓冲区

输入缓冲区有三个指针：

    eback()     gptr()      egptr()
       |          |            |
       v          v            v
       +----------+------------+
       | 已经读取 | 还可以读取 |
       +----------+------------+

输出缓冲区也有三个指针：

    pbase()     pptr()      epptr()
       |          |            |
       v          v            v
       +----------+------------+
       | 已经写入 | 还可以写入 |
       +----------+------------+

其中：

    pbase()  = 输出缓冲区开始
    pptr()   = 下一次写入的位置
    epptr()  = 输出缓冲区结束

判断还有没有输出空间：

    pptr() < epptr()

判断还有没有输入数据：

    gptr() < egptr()


三、std::streambuf 默认没有自己的缓冲区
----------------------------------------

std::streambuf 的构造函数中：

    _M_in_beg  = nullptr;
    _M_in_cur  = nullptr;
    _M_in_end  = nullptr;

    _M_out_beg = nullptr;
    _M_out_cur = nullptr;
    _M_out_end = nullptr;

也就是说：

    eback() = nullptr
    gptr()  = nullptr
    egptr() = nullptr

    pbase() = nullptr
    pptr()  = nullptr
    epptr() = nullptr

所以 std::streambuf 默认情况下：

    没有输入缓冲区
    没有输出缓冲区

如果派生类想让 streambuf 使用一块实际的缓冲区，
可以调用：

    setg()     // 设置输入缓冲区
    setp()     // 设置输出缓冲区


四、单字符输出：sputc()
------------------------

std::streambuf 提供了 sputc()，它不是我们自己重写的。

它的核心逻辑类似：

    int_type sputc(char_type c)
    {
        if (pptr() < epptr())
        {
            *pptr() = c;
            pbump(1);
            return c;
        }
        else
        {
            return overflow(c);
        }
    }

也就是说：

                    sputc('A')
                         |
                         v
                  pptr() < epptr() ?
                     /          \
                   是            否
                   |              |
                   v              v
                直接写         overflow('A')
                缓冲区              |
                                   v
                            派生类自己处理


所以：

    单个字符并不是直接调用 overflow()。

准确流程是：

    单字符
       |
       v
    sputc()
       |
       +---- 有 put area 空间 ----> 直接写入
       |
       +---- 没有 put area 空间 --> overflow()


五、多个字符输出：sputn() / xsputn()
------------------------------------

std::streambuf 同样提供了 sputn()：

    sputn(data, size)

它内部会调用虚函数：

    xsputn(data, size)

所以：

    多个字符
       |
       v
    sputn()
       |
       v
    xsputn()
       |
       v
    批量处理数据


xsputn() 的存在主要是为了：

    不要把一大段数据拆成一个一个字符处理。

例如：

    ostream << "Hello World";

可以一次把：

    "Hello World"

交给 xsputn() 处理。


六、overflow() 到底是什么？
----------------------------

overflow() 名字里的 "overflow" 指的是：

    std::streambuf 自己的 put area 没有空间了。

例如：

    pbase()                    epptr()
       |                          |
       v                          v
       +--------------------------+
       | A B C D E F G H          |
       +--------------------------+
                                ^
                                pptr()

此时：

    pptr() == epptr()

再写一个字符：

    sputc('X')

就无法直接写入 put area。

于是：

    sputc()
       |
       v
    overflow('X')

overflow() 是一个虚函数：

    virtual int_type overflow(int_type c);

它的意义就是：

    “我的内部输出缓冲区处理不了了，
     派生类你自己决定这个字符应该怎么处理。”


七、我们的 SmallStreamBuffer 做了什么？
-----------------------------------------

我们的代码：

    class SmallStreamBuffer final : public std::streambuf
    {
        ...
    };

它选择了一种特殊的设计：

    不使用 std::streambuf 自己的 put area。

也就是说，我们没有：

    setp(...)

因此：

    pbase()
    pptr()
    epptr()

默认都是 nullptr。

所以单字符输出：

    ostream
       |
       v
    sputc('A')
       |
       v
    pptr() < epptr() ?

    false

于是：

    overflow('A')
       |
       v
    m_buffer.append('A')


我们的 overflow()：

    int_type overflow(int_type c) override
    {
        if (traits_type::eq_int_type(c, traits_type::eof()))
        {
            return traits_type::not_eof(c);
        }

        m_buffer.append(traits_type::to_char_type(c));
        return c;
    }

它的作用就是：

    “streambuf 无法直接处理这个字符，
     那我就把这个字符交给 InlineBuffer。”


八、多个字符则走 xsputn()
--------------------------

我们自己重写了：

    std::streamsize xsputn(
        const char* data,
        std::streamsize size
    ) override
    {
        m_buffer.append(data, static_cast<std::size_t>(size));
        return size;
    }

所以多个字符的流程是：

    ostream
       |
       v
    sputn(data, size)
       |
       v
    xsputn(data, size)
       |
       v
    InlineBuffer::append(data, size)


注意：

    这里没有经过 streambuf 自己的 put area。

我们直接把数据交给自己的 InlineBuffer。


九、因此我们的 SmallStreamBuffer 实际结构
------------------------------------------

                    std::ostream
                         |
                         |
              +----------+----------+
              |                     |
           单个字符               多个字符
              |                     |
              v                     v
           sputc()                sputn()
              |                     |
        pptr() < epptr()?          |
              |                     |
             否                     v
              |                  xsputn()
              v                     |
          overflow()                |
              |                     |
              +----------+----------+
                         |
                         v
                    InlineBuffer
                         |
              +----------+----------+
              |                     |
        小于 INLINE_CAPACITY      超过容量
              |                     |
              v                     v
        m_inline 数组          m_overflow vector


十、InlineBuffer 和 streambuf 是两层不同的东西
------------------------------------------------

这里一定要区分：

    std::streambuf
    InlineBuffer

它们解决的是两个不同的问题。

std::streambuf：

    负责把 std::ostream 的输出接口接到
    “实际的数据处理对象”。

InlineBuffer：

    负责真正保存日志数据。

所以：

    ostream
       |
       v
    SmallStreamBuffer
       |
       v
    InlineBuffer
       |
       +---- 小日志 --> 固定数组
       |
       +---- 大日志 --> vector


十一、为什么 InlineBuffer 又设计成 inline + vector？
------------------------------------------------------

InlineBuffer：

    std::array<char, INLINE_CAPACITY> m_inline;
    std::vector<char> m_overflow;

意思是：

    短日志：
        直接放在 stack/object 内部的固定数组中。

    长日志：
        超过固定容量以后，再切换到 vector。

例如：

    INLINE_CAPACITY = 256

正常日志：

    "player login..."
           |
           v
    m_inline[256]

不需要动态内存分配。


如果日志超过 256：

           256 bytes
        +-------------+
        | m_inline    |
        +-------------+
              |
              v
        切换到 vector
              |
              v
        m_overflow


十二、最终可以这样理解整个设计
--------------------------------

std::ostream：

    “我要输出数据。”

std::streambuf：

    “好的，我负责把 ostream 的输出接出去。”

SmallStreamBuffer：

    “我不使用 streambuf 自己的缓冲区，
     我直接把数据转发给 InlineBuffer。”

InlineBuffer：

    “好的，我负责真正保存这些日志数据，
     短日志放固定数组，长日志放 vector。”


最终：

    std::ostream
         |
         | << "hello"
         v
    SmallStreamBuffer
         |
         +---- 单字符 --> overflow()
         |
         +---- 多字符 --> xsputn()
         |
         v
    InlineBuffer
         |
         +---- 小数据 --> std::array
         |
         +---- 大数据 --> std::vector


核心记忆：

    sputc()
        = std::streambuf 提供的单字符入口

    sputn()
        = std::streambuf 提供的多字符入口

    xsputn()
        = 派生类可以重写的批量输出接口

    overflow()
        = streambuf 没有可用输出位置时，
          派生类负责处理输出的接口

    setp()
        = 如果想使用 streambuf 自己的输出缓冲区，
          就用 setp() 设置 pbase/pptr/epptr

    我们的 SmallStreamBuffer：
        不使用 setp()
        不使用 streambuf 自己的输出缓冲区
        直接把 xsputn()/overflow() 转发给 InlineBuffer
================================================================================
*/


namespace pans::detail{

template<std::size_t INLINE_CAPACITY>
class InlineBuffer
{
public:
    void append(const char *data, std::size_t len)
    {
        if (0 == len)
        {
            return;
        }
        //长日志中没有写入过任何内容 并且 短日志存储空间足够使用
        if (m_overflow.empty() && m_size + len <= INLINE_CAPACITY)
        {
            std::memcpy(m_inline.data() + m_size, data, len);
            m_size += len;
            return;
        }
        
        if (m_overflow.empty())
        {
            const std::size_t required_capacity = m_size + len;
            m_overflow.reserve(std::max(required_capacity, INLINE_CAPACITY * 2));
            //把旧的先插进来
            m_overflow.insert(m_overflow.end(), m_inline.data(), m_inline.data() + m_size);
        }
        
        //新的也进行插入
        m_overflow.insert(m_overflow.end(), data, data + len);
        m_size = m_overflow.size();
    }

    void append(std::string_view value)
    {
        append(value.data(), value.size());
    }

    void append(char value)
    {
        append(&value, 1);
    }

    [[nodiscard]] const char* data() const noexcept
    {
        return m_overflow.empty() ? m_inline.data() : m_overflow.data();
    }

    [[nodiscard]] std::size_t size() const noexcept
    {

        return m_size;
    }

    [[nodiscard]] std::string_view view() const noexcept
    {
        return {data(), size()}; 
    }
private:
    std::array<char, INLINE_CAPACITY> m_inline{}; //短日志
    std::vector<char> m_overflow;  //长日志
    std::size_t m_size = 0;         //当前已经存储了多少字节
};


template<std::size_t INLINE_CAPACITY>
class SmallStreamBuffer final : public std::streambuf
{
public:
    explicit SmallStreamBuffer(InlineBuffer<INLINE_CAPACITY>& buffer) noexcept
        : m_buffer(buffer)
    {}

private:    //重载streambuf的两个虚函数 实现输出到InlineBuffer
    std::streamsize xsputn(const char* data, std::streamsize size) override
    {
        if (size <= 0)
        {
            return 0;
        }
        m_buffer.append(data, static_cast<std::size_t>(size));
        return size;
    }

    int_type overflow(int_type c) override
    {
        if (traits_type::eq_int_type(c, traits_type::eof()))
        {
            return traits_type::not_eof(c);
        }
        
        m_buffer.append(traits_type::to_char_type(c));
        return c;
    }


private:
    InlineBuffer<INLINE_CAPACITY>& m_buffer;
};

}




#endif