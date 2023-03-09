#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <queue>
#include <any>
#include <thread>
#include <functional>
#include <mutex>
#include <unordered_set>
#include <condition_variable>
#include <atomic>

namespace thpl{
    using task_id_t = unsigned long;
    using in_t = unsigned long;

    class ThreadPool {
    private:
        // threads
        std::vector<std::thread> threads;

        // tasks queue
        std::queue<std::pair<std::function<std::any()>, task_id_t>> q; // task, task_id
        std::mutex q_mtx;
        std::condition_variable q_cv;

        // store completed tasks results
        std::unordered_map<task_id_t, std::any> tasks_um;
        std::mutex tasks_um_mtx;
        std::condition_variable tasks_um_cv;

        // stop flag
        std::atomic<bool> stop = false;

        // next task's id
        std::atomic<task_id_t> next_id = 0;

        void run();

    public:
        explicit ThreadPool(size_t pool_size);

        ~ThreadPool();

        std::any wait(task_id_t task_id);
        void wait_all();

        task_id_t add_task(const std::function<std::any(in_t, in_t)> &func, in_t in1, in_t in2);
    };
}

#endif //THREADPOOL_THREADPOOL_H
