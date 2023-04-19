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

using task_id_t = unsigned long;
using in_t = arg_t; // input argument type
using ret_t = arg_t; // return type
using input_task_t = std::function<ret_t(arg_t, arg_t)>; // function type on the input which gets converted to:
using task_t = std::function<ret_t()>; // function type which is being stored
using task_q_t = std::pair<task_id_t, task_t>;
using result_q_t = std::pair<task_id_t, arg_t>;

class ThreadPool {
private:
    // threads
    std::thread **threads;
    size_t poolSize;

    // tasks queue
    std::queue<task_q_t> q; // task, task_id
    std::mutex qMtx;
    std::condition_variable qCv;

    // stop flag
    std::atomic<bool> stop = false;

    // result id variable
    std::atomic<task_id_t> newTaskId;

    // results queue
    std::queue<result_q_t> resultsQ;
    std::mutex resultsMtx;
    std::condition_variable resultsCv;

    // run thread
    void run();

public:
    explicit ThreadPool(size_t poolSize_);

    ~ThreadPool();

    task_id_t addTask(const input_task_t &func, in_t in1, in_t in2);
    result_q_t popResult();
};

#endif //THREADPOOL_THREADPOOL_H
