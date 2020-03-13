#include <diff/diff_library.hpp>

DIFF_LIBRARY_NAMESPACE_BEGIN

int is_number(char c)
{
    return c <= '9' && c >= '0';
}

int is_newline(char c)
{
    return c == '\n';
}

diff_opcode get_opcode(const string& operation)
{
    auto b = operation.begin();
    while(*b != 'a' && *b != 'c' && *b != 'd')
    {
        b++;
    }

    switch(*b)
    {
        case 'a':
            return diff_opcode::a;
        case 'c':
            return  diff_opcode::c;
        case 'd':
            return  diff_opcode::d;
    }

    return diff_opcode::a;
}

diff_result get_diff_between_file_pair(const pair<filepath, filepath>& filepair)
{
    //creating command: diff filepair.file1 filepair.file2 >> temp.txt
    size_t required_size = snprintf(nullptr, 0, "diff %s %s >> temp.txt", filepair.first.c_str(), filepair.second.c_str());
    string command = string(required_size + 1); // dont count automatically appended \0 character from snprintf (it is already inside string data)
    snprintf(command.data(), command.size(), "diff %s %s >> temp.txt", filepair.first.c_str(), filepair.second.c_str());
    // calling diff command from bash
    system(command.c_str());

    //loading temporary file data into memory

    //open temporary file
    int file_descriptor = open("temp.txt", O_RDONLY);
    if(file_descriptor)
    {

        //get the size of temporary file
        int file_size = lseek(file_descriptor, 0, SEEK_END);
        //go back to the beginning of file
        lseek(file_descriptor, 0, SEEK_SET);
        //allocate buffer of size equal to file size
        string file_buffer = string(file_size);
        //load data from file into buffer
        read(file_descriptor, file_buffer.data(), file_buffer.size());
        //close file
        close(file_descriptor);
        //remove temporary file
        system("rm temp.txt");

        // counting operations

        int operations_count = file_size > 0;
        size_t current_index = 0;
        while((current_index = file_buffer.find(current_index, is_newline)) != file_buffer.size())
        {
            if(++current_index == file_buffer.size())
            {
                break;
            }

            if(is_number(file_buffer[current_index]))
            {
                operations_count++;
            }
        }

        // split file buffer and populate diff_result structure

        diff_result dr{vector<diff_operation>(operations_count)};

        if(operations_count)
        {
            int operation_start = 0;
            int operation_end = 0;
            int operation_index = 0;
            current_index = 0;
            while((current_index = file_buffer.find(current_index, is_newline)) != file_buffer.size())
            {
                if(++current_index == file_buffer.size())
                {
                    break;
                }

                if(is_number(file_buffer[current_index]))
                {
                    operation_end = current_index;
                    dr.operations[operation_index].operation_description = file_buffer.substr(operation_start, operation_end - operation_start);
                    dr.operations[operation_index].operation_code = get_opcode(dr.operations[operation_index].operation_description);
                    operation_index++;
                    operation_start = current_index;
                }
            }

            operation_end = current_index;
            dr.operations[operation_index].operation_description = file_buffer.substr(operation_start, operation_end - operation_start);
            dr.operations[operation_index].operation_code = get_opcode(dr.operations[operation_index].operation_description);
        }

        return dr;
    }

    return diff_result{};
}

vector<diff_result> get_diff_between_file_pairs(const vector<pair<filepath, filepath>>& filepairsequence) {
    vector<diff_result> diff_result_table = vector<diff_result>(filepairsequence.size());

    std::transform(filepairsequence.begin(), filepairsequence.end(), diff_result_table.begin(), [](const pair<filepath , filepath >& fp){
        return get_diff_between_file_pair(fp);
    });

    return diff_result_table;
}

DIFF_LIBRARY_NAMESPACE_END
