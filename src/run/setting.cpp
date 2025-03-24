#include "setting.h"
#include <assert.h>

Setting::Setting(){}

Setting::Setting(const argsController &ac) {
    if (ac.exist("-c")) use_core = true;
    else use_core = false;

    if (ac.exist("-T")) T = atoi(ac["-T"].c_str());
    else T = 64;

    assert(ac.exist("-d"));
    if (ac["-d"] == "FISTA") is_fista = true;
    else is_fista = false;

    if (ac.exist("-e")) is_exact = true;
    else is_exact = false;

    if (ac.exist("-Texact")) T_exact = atoi(ac["-Texact"].c_str());
    else T_exact = 10;

    if (ac.exist("-sync")) is_asynchronous = false;
    else is_asynchronous = true;

    if (ac.exist("-fp")) is_fractional = true;
    else is_fractional = false;

    assert(ac.exist("-g"));
    path_G = ac["-g"];

    assert(ac.exist("-r"));
    path_R = ac["-r"];

    assert(ac.exist("-a"));
    path_A = ac["-a"];

    if (ac.exist("-gp")) path_G_persistent = ac["-gp"];
    else path_G_persistent = "";

    if (ac.exist("-rp")) path_R_persistent = ac["-rp"];
    else path_R_persistent = "";

    if (ac.exist("-ap")) path_A_persistent = ac["-ap"];
    else path_A_persistent = "";
}