#include "SpecialFunction.h"
#include "VMEngine.h"

namespace Pip2 {
    int task_create(std::uint32_t addr, int p0, int p1, int p2) {
        auto task_handler = engine_instance->task_handler();
        return task_handler->create_task(addr, p0, p1, p2);
    }

    void task_dispose(int task_id) {
        auto task_handler = engine_instance->task_handler();
        task_handler->dispose_task(task_id);
    }

    int task_receive() {
        auto task_handler = engine_instance->task_handler();
        return task_handler->receive();
    }

    int task_receive_any(int task_id) {
        auto task_handler = engine_instance->task_handler();
        return task_handler->receive(task_id);
    }

    void task_send(int task_id, int message) {
        auto task_handler = engine_instance->task_handler();
        task_handler->send(task_id, message);
    }

    void task_set_stack_size(std::uint32_t stack_size) {
        auto task_handler = engine_instance->task_handler();
        task_handler->set_stack_size(stack_size);
    }

    int task_alive(int task_id) {
        auto task_handler = engine_instance->task_handler();
        return task_handler->task_valid(task_id);
    }

    int task_this() {
        auto task_handler = engine_instance->task_handler();
        return task_handler->current_task();
    }

    void task_yield() {
        auto task_handler = engine_instance->task_handler();
        task_handler->yield_current();
    }

    void task_kill_current() {
        auto task_handler = engine_instance->task_handler();
        task_handler->kill_current();
    }

    const std::map<SpecialPoolFunction, SpecialPoolFunctionInfo> SPECIAL_POOL_FUNCTION_INFOS = {
            { SpecialPoolFunction::CREATE_TASK, { "task_create", 4, true, reinterpret_cast<void*>(&task_create) } },
            { SpecialPoolFunction::DISPOSE_TASK, { "task_dispose", 1, false, reinterpret_cast<void*>(&task_dispose) } },
            { SpecialPoolFunction::RECEIVE, { "task_receive", 0, true, reinterpret_cast<void*>(&task_receive) } },
            { SpecialPoolFunction::RECEIVE_ANY, { "task_receive_any", 1, true, reinterpret_cast<void*>(&task_receive_any) } },
            { SpecialPoolFunction::SEND, { "task_send", 2, false, reinterpret_cast<void*>(&task_send) } },
            { SpecialPoolFunction::SET_STACK_SIZE, { "task_set_stack_size", 1, false, reinterpret_cast<void*>(&task_set_stack_size) } },
            { SpecialPoolFunction::TASK_ALIVE, { "task_alive", 1, true, reinterpret_cast<void*>(&task_alive) } },
            { SpecialPoolFunction::THIS_TASK, { "task_this", 0, true, reinterpret_cast<void*>(&task_this) } },
            { SpecialPoolFunction::YIELD_TASK, { "task_yield", 0, false, reinterpret_cast<void*>(&task_yield) } },
            { SpecialPoolFunction::KILL_CURRENT_TASK, { "task_kill_current", 0, false, reinterpret_cast<void*>(&task_kill_current) } },
    };
}
