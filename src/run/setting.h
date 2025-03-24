#ifndef SETTING_H
#define SETTING_H

#include "../tools/getArgs.hpp"

class Setting {
public:
    int T;
    int T_exact;
    bool is_fista;
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