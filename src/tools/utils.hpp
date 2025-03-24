#ifndef UTILS_HPP
#define UTILS_HPP

#include "getArgs.hpp"
#include <assert.h>
#include <algorithm>
#include <cstdio>
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

#endif