#include "TaskHandler.h"
#include "VMEngine.h"
#include <fstream>

namespace Pip2 {
    static constexpr std::size_t HOST_STACK_SIZE = 0x100000;

    static void task_execute_entry_point() {
        TaskHandler *task_handler = engine_instance->task_handler();
        task_handler->execute_entry_point_current_task();
    }

    TaskHandler::TaskHandler(VMEngine *engine, TaskExecuteEntryFunc execute_entry_func, Common::TaskStackCreateFunc stack_create_func,
                             Common::TaskStackFreeFunc stack_free_func)
        : stack_create_func_(stack_create_func)
        , stack_free_func_(stack_free_func)
        , execute_entry_func_(execute_entry_func)
        , stack_size_(-1)
        , current_task_id_(-1)
        , engine_(engine) {
    }

    void TaskHandler::execute_entry_point_current_task() {
        execute_entry_func_(*tasks_[current_task_id_ - 1]);
        current_task_finished();
    }

    int TaskHandler::push_task(std::unique_ptr<TaskData> &task_data) {
        for (auto i = 0; i < tasks_.size(); i++) {
            if (tasks_[i] == nullptr) {
                tasks_[i] = std::move(task_data);
                tasks_[i]->id_ = static_cast<int>(i + 1);

                return i + 1;
            }
        }

        tasks_.push_back(std::move(task_data));
        tasks_.back()->id_ = static_cast<int>(tasks_.size());

        return static_cast<int>(tasks_.size());
    }

    int TaskHandler::create_task(std::uint32_t func_addr, int p0, int p1, int p2) {
        std::unique_ptr<TaskData> task_data = std::make_unique<TaskData>();

        task_data->context_.regs_[Register::P0 >> 2] = p0;
        task_data->context_.regs_[Register::P1 >> 2] = p1;
        task_data->context_.regs_[Register::P2 >> 2] = p2;

        // Create stack
        task_data->stack_addr_ = stack_create_func_ ? stack_create_func_(engine_->userdata(), stack_size_) : 0;
        task_data->context_.regs_[Register::SP >> 2] = task_data->stack_addr_;
        task_data->handle_ = co_create(HOST_STACK_SIZE, task_execute_entry_point);
        task_data->entry_point_ = func_addr;

        schedule_task(task_data.get());
        return push_task(task_data);
    }

    void TaskHandler::run_entry_point_task() {
        main_handle_ = co_active();

        std::unique_ptr<TaskData> task_data = std::make_unique<TaskData>();

        task_data->context_ = entry_point_context_;
        task_data->handle_ = co_create(HOST_STACK_SIZE, task_execute_entry_point);
        task_data->entry_point_ = 0;
        task_data->stack_addr_ = 0;

        TaskData *task_data_ptr = task_data.get();
        push_task(task_data);

        schedule_task(task_data_ptr);
        switch_to_next_task();
    }

    void TaskHandler::dispose_task(int task_id) {
        if (task_id <= 0 || task_id > tasks_.size()) {
            throw std::runtime_error("Invalid task ID to dispose!");
        }

        TaskData *task_data_ptr = tasks_[task_id - 1].get();
        if (task_data_ptr->stack_addr_ != 0 && stack_free_func_) {
            stack_free_func_(engine_->userdata(), task_data_ptr->stack_addr_);
        }

        tasks_[task_id - 1] = nullptr;

        unschedule_task(task_data_ptr);
    }

    int TaskHandler::receive(int from_task) {
        if (from_task == TASK_CURRENT) {
            return tasks_[current_task_id_ - 1]->received_data_;
        } else {
            if (from_task <= 0 || from_task > tasks_.size()) {
                return 0;
            } else {
                return tasks_[from_task - 1]->received_data_;
            }
        }
    }

    void TaskHandler::send(int to_task, int data) {
        auto &task = tasks_[to_task - 1];
        if (task == nullptr) {
            throw std::runtime_error("Invalid task ID to send!");
        }
        task->received_data_ = data;
    }

    int TaskHandler::task_valid(int task_id) const {
        return task_id > 0 && task_id <= tasks_.size() && tasks_[task_id - 1] != nullptr;
    }

    int TaskHandler::current_task() const {
        return current_task_id_;
    }

    void TaskHandler::yield_current() {
        schedule_task(tasks_[current_task_id_ - 1].get());
        switch_to_next_task();
    }

    void TaskHandler::kill_current() {
        dispose_task(current_task_id_);
        switch_to_next_task();
    }

    VMContext &TaskHandler::current_task_context() {
        if (current_task_id_ < 0) {
            return entry_point_context_;
        }

        return tasks_[current_task_id_ - 1]->context_;
    }

    void TaskHandler::schedule_task(Pip2::TaskData *task_data) {
        tasks_queue_.push_back(task_data);
    }

    void TaskHandler::switch_to_next_task() {
        if (tasks_queue_.empty()) {
            // Return to main execution
            current_task_id_ = -1;
            co_switch(main_handle_);

            return;
        } else {
            auto next_task = tasks_queue_.front();
            tasks_queue_.erase(tasks_queue_.begin());

            current_task_id_ = next_task->id_;
            co_switch(next_task->handle_);
        }
    }

    void TaskHandler::unschedule_task(TaskData *task_data) {
        auto it = std::find(tasks_queue_.begin(), tasks_queue_.end(), task_data);
        if (it != tasks_queue_.end()) {
            tasks_queue_.erase(it);
        }
    }

    void TaskHandler::current_task_finished() {
        dispose_task(current_task_id_);
        switch_to_next_task();
    }
}
