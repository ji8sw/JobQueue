#include "JobQueue.h"
#include <iostream>
#include <thread>
#include <format>

int main()
{
    JobQueue NewQueue = JobQueue();

    NewQueue.Push([&] { std::this_thread::sleep_for(std::chrono::seconds(1)); });
    NewQueue.Push([&] { std::this_thread::sleep_for(std::chrono::seconds(2)); });
    NewQueue.Push([&] { std::this_thread::sleep_for(std::chrono::seconds(3)); });
    NewQueue.Push([&] { std::this_thread::sleep_for(std::chrono::seconds(4)); });

    NewQueue.Start();

    while (NewQueue.GetRemainingJobs() != 0)
    {
        system("cls");
        std::cout << std::format("Running jobs {}/4...", 4 - NewQueue.GetRemainingJobs()) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    NewQueue.Pause();

    system("cls");
    std::cout << "Completed all jobs..." << std::endl;

    return 0;
}
