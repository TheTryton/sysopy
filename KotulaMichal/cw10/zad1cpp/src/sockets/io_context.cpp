#include <sockets/io_context.hpp>

SOCKETS_NAMESPACE_BEGIN

std::function<void()> io_context::pop() {
    unique_lock sl{_completion_queue_mutex};
    if(_completion_queue.empty())
    {
        _completion_queue_notempty.wait(sl, [this](){return !_completion_queue.empty(); });
    }

    if(_should_stop)
    {
        return function<void()>();
    } else{
        auto front = std::move(_completion_queue.front());
        _completion_queue.pop();
        return front;
    }
}

error_code io_context::run() {
    while(!_should_stop)
    {
        auto ch = pop();

        if(_should_stop)
        {
            return error_code();
        }

        ch();
    }

    return error_code();
}

void io_context::stop() {
    unique_lock sl{_completion_queue_mutex};
    _should_stop=true;
    _completion_queue_notempty.notify_all();
}

SOCKETS_NAMESPACE_END