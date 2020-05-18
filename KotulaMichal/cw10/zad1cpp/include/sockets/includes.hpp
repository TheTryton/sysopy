#pragma once

#include <sockets/defines.hpp>

#include <system_error>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <poll.h>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <variant>
#include <cstddef>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <functional>
#include <string>
#include <cstring>
#include <string_view>
#include <unistd.h>
#include <memory>