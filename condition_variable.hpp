#ifndef _RLIB_EASY_CONDITION_VAR
#define _RLIB_EASY_CONDITION_VAR 1

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <numeric>

#include <rlib/class_decorator.hpp>

namespace rlib {
    class easy_condition_variable : rlib::noncopyable {
    public:
        easy_condition_variable()
            : stdcv(), stdcv_mutex(), notify_count(0), notify_all_mark(false) {}
        
        void notify_one() {
            ++notify_count;
            stdcv.notify_one();
        }
        void notify_all() {
            notify_all_mark = true;
            stdcv.notify_all();
        }

        const std::function<bool()> determine_wait_cond = [this]{
                if(notify_all_mark)
                    return true;
                while(true) {
                    auto val = notify_count.load();
                    if(val <= 0)
                        return false;
                    if(notify_count.compare_exchange_strong(val, val-1))
                        return true;
                }
            };
        void wait() {
            notify_all_mark = false;
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            stdcv.wait(lk, determine_wait_cond);
            lk.unlock();
        }
        template< class Rep, class Period >
        bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
            notify_all_mark = false;
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            bool ret = stdcv.wait_for(lk, rel_time, determine_wait_cond);
            lk.unlock();
            return ret;
        }
        template< class Clock, class Duration >
        bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
            notify_all_mark = false;
            std::unique_lock<std::mutex> lk(stdcv_mutex);
            bool ret = stdcv.wait_until(lk, timeout_time, determine_wait_cond);
            lk.unlock();
            return ret;
        }
    private:
        std::condition_variable stdcv;
        std::mutex stdcv_mutex;
        std::atomic<int> notify_count;
        std::atomic<bool> notify_all_mark;
    };
} // namespace rlib



#endif