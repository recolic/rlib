#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <rlib/condition_variable.hpp>

#include <thread>
#include <iostream>

rlib::easy_condition_variable rcv, rcv_done;
volatile int globalTestVar = 0;
void t1() {
    rcv.wait();
    globalTestVar *= 3;
    rcv_done.notify_all();
}
void t2() {
    rcv.wait();
    globalTestVar *= 2;
    rcv_done.notify_all();
}
void t3() {
    globalTestVar += 3;
    rcv.notify_one();
    rcv_done.wait();
}


TEST_CASE("condition_variable_1") {
    const auto tests = 512;
    std::cout << "Running tests... It may be slow." << std::endl;
    for(auto cter = 0; cter < 1024; ++cter) {
        globalTestVar = 0;
        auto th1 = std::thread(t1);
        auto th2 = std::thread(t2);
        auto th3 = std::thread(t3);
        th3.join();
        REQUIRE((globalTestVar == 9 || globalTestVar == 6));
        rcv.notify_all();
        th1.join();
        th2.join();
        REQUIRE(globalTestVar == 18);
    }
}


