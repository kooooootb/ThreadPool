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

#include "Common.h"

using task_id_t = unsigned long; // unused
using in_t = arg_t; // input argument type
using ret_t = void; // return type
using input_task_t = std::function<ret_t(arg_t, arg_t)>; // function type on the input which gets converted to:
using task_t = std::function<ret_t()>; // function type which is being stored

class ThreadPool {
private:
    // threads
    std::thread **threads;
    size_t poolSize;

    // tasks queue
    std::queue<task_t> q; // task, task_id
    std::mutex qMtx;
    std::condition_variable qCv;

    // stop flag
    std::atomic<bool> stop = false;

    // run thread
    void run();

public:
    explicit ThreadPool(size_t poolSize_);

    ~ThreadPool();

    void addTask(const input_task_t &func, in_t in1, in_t in2);
};

#endif //THREADPOOL_THREADPOOL_H
