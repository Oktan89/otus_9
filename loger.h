#pragma once
#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <ctime>
#include <chrono>


class LogerBulk
{

    using bulk_t = std::vector<std::string>;
    using queue_t = std::queue<bulk_t>;
    using bulk_time = std::chrono::time_point<std::chrono::high_resolution_clock>;

    std::condition_variable m_cv;
    std::condition_variable m_cv_file;
    queue_t m_queue_cli;
    queue_t m_queue_file;
    std::vector<std::thread> m_pool;
    std::mutex m_mutex;
    std::mutex m_mutex_file;
    std::atomic_bool m_stop{false};
    std::atomic_bool m_stop_file{false};

public:
    
    LogerBulk(const LogerBulk& other) = delete;
    LogerBulk(LogerBulk&& other) = delete;
    LogerBulk& operator=(const LogerBulk& oher) = delete;
    LogerBulk& operator=(LogerBulk&& oher) = delete;
    static LogerBulk& get()
    {
        static LogerBulk log;
        return log;
    }
    void push(const bulk_t& bulk);

private:
    void log_cli();
    
    void log_file();

    bulk_time getTime() const;

    LogerBulk();
    ~LogerBulk();
};

