#pragma once

#include <libco.h>
#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>
#include <queue>
#include <stack>

#include "Common.h"
#include "VMContext.h"

namespace Pip2 {
    struct TaskData;

    using TaskExecuteEntryFunc = std::function<void(TaskData&)>;

    static constexpr int TASK_CURRENT = -1;
    static constexpr int ENTRY_POINT_TASK = 1;

    class VMEngine;

    struct TaskData {
        cothread_t handle_;
        std::uint32_t entry_point_;
        std::uint32_t stack_addr_ = 0;

        int received_data_ = 0;
        int id_ = 0;

        VMContext context_;
    };

    class TaskHandler {
    private:
        std::vector<std::unique_ptr<TaskData>> tasks_;
        std::vector<TaskData*> tasks_queue_;

        Common::TaskStackCreateFunc stack_create_func_;
        Common::TaskStackFreeFunc stack_free_func_;

        TaskExecuteEntryFunc execute_entry_func_;

        std::int64_t stack_size_;

        VMContext entry_point_context_;
        int current_task_id_;

        cothread_t main_handle_;
        VMEngine *engine_;

    private:
        int push_task(std::unique_ptr<TaskData> &task_data);

    private:
        void schedule_task(TaskData *task_data);
        void unschedule_task(TaskData *task_data);
        void current_task_finished();
        void switch_to_next_task();

    public:
        explicit TaskHandler(VMEngine *engine, TaskExecuteEntryFunc execute_entry_func, Common::TaskStackCreateFunc stack_create_func,
                             Common::TaskStackFreeFunc stack_free_func);

        ~TaskHandler() = default;

        int create_task(std::uint32_t func_addr, int p0, int p1, int p2);
        void run_entry_point_task();

        void dispose_task(int task_id);

        int receive(int from_task = TASK_CURRENT);
        void send(int to_task, int data);

        std::uint32_t set_stack_size(const std::uint32_t stack_size) {
            stack_size_ = static_cast<std::int64_t>(stack_size + (((stack_size_ & 3) != 0) ? (4 - (stack_size_ & 3)) : 0));
            return stack_size_;
        }

        [[nodiscard]] int task_valid(int task_id) const;
        [[nodiscard]] int current_task() const;

        void yield_current();
        void kill_current();

        void execute_entry_point_current_task();

        VMContext &current_task_context();
    };
}