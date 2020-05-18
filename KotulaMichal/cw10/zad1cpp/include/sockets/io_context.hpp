#pragma once

#include <sockets/common.hpp>

SOCKETS_NAMESPACE_BEGIN

class io_context
{
private:
    queue<function<void()>> _completion_queue;
    mutex _completion_queue_mutex;
    condition_variable _completion_queue_notempty;
    atomic<bool> _should_stop = false;
public:
    template<class F>
    void post(F&& completion_handler)
    {
        unique_lock sl{_completion_queue_mutex};
        bool was_empty = _completion_queue.empty();
        _completion_queue.emplace(std::move(completion_handler));
        if(was_empty)
        {
            _completion_queue_notempty.notify_one();
        }
    }
private:
    function<void()> pop();
public:
    error_code run();
    void stop();
};

SOCKETS_NAMESPACE_END