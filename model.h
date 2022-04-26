#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include "interface.h"
#include "loger.h"

struct protocol
{
    static constexpr char eof[] = "EOF";
    static constexpr char start[] = "{";
    static constexpr char end[] = "}";
};


class Model 
{
    std::vector<std::string> _batch;
    std::unique_ptr<__BaseState> _state;
    std::size_t _batch_size = 0;
    std::atomic<bool> status = true;
    LogerBulk &log;
    friend class StaticBlock;
    friend class DinamicBlock;
    ///Active object for processing
    std::string buf_send;
    std::condition_variable m_cv_processing;
	std::queue<std::pair<const char*, std::size_t>> m_tasks_buff;
	std::mutex m_mutex;
	std::thread m_thread;
	std::atomic<bool> m_stopped;
 
public:
    Model(std::size_t bulk);

    void start();

    void send(const char *data, std::size_t size);

    void send_end();

private:
    void notify();

    void bulk_processing();

    void push(const std::string& com);

    void push_buf(const char *data, std::size_t size);

    std::size_t get_batch_size() const noexcept;

    std::vector<std::string>& getData();

    void TransitionTo(std::unique_ptr<__BaseState>& state);

    bool __isdigit(char ch);

};

