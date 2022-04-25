#include "model.h"
#include "state.h"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <ctime>


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
    m_stopped = false;
    m_thread = std::thread(&Model::bulk_processing, this);
}

void Model::bulk_processing()
{
		while (!m_stopped) {
			std::unique_lock<std::mutex> lck{m_mutex};
			while (!m_stopped && m_tasks_buff.empty())
				m_condition.wait(lck);

			if (m_stopped)
				break;

			auto pair = m_tasks_buff.front();
			m_tasks_buff.pop();
			push_buf(pair.first, pair.second);
		}

		while(!m_tasks_buff.empty()) {
			auto pair = m_tasks_buff.front();
			m_tasks_buff.pop();
			push_buf(pair.first, pair.second);
		}

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

void Model::push_buf(const char *data, std::size_t size)
{

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

void Model::send(const char *data, std::size_t size)
{

    std::lock_guard<std::mutex> guard(m_mutex);
		m_tasks_buff.push(
			std::make_pair(
				data,
				size
			)
		);
		m_condition.notify_all();
}

void Model::send_end(const char* data)
{
    if (!m_stopped) {
			m_stopped = true;
			m_condition.notify_one();
			m_thread.join();
		}
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