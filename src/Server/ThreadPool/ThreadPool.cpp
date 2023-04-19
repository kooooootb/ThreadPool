#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t poolSize_) : poolSize(poolSize_) , newTaskId(0) {
    threads = new std::thread*[poolSize];
    for(int i = 0; i < poolSize; ++i){
        threads[i] = new std::thread(&ThreadPool::run, this);
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    qCv.notify_all();
    resultsCv.notify_all();
    for(int i = 0;i < poolSize;++i){
        threads[i]->join();
        delete threads[i];
    }

    delete [] threads;
}

void ThreadPool::run() {
    while(!stop){
        std::unique_lock<std::mutex> tasksLock(qMtx);

        qCv.wait(tasksLock, [this]() -> bool {
            return (!q.empty() || stop);
        });

        if(!q.empty()){
            auto taskEntry = std::move(q.front());
            q.pop();
            tasksLock.unlock();

            auto result = taskEntry.second();
            std::lock_guard<std::mutex> resultsLock(resultsMtx);
            resultsQ.emplace(taskEntry.first, result);
        }
        resultsCv.notify_all();
    }
}

task_id_t ThreadPool::addTask(const std::function<ret_t(in_t, in_t)> &func, in_t in1, in_t in2) {
    std::lock_guard<std::mutex> lock(qMtx);

    task_id_t id = newTaskId++;
    q.emplace(id, [func, in1, in2]() -> ret_t { return func(in1, in2); });
    qCv.notify_one();

    return id;
}

result_q_t ThreadPool::popResult() {
    std::unique_lock<std::mutex> resultsLock(resultsMtx);

    resultsCv.wait(resultsLock, [this]() -> bool {
        return (!resultsQ.empty() || stop);
    });

    if(!resultsQ.empty()){
        result_q_t returning = resultsQ.front();
        resultsQ.pop();
        resultsLock.unlock();

        return returning;
    } else{
        throw std::runtime_error("thread pool stopped");
    }
}
