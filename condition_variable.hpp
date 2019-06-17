#ifndef _RLIB_EASY_CONDITION_VAR
#define _RLIB_EASY_CONDITION_VAR 1

#include <condition_variable>
#include <mutex>
#include <atomic>

#include <rlib/class_decorator.hpp>

namespace rlib {
    class easy_condition_variable : rlib::noncopyable {
    public:
        easy_condition_variable()
            : stdcv(), stdcv_mutex(), notify_count(0) {}
        
        void notify_one() {
            stdcv.notify_one();
        }
        void notify_all() {
            stdcv.notify_all();
        }

        void wait() {
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            stdcv.wait(lk, [this]{return notify_count.load() > 0;});
            lk.unlock();
        }
        template< class Rep, class Period >
        bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            bool ret = stdcv.wait_for(lk, rel_time, [this]{return notify_count.load() > 0;});
            lk.unlock();
            return ret;
        }
        template< class Clock, class Duration >
        bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            bool ret = stdcv.wait_until(lk, timeout_time, [this]{return notify_count.load() > 0;});
            lk.unlock();
            return ret;
        }
    private:
        std::condition_variable stdcv;
        std::mutex stdcv_mutex;
        std::atomic<int> notify_count;
    };
} // namespace rlib



#endif