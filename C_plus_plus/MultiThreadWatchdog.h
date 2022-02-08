/**
 * @file      MultiThreadWatchdog.h
 * @author    Atakan S.
 * @brief     Generalized Watchdog Class for multi-thread programs.
 *
 * @copyright Copyright (c) 2020 Atakan SARIOGLU ~ www.atakansarioglu.com
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef _H_MultiThreadWatchdog_H_
#define _H_MultiThreadWatchdog_H_

#include <iostream>
#include <unordered_map>
#include <list>
#include <mutex>
#include <atomic>
#include <thread>
#include <exception>

class MultiThreadWatchdog
{
public:
    using Interval_t = std::chrono::duration<std::chrono::seconds>;
    using ThreadId_t = std::thread::id;
    using Time_t = std::chrono::time_point<std::chrono::system_clock>;

    MultiThreadWatchdog(double interval, size_t maxNumClients)
        : m_maxNumClients(maxNumClients), m_interval(interval)
    {   
        m_bgThread = std::thread(&MultiThreadWatchdog::bgThread, this);
    }

    bool Kick(const ThreadId_t &id = std::this_thread::get_id())
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_hashMap.count(id))
        {
            // Erase the current item.
            m_list.erase(m_hashMap[id]);
            m_hashMap.erase(id);
            --m_currentNumClients;
        }
        else if (m_currentNumClients == m_maxNumClients)
        {
            // Cannot add item.
            return false;
        }

        // Insert new item to front.
        auto expireTime = std::chrono::system_clock::now();
        expireTime += std::chrono::duration_cast<std::chrono::seconds>(m_interval);
        m_list.emplace_front(std::make_pair(id, expireTime));
        m_hashMap[id] = m_list.begin();
        ++m_currentNumClients;

        // Kicked.
        std::cout << "Kicked from: " << id << std::endl;
        return true;
    }

    bool Done(const ThreadId_t &id = std::this_thread::get_id())
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_hashMap.count(id))
        {
            // Erase current item.
            m_list.erase(m_hashMap[id]);
            m_hashMap.erase(id);
            --m_currentNumClients;

            // Removed.
            std::cout << "Thread: " << id << " is done!" << std::endl;
            return true;
        }

        // Not found.
        return false;
    }

    bool IsExpired(ThreadId_t &expiredId)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_list.size() && m_list.back().second < std::chrono::system_clock::now())
        {
            expiredId = m_list.back().first;
            return true;
        }
        return false;
    }

    auto GetNextExpireTime()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto expireTime = std::chrono::system_clock::now();
        expireTime += std::chrono::duration_cast<std::chrono::seconds>(m_interval);
        return m_list.size() ? m_list.back().second : expireTime;
    }

    ~MultiThreadWatchdog()
    {
        m_bgThreadEnable = false;
        m_bgThread.join();
    }

private:
    size_t m_maxNumClients = 0;
    size_t m_currentNumClients = 0;
    std::list<std::pair<ThreadId_t, Time_t>> m_list;
    std::unordered_map<ThreadId_t, decltype(m_list)::iterator> m_hashMap;
    std::chrono::duration<double> m_interval;
    std::mutex m_mutex;
    std::thread m_bgThread;
    std::atomic<bool> m_bgThreadEnable{true};

    void bgThread()
    {
        while (m_bgThreadEnable)
        {
            ThreadId_t expiredThread{};
            if (IsExpired(expiredThread))
            {
                // Trigger watchdog reset.
                std::cout << "Expired thread: " << expiredThread << "!" << std::endl;
				
				// TODO: Replace with respect to ypur system's requirement.
                std::terminate();
            }

            // Kick the WatchDog.
            std::cout << "Kicked System!" << std::endl;

            // Nothing has expired.
            std::this_thread::sleep_until(GetNextExpireTime());
        }
    }
};

#endif
