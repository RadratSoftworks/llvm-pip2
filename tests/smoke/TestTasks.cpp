#include "VMEngine.h"

#include <cstring>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

std::vector<std::uint8_t> memory;
std::chrono::time_point<std::chrono::steady_clock> start_time;
std::chrono::time_point<std::chrono::steady_clock> previous_swap_time;
std::uint32_t current_memory_top = 0;
int stack_instance_count = 0;

Pip2::VMEngine *current_engine;

static constexpr std::size_t DEFAULT_STACK_SIZE = 0x4000;
static constexpr std::size_t MAX_STACK_INSTANCE = 20;
static constexpr int FPS = 30;

int vmgp_get_button_data() {
    if (_kbhit()) {
        return 0x10;
    } else {
        return 0;
    }
}

int vmgp_get_tick_count() {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
        - start_time).count());
}

void vmgp_clear_scr() {
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}

void vmgp_flip_scr() {
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
        - previous_swap_time).count();

    if (delta < 1000 / FPS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS - delta));
    }
}

void vmgp_print(int mode, int x, int y, std::uint32_t str_addr) {
    auto str_ptr = reinterpret_cast<char *>(memory.data() + str_addr);
    std::cout << str_ptr << std::endl;
}

void vmgp_str_copy(std::uint32_t dst_addr, std::uint32_t src_addr) {
    auto dst_ptr = reinterpret_cast<char *>(memory.data() + dst_addr);
    auto src_ptr = reinterpret_cast<char *>(memory.data() + src_addr);

    std::strcpy(dst_ptr, src_ptr);
}

void hle_handler(void *userdata, int num) {
    auto &context = current_engine->context();

    switch (num) {
        case 1:
            vmgp_clear_scr();
            break;

        case 4:
            vmgp_flip_scr();
            break;

        case 5:
            context.regs_[Pip2::Register::R0 >> 2] = static_cast<std::uint32_t>(vmgp_get_button_data());
            break;

        case 6:
            context.regs_[Pip2::Register::R0 >> 2] = static_cast<std::uint32_t>(vmgp_get_tick_count());
            break;

        case 7:
            vmgp_print(static_cast<int>(context.regs_[Pip2::Register::P0 >> 2]),
                       static_cast<int>(context.regs_[Pip2::Register::P1 >> 2]),
                       static_cast<int>(context.regs_[Pip2::Register::P2 >> 2]),
                       context.regs_[Pip2::Register::P3 >> 2]);
            break;

        case 12:
            vmgp_str_copy(context.regs_[Pip2::Register::P0 >> 2],
                          context.regs_[Pip2::Register::P1 >> 2]);
            break;

        default:
            break;
    }
}

/**
 * @brief Allocate emulated stack.
 *
 * This function expands the memory vector and return the stack top. The stack top
 * should be at the end of the vector, and aligned by 4.
 *
 * @param size The size of the new stack. If the size is negative, the default stack size will be used.
 * @return The stack top address
 */
std::uint32_t allocate_stack(void*, std::int64_t size) {
    if (size < 0) {
        size = DEFAULT_STACK_SIZE;
    }

    if (stack_instance_count >= MAX_STACK_INSTANCE) {
        throw std::runtime_error("Too many stack instances!");
    }

    current_memory_top += size;
    stack_instance_count++;

    return current_memory_top & ~0x3;
}

int main(int argc, const char** argv) {
    std::ifstream program_binary_stream{"task.export.bin", std::ios::binary};
    std::ifstream program_info_stream{"task.export.info"};

    nlohmann::json program_info = nlohmann::json::parse(program_info_stream);
    if (!program_info["poolItems"].is_array())
    {
        std::cout << "Pool items are not an array!";
        return -1;
    }

    std::vector<std::uint64_t> pool_items;
    for (const auto &pool_item : program_info["poolItems"])
    {
        pool_items.push_back(pool_item.get<std::uint64_t>());
    }

    std::uint32_t start_offset = program_info["startOffset"].get<std::uint32_t>();

    program_binary_stream.seekg(0, std::ios::end);
    current_memory_top = static_cast<std::uint32_t>(program_binary_stream.tellg());
    memory.resize(current_memory_top + DEFAULT_STACK_SIZE * MAX_STACK_INSTANCE);
    program_binary_stream.seekg(0, std::ios::beg);
    program_binary_stream.read(reinterpret_cast<char *>(memory.data()), memory.size());

    Pip2::VMConfigParameters config(memory.data(), memory.size(), pool_items.data(), pool_items.size(),
                                    allocate_stack, nullptr);

    Pip2::VMOptions options {
            .divide_by_zero_result_zero = true,
            .cache_ = true,
            .cache_root_path_ = "G:\\",
            .text_base_ = start_offset,
            .entry_point_ = 0
    };

    Pip2::VMEngine engine("tasks", config, std::move(options));
    current_engine = &engine;

    engine.context().regs_[Pip2::Register::SP >> 2] = allocate_stack(nullptr, -1);

    start_time = std::chrono::steady_clock::now();
    previous_swap_time = start_time;

    engine.execute_task_aware(hle_handler, nullptr);

    return 0;
}