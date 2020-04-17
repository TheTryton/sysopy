#pragma once

// system V

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// std

#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <utility>
#include <tuple>
#include <string>
#include <string_view>
#include <optional>
#include <algorithm>
#include <signal.h>

using namespace std;

using byte = char;
using byte_buffer = vector<char>;

using byte_buffer_iterator = byte_buffer::iterator;
using byte_buffer_const_iterator = byte_buffer::const_iterator;

using client_id_t = size_t;

class imessage;
class imessage_queue;
class imessage_deserializer;

template<class T>
class serialization;


