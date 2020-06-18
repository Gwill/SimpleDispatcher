#ifndef DISPATCHER_DISPATCHQUEUE_H
#define DISPATCHER_DISPATCHQUEUE_H

#include <functional>
#include <queue>
#include <mutex>


class DispatchQueue {
    typedef std::function<void(void)> fp_t;

public:
    explicit DispatchQueue(size_t thread_cnt = 1);
    ~DispatchQueue();

    // dispatch and copy
    void dispatch(const fp_t& op);
    // dispatch and move
    void dispatch(fp_t&& op);

    // Deleted operations
    DispatchQueue(const DispatchQueue& rhs) = delete;
    DispatchQueue(DispatchQueue&& rhs) = delete;
    DispatchQueue& operator=(const DispatchQueue& rhs) = delete;
    DispatchQueue& operator=(DispatchQueue&& rhs) = delete;

private:
    std::mutex lock_;
    std::vector<std::thread> threads_;
    std::queue<fp_t> q_;
    std::condition_variable cv_;
    bool quit_ = false;

    void dispatch_thread_handler();
};


DispatchQueue::DispatchQueue(size_t thread_cnt) : threads_(thread_cnt)
{
    for(auto & thread : threads_)
    {
        thread = std::thread(&DispatchQueue::dispatch_thread_handler, this);
    }
}

DispatchQueue::~DispatchQueue()
{
    // Signal to dispatch threads that it's time to wrap up
    std::unique_lock<std::mutex> lock(lock_);
    quit_ = true;
    lock.unlock();
    cv_.notify_all();

    // Wait for threads to finish before we exit
    for(auto & thread : threads_)
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }
}

void DispatchQueue::dispatch(const fp_t& op)
{
    std::unique_lock<std::mutex> lock(lock_);
    q_.push(op);

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    cv_.notify_one();
}

void DispatchQueue::dispatch(fp_t&& op)
{
    std::unique_lock<std::mutex> lock(lock_);
    q_.push(std::move(op));

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    cv_.notify_one();
}

void DispatchQueue::dispatch_thread_handler()
{
    std::unique_lock<std::mutex> lock(lock_);

    do {
        //Wait until we have data or a quit signal
        cv_.wait(lock, [this]{
            return (!q_.empty() || quit_);
        });

        //after wait, we own the lock
        if(!quit_ && !q_.empty())
        {
            auto op = std::move(q_.front());
            q_.pop();

            //unlock now that we're done messing with the queue
            lock.unlock();
            op();
            lock.lock();
        }
    } while (!quit_);
}

#endif //DISPATCHER_DISPATCHQUEUE_H
