#pragma once

#include <sys/times.h>
#include <chrono>

class time_point;
class duration;

class time_point
{
private:
    tms _tp;
    clock_t _rt;
public:
    time_point() = delete;
    time_point(tms tp, clock_t rt);
    time_point(const time_point& other) = default;
    time_point(time_point&& other) = default;
public:
    time_point& operator=(const time_point& other) = default;
    time_point& operator=(time_point&& other) = default;
public:
    duration operator-(const time_point& other) const;
public:
    static time_point get_time();
};

class duration
{
private:
    tms _btp;
    tms _etp;
    clock_t _brt;
    clock_t _ert;
public:
    duration() = delete;
    duration(tms b, clock_t brt, tms e, clock_t ert);
    duration(const duration& other) = default;
    duration(duration&& other) = default;
public:
    duration& operator=(const duration& other) = default;
    duration& operator=(duration&& other) = default;
public:
    size_t user_time() const;
    size_t system_time() const;
    size_t total_user_time() const;
    size_t total_system_time() const;
    size_t total_real_time() const;
};