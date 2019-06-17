#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include <rlib/pool.hpp>

#include <string>
using std::string;

struct pooled_obj_t {
    pooled_obj_t(int arg1, string arg2)
        : arg1(arg1), arg2(arg2) {}
    
    pooled_obj_t(const pooled_obj_t &another) = delete;
    pooled_obj_t() = delete;

    int arg1;
    string arg2;
};

struct pooled_obj2_t {
    pooled_obj2_t(string arg1)
        : arg1(arg1) {}
    
    pooled_obj2_t(const pooled_obj2_t &another) = delete;
    pooled_obj2_t() = delete;

    string arg1;
};

TEST_CASE("fixed object pool") {
    size_t pool_size = 8;
    const auto arg1 = 666;
    const auto arg2 = string("fuck you");
    rlib::object_pool<rlib::object_pool_policy_fixed, pooled_obj_t, decltype(arg1), decltype(arg2)> 
        fixed_pool(rlib::object_pool_policy_fixed(pool_size), arg1, arg2);
    
    auto res = fixed_pool.try_borrow_one();
    REQUIRE(res != nullptr);
    REQUIRE(res->arg1 == arg1);
    REQUIRE(res->arg2 == arg2);

    size_t test_rounds = 1024;

    for(auto _ = 0; _ < test_rounds; ++_) {
        std::list<decltype(res)> objs;
        for(auto cter = 0; cter < pool_size - 1; ++cter) {
            auto ptr = fixed_pool.try_borrow_one();
            REQUIRE(ptr != nullptr);
            REQUIRE(ptr->arg2 == arg2);
            fixed_pool.reconstruct_one(ptr);
            REQUIRE(ptr->arg2 == arg2);
            objs.push_back(ptr);
        }
        REQUIRE(fixed_pool.try_borrow_one() == nullptr);
        REQUIRE(fixed_pool.try_borrow_one() == nullptr);
        for(auto cter = 0; cter < pool_size - 1; ++cter) {
            fixed_pool.release_one(*objs.begin());
            objs.pop_front();
        }
    }
    REQUIRE(fixed_pool.size() == pool_size);
}

TEST_CASE("infinite dynamic object pool") {
    const auto arg1 = string("fuck you 2");
    rlib::object_pool<rlib::object_pool_policy_watermarks, pooled_obj2_t, decltype(arg1)>
        inf_pool(rlib::object_pool_policy_watermarks(), arg1);
    
    auto res = inf_pool.try_borrow_one();
    REQUIRE(res != nullptr);
    REQUIRE(res->arg1 == arg1);

    size_t pool_size = 16;
    size_t test_rounds = 512;

    for(auto _ = 0; _ < test_rounds; ++_) {
        std::list<decltype(res)> objs;
        for(auto cter = 0; cter < pool_size; ++cter) {
            auto ptr = inf_pool.try_borrow_one();
            REQUIRE(ptr != nullptr);
            REQUIRE(ptr->arg1 == arg1);
            inf_pool.reconstruct_one(ptr);
            REQUIRE(ptr->arg1 == arg1);
            objs.push_back(ptr);
        }
        for(auto cter = 0; cter < pool_size - 1; ++cter) {
            inf_pool.release_one(*objs.begin());
            objs.pop_front();
        }
        // "Leak" one object.
    }
}

TEST_CASE("fixed object pool parallel test") {
    size_t pool_size = 8;
    const auto arg1 = 666;
    const auto arg2 = string("fuck you");
    rlib::object_pool<rlib::object_pool_policy_fixed, pooled_obj_t, decltype(arg1), decltype(arg2)> 
        fixed_pool(rlib::object_pool_policy_fixed(pool_size), arg1, arg2);
 
}

