#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class JobQueue
{
private:
    std::vector<std::function<void()>> Queue;
    bool Ticking = false;
    std::mutex Mutex;
    std::condition_variable Condition;
    std::thread WorkerThread;

    void Tick()
    {
        while (IsTicking())
        {
            std::function<void()> Job;
            {
                std::unique_lock<std::mutex> lock(Mutex);
                Condition.wait(lock, [this] { return !Queue.empty() || !Ticking; });

                if (!Ticking) return;

                if (!Queue.empty())
                {
                    Job = Queue.front();
                    Queue.erase(Queue.begin());
                }
            }

            if (Job) Job();
        }
    }

public:
    JobQueue() = default;
    ~JobQueue()
    {
        Pause();
        if (WorkerThread.joinable())
            WorkerThread.join();
    }

    bool IsTicking()
    {
        return Ticking;
    }

    void Pause()
    {
        {
            std::unique_lock<std::mutex> lock(Mutex);
            Ticking = false;
        }
        Condition.notify_all();
    }

    void Start()
    {
        {
            std::unique_lock<std::mutex> lock(Mutex);
            Ticking = true;
        }
        Condition.notify_all();

        if (!WorkerThread.joinable())
        {
            WorkerThread = std::thread(&JobQueue::Tick, this);
        }
    }

    void Push(std::function<void()> NewJob)
    {
        {
            std::unique_lock<std::mutex> lock(Mutex);
            Queue.push_back(NewJob);
        }
        Condition.notify_one();
    }

    int GetRemainingJobs()
    {
        return Queue.size();
    }
};