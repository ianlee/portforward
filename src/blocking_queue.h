#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <cstddef>
#include <chrono>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
/** 
thread safe blocking queue. perfect for worker threads in a thread pool.
blocking queue pulled from:
http://codereview.stackexchange.com/questions/39199/multi-producer-consumer-queue-without-boost-in-c11?rq=1
*/
template<typename T>
class blocking_queue {
public:
    typedef std::queue<T> queue_t;
    typedef typename queue_t::size_type  size_type;
    blocking_queue(){_max_size=100;}
    explicit blocking_queue(std::size_t max_size) :_max_size(max_size), _q()
    {}
    blocking_queue(const blocking_queue&) = delete;
    blocking_queue& operator=(const blocking_queue&) = delete;
    ~blocking_queue() = default;

    size_type size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _q.size();
    }
    //Return false if failed to push due to full queue after the timeout have passed
    bool push(const T& item, const std::chrono::milliseconds& timeout)
    {
        auto wait_until = std::chrono::system_clock::now() + timeout;
        {            
            std::unique_lock<std::mutex> ul(_mutex);
            if (!_item_popped_cond.wait_until(ul, wait_until, [this]() { return this->_q.size() < this->_max_size; }))
                return false;
            _q.push(item);
        }        
       _item_pushed_cond.notify_one();
        return true;
    }
    //Return false if failed to pop due to empty queue after the timeout have passed
    bool pop(T& item, const std::chrono::milliseconds& timeout)
    {
        auto wait_until = std::chrono::system_clock::now() + timeout;
        {            
            std::unique_lock<std::mutex> ul(_mutex);
            if (!_item_pushed_cond.wait_until(ul, wait_until, [this]() { return !this->_q.empty(); }))
                return false;

            item = _q.front();
            _q.pop();
        }        
        _item_popped_cond.notify_one();
        return true;
    }
private:
    size_type _max_size;
    std::queue<T> _q;
    std::mutex _mutex;
    std::condition_variable _item_pushed_cond;
    std::condition_variable _item_popped_cond;
};

#endif
