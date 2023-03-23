#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t poolSize_) : poolSize(poolSize_) {
    threads = new std::thread*[poolSize];
    for(int i = 0; i < poolSize; ++i){
        threads[i] = new std::thread(std::thread(&ThreadPool::run, this));
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    qCv.notify_all();
    for(int i = 0;i < poolSize;++i){
        threads[i]->join();
        delete threads[i];
    }

    delete [] threads;
}

void ThreadPool::run() {
    while(!stop){
        std::unique_lock<std::mutex> lock(qMtx);

        qCv.wait(lock, [this]() -> bool {
            return (!q.empty() || stop);
        });

        if(!q.empty()){
            auto task = std::move(q.front());
            q.pop();
            lock.unlock();

            task();
        }
    }
}

void ThreadPool::addTask(const std::function<ret_t(in_t, in_t)> &func, in_t in1, in_t in2) {
    std::lock_guard<std::mutex> lock(qMtx);
    q.emplace([func, in1, in2]() -> ret_t { return func(in1, in2); });
    qCv.notify_one();
}