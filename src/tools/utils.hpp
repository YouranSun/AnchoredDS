#ifndef UTILS_HPP
#define UTILS_HPP

#include "getArgs.hpp"
#include <assert.h>
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <vector>

#define eprintf( ... ) fprintf(stderr, __VA_ARGS__)
#define eputs(str) fputs(str"\n", stderr)

template<typename T> T *read(const argsController &ac, const std::string &arg_name) {
    assert(ac.exist(arg_name));
    T *arg = new T();
    arg -> readFromText(ac[arg_name].c_str());
    eprintf("FINISHED\tread from text %s\n", ac[arg_name].c_str());
    return arg;
}

template<typename T> T *read(const std::string &path) {
    T *arg = new T();
    arg -> readFromText(path.c_str());
    eprintf("FINISHED\tread from text %s\n", path.c_str());
    return arg;
}

template<typename T> void write(const T *instance, const argsController &ac, const std::string &arg_name) {
    // assert(ac.exist(arg_name));
    if (!ac.exist(arg_name)) {
        eprintf("FAILED\t-<%s> doesn't exist; writing failed\n", arg_name.c_str());
        return;
    }
    instance -> writeToText(ac[arg_name].c_str());
    eprintf("FINISHED\twrite to text %s\n", ac[arg_name].c_str());
    return;
}

template<typename T> void write(const T *instance, const std::string &path) {
    // assert(ac.exist(arg_name));
    if (path == "") {
        eprintf("FAILED\t-<%s> doesn't exist; writing failed\n", path.c_str());
        return;
    }
    instance -> writeToText(path.c_str());
    eprintf("FINISHED\twrite to text %s\n", path.c_str());
    return;
}

inline void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;
    
    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> vsize >> rss;
    }
    
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}
#endif