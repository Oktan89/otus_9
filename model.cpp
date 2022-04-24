#include "model.h"
#include "state.h"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>

constexpr char protocol::eof[];
constexpr char protocol::start[];
constexpr char protocol::end[];


Model::Model(std::size_t bulk)
{

    if (bulk >= 1)
    {
        _batch_size = bulk;
        _batch.reserve(bulk);
    }
    else
    {
        std::cout << "Invalid size bulk, must be greater than: >=1\n" << std::endl;                  
        exit(EXITFAILURE);
    }
}

void Model::start()
{
    std::unique_ptr<__BaseState> state_static = std::make_unique<StaticBlock>();
    TransitionTo(state_static);

}

bool Model::__isdigit(char ch)
{
    return std::isdigit(static_cast<unsigned char>(ch));
}

std::size_t Model::get_batch_size() const noexcept
{
    return _batch_size;
}

void Model::TransitionTo(std::unique_ptr<__BaseState>& state)
{
    _state = std::move(state);
    _state->setModelContext(this);
}

void Model::send(const char *data, std::size_t size)
{
    //  std::lock_guard<std::mutex> lg(mt);
    for(std::size_t i = 0; i < size; ++i)
    {
        
        if(data[i] != '\n')
        {
            buf_send.push_back(data[i]);
        }
        else if(data[i] == '\n')
        {
            push(buf_send);
            buf_send.clear();
        }
    }
}

void Model::send_end(const char* data)
{
    push(data);
    buf_send.clear();
}

void Model::push(const std::string& com)
{
    if(com == protocol::eof)
    {
        _state->Exit();
    }
    else if(com == protocol::start)
    {
        _state->StartBlock();
    }
    else if(com == protocol::end)
    {
        _state->EndBlock();
    }
    else
    {
        _batch.emplace_back(com);
        _state->ReadBlock();
    }
   
}

void Model::notify()
{
    std::vector<std::string>& data = getData();
    
    for(auto it = data.cbegin(); it != data.cend(); ++it)
    {
        if(it != data.begin())
        {
            std::cout << ", ";
        }
        else
        {
            std::cout << "bulk: ";
        }
        std::cout << *it;
    }
        
    if(data.size() != 0) 
        std::cout << std::endl;
}

bool Model::getStatus() const noexcept
{
    return status;
}

std::vector<std::string>& Model::getData()
{
    return _batch;
}

void Model::setTimeStartBlock()
{
    _t = std::chrono::system_clock::now();
}

Model::model_time Model::getTimeStartBlock() const noexcept
{
    return _t;
}

Model::model_time Model::getTime()
{
    return std::chrono::system_clock::now();
}