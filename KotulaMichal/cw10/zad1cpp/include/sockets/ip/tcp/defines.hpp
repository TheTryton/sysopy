#pragma once

#include <sockets/defines.hpp>

#define TCP_NAMESPACE tcp
#define TCP_NAMESPACE_FULL IP_NAMESPACE_FULL::TCP_NAMESPACE
#define TCP_NAMESPACE_BEGIN namespace TCP_NAMESPACE_FULL{
#define TCP_NAMESPACE_END }