#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <atomic>
#include "interface.h"

struct protocol
{
    static constexpr char eof[] = "EOF";
    static constexpr char start[] = "{";
    static constexpr char end[] = "}";
};


class Model 
{
public:
    using model_time = std::chrono::time_point<std::chrono::high_resolution_clock>;

private:
    std::vector<std::string> _batch;
    std::unique_ptr<__BaseState> _state;
    std::string buf_send;
    std::size_t _batch_size = 0;
    std::atomic<bool> status = true;
    model_time _t;
    friend class StaticBlock;
    friend class DinamicBlock;
public:
    Model(std::size_t bulk);

    void start();

    void push(const std::string& com);

    void send(const char *data, std::size_t size);

    void send(const char* data);

private:
    void notify();

    std::size_t get_batch_size() const noexcept;

    std::vector<std::string>& getData();

    void TransitionTo(std::unique_ptr<__BaseState>& state);

    void setTimeStartBlock();

    model_time getTimeStartBlock() const noexcept;

    model_time getTime();

    bool getStatus() const noexcept;
    
    void setStatus(bool stat) { status = stat;} 

    bool __isdigit(char ch);

};

