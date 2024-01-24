#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace Pip2 {
    int task_create(std::uint32_t addr, int p0, int p1, int p2);
    void task_dispose(int task_id);
    int task_receive();
    int task_receive_any(int task_id);
    void task_send(int task_id, int message);
    void task_set_stack_size(std::uint32_t stack_size);
    int task_alive(int task_id);
    int task_this();
    void task_yield();
    void task_kill_current();

    enum SpecialPoolFunction {
        CREATE_TASK = 0,
        DISPOSE_TASK = 1,
        RECEIVE = 2,
        RECEIVE_ANY = 3,
        SEND = 4,
        SET_STACK_SIZE = 5,
        TASK_ALIVE = 6,
        THIS_TASK = 7,
        YIELD_TASK = 8,
        KILL_CURRENT_TASK = 9
    };

    struct SpecialPoolFunctionInfo {
        std::string name_;
        std::uint32_t arg_count_;
        bool has_return_value_;
        void *func_ptr_;
    };

    extern const std::map<SpecialPoolFunction, SpecialPoolFunctionInfo> SPECIAL_POOL_FUNCTION_INFOS;
}