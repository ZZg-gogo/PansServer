#ifndef __PANS_INCLUDE_MUTEX_H__
#define __PANS_INCLUDE_MUTEX_H__

#include <atomic>
#include <mutex>

//判断CPU架构
#if defined(__x86_64__) || defined(__i386__)
    #include <immintrin.h>
#elif defined(__aarch64__) || defined(__arm__)
    #include <arm_acle.h>
#endif

#if defined(__linux__) && defined(__GLIBC__)
    #include <pthread.h>
#endif


inline void cpu_relax() noexcept
{
//告诉CPU我现在是在自旋等待，你可以针对这种情况做一些优化，不需要这么激进地执行
#if defined(__x86_64__) || defined(__i386__)
    _mm_pause();    
#elif defined(__aarch64__) || defined(__arm__)
    __yield();
#endif 
}



namespace pans{

#if defined(__linux__) && defined(__GLIBC__)
class Spinlock
{
public:
    Spinlock() noexcept
    {
        //只允许该进程内部使用
        pthread_spin_init(&m_mutex, PTHREAD_PROCESS_PRIVATE);   
    }

    ~Spinlock() noexcept
    {
        pthread_spin_destroy(&m_mutex);
    }


    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete; 

    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex{};
};

#else

class Spinlock
{
public:
    using Lock = std::lock_guard<Spinlock>;

    Spinlock() noexcept = default;
    ~Spinlock() noexcept = default;

    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;


    void lock() noexcept
    {
        //锁之后的指令不会排到获取到锁之前
        while (m_mutex.test_and_set(std::memory_order_acquire)) //尝试抢占锁
        {
            while (m_mutex.test(std::memory_order_relaxed)) //抢占失败 然后查询锁
            {
                cpu_relax();
            }
            
        }
    }

    [[nodiscard]] bool try_lock() noexcept
    {
        return !m_mutex.test_and_set(std::memory_order_acquire);
    }

    void unlock() noexcept
    {
        // 临界区的所有修改
        // 在这里之前必须完成 内存序不会变
        m_mutex.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_mutex = ATOMIC_FLAG_INIT;
};

#endif

}





#endif //__PANS_INCLUDE_MUTEX_H__