#include "ThreadPool.h"

#include <iostream>
#include <functional>
#include <cmath>

using namespace thpl;

template<class out_t, class in1_t, class in2_t>
struct Request{
    size_t processingTime;
    std::function<out_t(in1_t, in2_t)> function;
    out_t result;
};


int main() {
    size_t poolSize;
    std::cout << "Size of pool:";
    std::cin >> poolSize;

    auto f1 = [](in_t in1, in_t in2) -> std::any {
        double result = 0;
        for(in_t i = 0;i < in1;++i){
            result += std::sqrt(in2 - i);
        }
//        result += std::log10(in1) + std::log10(in2);
        return result;
    };

    auto f2 = [](in_t in1, in_t in2) -> std::any {
        double result = 0;
        for(in_t i = 0;i < in1;++i){
            for(in_t j = 0;j < in2;++j){
                result += std::log10(std::pow(i + j, 11));
            }
        }
//        result = std::log(in1) + std::log(in2);
        return result;
    };

    std::vector<std::function<std::any(in_t, in_t)>> funcs = {f1, f2};
    std::vector<std::pair<in_t, in_t>> args = {
            {10, 100},
            {100, 100},
            {1000, 100},
            {10000, 100},
            {1000, 1000},
            {1000, 1000},
            {5000, 5000},
            {5000, 23000},
            {1200, 2300},
            {2300, 2800},
            {5400, 2000},
            {1000, 2320},
    };

    ThreadPool thread_pool(poolSize);
    std::vector<task_id_t> task_ids;
    for(const auto &arg : args){
        for(const auto &func : funcs){
            task_ids.emplace_back(thread_pool.add_task(func, arg.first, arg.second));
        }
    }

    for(const auto &id : task_ids){
        std::cout << std::any_cast<double>(thread_pool.wait(id)) << std::endl;
    }



    return 0;
}