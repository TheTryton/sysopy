#pragma once

#include <diff/common.hpp>

DIFF_LIBRARY_NAMESPACE_BEGIN

enum class diff_opcode
{
    c,
    a,
    d
};

struct diff_operation
{
    string operation_description;
    diff_opcode operation_code;
};

struct diff_result
{
    vector<diff_operation> operations;
};

using filepath = string;

extern "C" diff_result get_diff_between_file_pair(const pair<filepath, filepath>& filepair);
extern "C" vector<diff_result> get_diff_between_file_pairs(const vector<pair<filepath, filepath>>& filepairsequence);

DIFF_LIBRARY_NAMESPACE_END

