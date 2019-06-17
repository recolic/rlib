#ifndef _RLIB_OBJ_POOL_HPP
#define _RLIB_OBJ_POOL_HPP 1

#include <rlib/impl/traceable_list.hpp>
#include <rlib/class_decorator.hpp>
#include <utility>
#include <tuple>
#include <functional>
#include <algorithm>
#include <atomic>

#ifdef RLIB_SWITCH_USE_MINGW_THREAD_FIX
#include <mingw.mutex.h>
#include <mingw.thread.h>
#include <mingw.condition_variable.h>
#else
#include <thread>
#include <mutex>
#include <condition_variable>
#endif
using size_t = unsigned long;

namespace rlib {
    struct object_pool_policy_fixed {
        object_pool_policy_fixed(size_t size) : size(size) {}
        size_t objects_should_alloc(const size_t inuse_objects, const size_t avail_objects) const {
            return avail_objects < size ? size - avail_objects : 0;
        }
    private:
        const size_t size;
    };
    struct object_pool_policy_watermarks {
        // If free/all < alloc_threshold_rate, then alloc more objects.
        object_pool_policy_watermarks(float alloc_threshold_rate = 0.8, size_t min_objects = 8)
            : _alloc_threshold_rate(1.0/alloc_threshold_rate), min_objects(min_objects)  {}
        size_t objects_should_alloc(const size_t inuse_objects, const size_t avail_objects) const {
            if(avail_objects < min_objects)
                return min_objects - avail_objects;
            size_t threshold = inuse_objects * _alloc_threshold_rate;
            if(threshold > avail_objects)
                return threshold - avail_objects;
            return 0;
        }
    private:
        const float _alloc_threshold_rate;
        const size_t min_objects;
    };
    struct object_pool_policy_dynamic_smart {

    };

    /*
     * Multi-threaded object_pool. It will block current thread and wait if
     *     borrow_one() starves, until some other threads release their obj.
     */
    template<typename policy_t, typename obj_t, typename... _bound_construct_args_t>
    class object_pool : rlib::nonmovable {
    protected:
        using element_t = obj_t;
        using buffer_t = impl::traceable_list<obj_t, bool>;
        using this_type = object_pool<obj_t, _bound_construct_args_t ...>;
    public:
        object_pool() = delete;
        explicit object_pool(policy_t &&policy, _bound_construct_args_t ... _args)
                : policy(std::forward<policy_t>(policy)), _bound_args(std::forward<_bound_construct_args_t>(_args) ...) 
        {}

//        void fill_full() {
//            for (size_t cter = 0; cter < max_size; ++cter) {
//                new_obj_to_buffer();
//                free_list.push_back(&*--buffer.end());
//            }
//        }

        // `new` an object. Return nullptr if pool is full.
        obj_t *try_borrow_one() {
            std::lock_guard<std::mutex> _l(buffer_mutex);
            return do_try_borrow_one();
        }
        obj_t *borrow_one() {
            auto result = try_borrow_one();
            if(result)
                return result;
            // Not available. Wait for release_one.
            std::unique_lock<std::mutex> lk(buffer_mutex);

            // wait for a release
            borrow_cv.wait(lk, [this]{return this->new_obj_ready;});

            result = do_try_borrow_one();
            lk.unlock();
            if(!result)
                throw std::logic_error("unknown par error.");
            return result;
        }
        void release_one(obj_t *which) {
            {
                std::lock_guard<std::mutex> _l(buffer_mutex);
                free_list.push_front(which);
                typename buffer_t::iterator elem_iter(which);
                elem_iter.get_extra_info() = true; // mark as free.
                new_obj_ready = true;
                --inuse_objects;
            } // lock released.
            borrow_cv.notify_one();
        }

        void reconstruct_one(obj_t *which) {
            reconstruct_impl(which, std::make_index_sequence<sizeof...(_bound_construct_args_t)>());
        }

        size_t inuse_size() const {
            return inuse_objects;
        }
        size_t size() const {
            // total size. inuse + free.
            return inuse_objects + free_list.size();
        }

    protected:
        buffer_t buffer; // list<obj_t obj, bool is_free>
    private:
        std::list<obj_t *> free_list;
        std::mutex buffer_mutex; // mutex for buffer and free_list

        std::tuple<_bound_construct_args_t ...> _bound_args;

        size_t inuse_objects = 0;
        const policy_t policy;
        std::condition_variable borrow_cv; // use buffer_mutex on notifying alloc event. 
        volatile bool new_obj_ready = false;
        void notify_new_object_allocated(size_t how_many) {
            new_obj_ready = true;
            for(auto cter = 0; cter < how_many; ++cter)
                borrow_cv.notify_one();
        }

        // try_borrow_one without lock.
        obj_t *do_try_borrow_one() {
            // Optimize here if is performance bottleneck (lockless list... etc...)
            // NOT THREAD SAFE. USE buffer_mutex.
            if(policy.borrow_should_alloc()) {
                new_obj_to_buffer();
                free_list.push_back(&*--buffer.end());
            }
            if (free_list.size() > 0) {
                // Some object is free. Just return one.
                obj_t *result = *free_list.begin();
                free_list.pop_front();

                typename buffer_t::iterator elem_iter(result);
                elem_iter.get_extra_info() = false; // mark as busy.
                new_obj_ready = false;
                ++inuse_objects;
                return result;
            }
            return nullptr;
        }

        // fake emplace_back
        template<size_t ... index_seq>
        inline void new_obj_to_buffer_impl(std::index_sequence<index_seq ...>) {
            buffer.emplace_one(buffer.end(), true, std::get<index_seq>(_bound_args) ...);
        }
        template<size_t ... index_seq>
        inline void reconstruct_impl(obj_t *which, std::index_sequence<index_seq ...>) {
            which->~obj_t();
            new(which) obj_t(std::get<index_seq>(_bound_args) ...);
        }

        inline void new_obj_to_buffer() {
            new_obj_to_buffer_impl(std::make_index_sequence<sizeof...(_bound_construct_args_t)>());
        }

        std::mutex alloc_thread_notify_mutex;
        std::condition_variable alloc_thread_notify_cv;
        volatile bool alloc_thread_should_trigger = false;

        void alloc_thread_func() {
            // A new thread to alloc new objects basing on policy.
            while(true) {
                std::unique_lock<std::mutex> lk(alloc_thread_notify_mutex);
                alloc_thread_notify_cv.wait(lk, [this]{return this->alloc_thread_should_trigger;});
                // serial area.
                lk.unlock();

                auto should_alloc = policy.objects_should_alloc(inuse_size(), size());
                {
                    std::lock_guard<std::mutex> _l(buffer_mutex);
                    for(auto cter = 0; cter < should_alloc; ++cter) {
                        // Alloc a new object.
                        new_obj_to_buffer();
                        free_list.push_back(&*--buffer.end());
                    }
                }
                if(should_alloc > 0) {

                }
            }
        }
    };
}

#endif
