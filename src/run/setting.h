#ifndef SETTING_H
#define SETTING_H

#include "../tools/getArgs.hpp"
#include <ctime>

class Setting {
public:
    clock_t start_time;
    clock_t time_flow;
    int cnt_flow;
    
    int T;
    int T_exact;
    bool is_fista;
    bool is_fw;
    bool use_core;
    bool is_exact;
    bool is_asynchronous;
    bool is_fractional;
    std::string path_G;
    std::string path_A;
    std::string path_R;
    std::string path_G_persistent;
    std::string path_A_persistent;
    std::string path_R_persistent;

    Setting();
    Setting(const argsController &ac);
} ;

#endif