#include <time.hpp>

time_point::time_point(tms tp, clock_t rt) : _tp(tp), _rt(rt){}

duration time_point::operator-(const time_point &other) const {
    return duration(other._tp, other._rt, _tp, _rt);
}

time_point time_point::get_time() {
    tms tp;
    auto rt = times(&tp);
    return time_point(tp, rt);
}

duration::duration(tms b, clock_t brt, tms e, clock_t ert) : _btp(b), _etp(e), _brt(brt), _ert(ert) {}

size_t duration::user_time() const {
    return _etp.tms_utime - _btp.tms_utime;
}

size_t duration::system_time() const {
    return _etp.tms_stime - _btp.tms_stime;
}

size_t duration::total_user_time() const {
    return _etp.tms_cutime - _btp.tms_cutime;
}

size_t duration::total_system_time() const {
    return _etp.tms_cstime - _btp.tms_cstime;
}

size_t duration::total_real_time() const {
    return _ert - _brt;
}
