#include "ThreadPool.h"

namespace thpl{
    ThreadPool::ThreadPool(size_t pool_size) {
        threads.reserve(pool_size);
        for(int i = 0;i < pool_size;++i){
            threads.emplace_back(&ThreadPool::run, this);
        }
    }

    void ThreadPool::run() {
        while(!stop){
            std::unique_lock<std::mutex> lock(q_mtx);

            q_cv.wait(lock, [this]() -> bool {
                return (!q.empty() || stop);
            });

            if(!q.empty()){
                auto task = std::move(q.front());
                q.pop();
                lock.unlock();

                auto result = task.first();

                std::lock_guard<std::mutex> tasks_um_lock(tasks_um_mtx);
                tasks_um[task.second] = result;
            }
            tasks_um_cv.notify_all();
        }
    }

    std::any ThreadPool::wait(task_id_t task_id) {
        std::unique_lock<std::mutex> lock(tasks_um_mtx);

        tasks_um_cv.wait(lock, [this, task_id]() -> bool {
            return tasks_um.find(task_id) != tasks_um.end();
        });
        return tasks_um[task_id];
    }

    void ThreadPool::wait_all() {
        std::unique_lock<std::mutex> lock(tasks_um_mtx);

        tasks_um_cv.wait(lock, [this]() -> bool {
            std::lock_guard<std::mutex> q_lock(q_mtx);
            return q.empty();
        });
    }

    task_id_t ThreadPool::add_task(const std::function<std::any(in_t, in_t)> &func, in_t in1, in_t in2) {
        task_id_t task_id = next_id++;

        std::lock_guard<std::mutex> lock(q_mtx);
        q.emplace([&func, in1, in2]() -> std::any { return func(in1, in2); }, task_id);
        q_cv.notify_one();

        return task_id;
    }

    ThreadPool::~ThreadPool() {
        stop = true;
        q_cv.notify_all();
        for(auto & thread : threads){
            thread.join();
        }
    }
}