#include <diff/diff_library.hpp>
#include <time.hpp>
#include <dlfcn.h>
#include <assert.h>

using namespace STD_LIB_NAMESPACE_FULL;
using DIFF_LIBRARY_NAMESPACE_FULL::filepath;
using DIFF_LIBRARY_NAMESPACE_FULL::diff_result;

#ifdef DYNAMIC_LOAD_FUNC
using func_t =vector<diff_result>(*)(const vector<pair<filepath, filepath>>& filepairsequence);
static func_t _loaded_func = nullptr;
void* handle = nullptr;
void prepare()
{
    handle = dlopen(LIBRARY_NAME, RTLD_LAZY);
    assert(handle && "failed to load .so dynamically");
    _loaded_func = (func_t)dlsym(handle, "get_diff_between_file_pairs");
    assert(_loaded_func && "failed to find function symbol");
};
vector<diff_result> get_diff_between_file_pairs(const vector<pair<filepath, filepath>>& filepairsequence)
{
    return _loaded_func(filepairsequence);
}
void cleanup()
{
    _loaded_func = nullptr;
    dlclose(handle);
};
#else
void prepare(){};
void cleanup(){};
vector<diff_result> get_diff_between_file_pairs(const vector<pair<filepath, filepath>>& filepairsequence)
{
    return DIFF_LIBRARY_NAMESPACE_FULL::get_diff_between_file_pairs(filepairsequence);
}
#endif

void log_test_name()
{
    size_t required_size = snprintf(nullptr, 0, "echo \"Test: %s\" >> %s", TEST_NAME, RESULT_FILE);
    string command = string(required_size + 1); // dont count automatically appended \0 character from snprintf (it is already inside string data)
    snprintf(command.data(), command.size(), "echo \"Test: %s\" >> %s", TEST_NAME, RESULT_FILE);
    system(command.c_str());
}
void log_time_to_result_file(duration dur)
{
    auto clock_to_sec = float(sysconf(_SC_CLK_TCK));

    size_t required_size = snprintf(nullptr, 0, "echo \"Real time: %fs, User time: %fs, System time: %fs\" >> %s",
                                    dur.total_real_time() / clock_to_sec,
                                    dur.user_time() / clock_to_sec,
                                    dur.system_time() / clock_to_sec,
                                    RESULT_FILE
            );
    string command = string(required_size + 1); // dont count automatically appended \0 character from snprintf (it is already inside string data)
    snprintf(command.data(), command.size(), "echo \"Real time: %fs, User time: %fs, System time: %fs\" >> %s",
             dur.total_real_time() / clock_to_sec,
             dur.user_time() / clock_to_sec,
             dur.system_time() / clock_to_sec,
             RESULT_FILE
             );
    system(command.c_str());
}

void test_compare(const vector<pair<string, string>>& pairs)
{
    auto start = time_point::get_time();
    volatile auto diff_results = get_diff_between_file_pairs(pairs); // prevent g++ from optimizing out this statement
    auto end = time_point::get_time();

    log_time_to_result_file(end - start);
}

void test_delete_block()
{
    auto diff_results = get_diff_between_file_pairs({{"../test/small/a.txt", "../test/small/c.txt"},
                                                             {"../test/small/b.txt", "../test/small/d.txt"},
                                                             {"../test/medium/a.txt", "../test/medium/c.txt"},
                                                             {"../test/medium/b.txt", "../test/medium/d.txt"},
                                                             {"../test/big/a.txt", "../test/big/c.txt"},
                                                             {"../test/big/b.txt", "../test/big/d.txt"}});
    while(diff_results.size())
    {
        auto start = time_point::get_time();
        diff_results.erase(0);
        auto end = time_point::get_time();

        log_time_to_result_file(end - start);
    }
}

void test_add_delete_block() {
    vector<diff_result> diff_results; // prevent g++ from optimizing out this statement


    auto start = time_point::get_time();
    for(size_t i=0;i<6;i++)
    {

        diff_results = get_diff_between_file_pairs({{"../test/small/a.txt", "../test/small/c.txt"},
                                                         {"../test/small/b.txt", "../test/small/d.txt"},
                                                         {"../test/medium/a.txt", "../test/medium/c.txt"},
                                                         {"../test/medium/b.txt", "../test/medium/d.txt"},
                                                         {"../test/big/a.txt", "../test/big/c.txt"},
                                                         {"../test/big/b.txt", "../test/big/d.txt"}});
    }
    auto end = time_point::get_time();

    log_time_to_result_file(end - start);
}

int main(int argc, char** argv)
{
    prepare();
    vector<string> arguments = vector<string>(argc);
    std::transform(argv, argv + argc, arguments.begin(), [](char* arg){
        return string(arg);
    });

    if(arguments.size() == 3)
    {
        size_t table_size = atoi(argv[1]);
        size_t jobs_count = atoi(argv[2]);

        vector<diff_result> main_table = vector<diff_result>();

        for(size_t i=0;i<jobs_count;i++)
        {
            string command = string(256);
            scanf("%s", command.data());
            command.resize(strlen(command.c_str()));

            if(command == "create_table")
            {
                size_t new_size;
                scanf("%lu", &new_size);
                main_table.resize(new_size);
                table_size = new_size;
            }
            else if(command == "compare_pairs")
            {
                vector<pair<string, string>> pairs = vector<pair<string, string>>(table_size);
                for(size_t i=0;i<table_size;i++)
                {
                    string file_pair = string(256);
                    scanf("%s", file_pair.data());
                    file_pair.resize(strlen(file_pair.c_str()));
                    auto fp = file_pair.split(':');
                    pairs[i] = pair<string, string>{fp[0], fp[1]};
                }
                main_table = get_diff_between_file_pairs(pairs);
            }
            else if(command == "remove_block")
            {
                size_t index;
                scanf("%lu", &index);
                main_table.erase(index);
            }
            else if(command == "remove_operation")
            {
                size_t block_index;
                size_t operation_index;
                scanf("%lu %lu", &block_index, &operation_index);
                main_table[block_index].operations.erase(operation_index);
            }
        }
    }
    else if(arguments.size() == 2 && arguments.back() == "test")
    {
        log_test_name();

        test_compare( {
                                   {"../test/small/a.txt", "../test/small/b.txt"},
                                   {"../test/small/c.txt", "../test/small/d.txt"},
                           }); //small similar
        test_compare( {
                              {"../test/small/a.txt", "../test/small/c.txt"},
                              {"../test/small/b.txt", "../test/small/d.txt"},
                      }); //small different
        test_compare( {
                              {"../test/medium/a.txt", "../test/medium/b.txt"},
                              {"../test/medium/c.txt", "../test/medium/d.txt"},
                      }); //medium similar
        test_compare( {
                              {"../test/medium/a.txt", "../test/medium/c.txt"},
                              {"../test/medium/b.txt", "../test/medium/d.txt"},
                      }); //medium different
        test_compare( {
                              {"../test/big/a.txt", "../test/big/b.txt"},
                              {"../test/big/c.txt", "../test/big/d.txt"},
                      }); //big similar
        test_compare( {
                              {"../test/big/a.txt", "../test/big/c.txt"},
                              {"../test/big/b.txt", "../test/big/d.txt"},
                      }); //big different
        test_compare( {
                              {"../test/small/a.txt", "../test/small/c.txt"},
                              {"../test/small/b.txt", "../test/small/d.txt"},
                      }); //small count
        test_compare( {
                              {"../test/small/a.txt", "../test/small/c.txt"},
                              {"../test/small/b.txt", "../test/small/d.txt"},
                              {"../test/medium/a.txt", "../test/medium/c.txt"},
                              {"../test/medium/b.txt", "../test/medium/d.txt"},
                      }); //medium count
        test_compare( {
                              {"../test/small/a.txt", "../test/small/c.txt"},
                              {"../test/small/b.txt", "../test/small/d.txt"},
                              {"../test/medium/a.txt", "../test/medium/c.txt"},
                              {"../test/medium/b.txt", "../test/medium/d.txt"},
                              {"../test/big/a.txt", "../test/big/c.txt"},
                              {"../test/big/b.txt", "../test/big/d.txt"},
                      }); //big count
        test_delete_block(); // block removal
        test_add_delete_block(); // add delete several times
    } else{
        printf("Invalid argument count: diff_app <table_elements_count> <jobs_count>");
    }

    cleanup();
    return 0;
}