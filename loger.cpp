#include "loger.h"
#include <iostream>
#include <fstream>

LogerBulk::LogerBulk()
{
    m_pool.emplace_back(&LogerBulk::log_cli, this);
    m_pool.emplace_back(&LogerBulk::log_file, this);
    m_pool.emplace_back(&LogerBulk::log_file, this);
}


LogerBulk::~LogerBulk()
{
    m_stop = true;
    m_cv.notify_all();

    for(auto &p : m_pool)
    {
        if(p.joinable())
            p.join();
    }        
}


void LogerBulk::push(const bulk_t& bulk)
{
    std::lock_guard<std::mutex> lg(m_mutex);
        m_queue_cli.push(bulk);
    m_cv.notify_one();
}

void LogerBulk::log_cli()
{
    auto show_bulk = [](const bulk_t& bulk)
    {
        for (auto it = bulk.cbegin(); it != bulk.cend(); ++it)
        {
            if (it != bulk.begin())
            {
                std::cout << ", ";
            }
            else
            {
                std::cout << "bulk: ";
            }
            std::cout << *it;
        }

        if (bulk.size() != 0)
            std::cout << std::endl;
    };

    while(!m_stop)
    {
        std::unique_lock<std::mutex> ul(m_mutex);
        while(m_queue_cli.empty() && !m_stop)
            m_cv.wait(ul);
        
        if(m_stop)
            break;

        auto bulk = m_queue_cli.front();
        m_queue_cli.pop();
        show_bulk(bulk);
        ul.unlock();
        std::lock_guard<std::mutex> lg(m_mutex_file);
            m_queue_file.push(bulk);
        m_cv_file.notify_all();
    }

    while(!m_queue_cli.empty())
    {
        auto bulk = m_queue_cli.front();
        m_queue_cli.pop();
        show_bulk(bulk);
        std::lock_guard<std::mutex> lg(m_mutex_file);
            m_queue_file.push(bulk);
        m_cv_file.notify_all();
    }
    m_stop_file = true;
    m_cv_file.notify_all();
}


void LogerBulk::log_file()
{
    auto write_file = [this](const bulk_t& bulk)
    {
        bulk_time t = getTime();
        std::string id = (std::this_thread::get_id() == m_pool[1].get_id())? "1" : "2";
        std::string file_name = "bulk["  + id + "]" +
                                std::to_string(std::chrono::system_clock::to_time_t(t)) +
                                ".log";

        if (std::ifstream(file_name))
        {
            auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(getTime() - t);
            file_name.insert(file_name.find_first_of("."), "_" + std::to_string(nano.count())); 
        }
        std::ofstream file(file_name, std::ios::app);

        for (auto it = bulk.cbegin(); it != bulk.cend(); ++it)
        {
            if (it != bulk.begin())
            {
                file << ", ";
            }
            else
            {
                file << "bulk: ";
            }
            file << *it;
        }

        if (bulk.size() != 0)
            file << std::endl;
        file.close();
    };

    while(!m_stop_file)
    {
        std::unique_lock<std::mutex> ul(m_mutex_file);
        while(m_queue_file.empty() && !m_stop_file)
            m_cv_file.wait(ul);
        
        if(m_stop_file)
            break;

        auto bulk = m_queue_file.front();
        m_queue_file.pop();
        write_file(bulk);
    }

    while(!m_queue_file.empty())
    {
        std::lock_guard<std::mutex> lg(m_mutex_file);
        if(m_queue_file.empty())
            break;
        auto bulk = m_queue_file.front();
        m_queue_file.pop();
        write_file(bulk);
    }
    m_cv_file.notify_all();
}

LogerBulk::bulk_time LogerBulk::getTime() const
{
    return std::chrono::system_clock::now();
}
