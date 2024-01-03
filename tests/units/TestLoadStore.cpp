#include <catch2/catch_test_macros.hpp>
#include "TestEnvironment.h"
#include "RandomIntGenerator.h"
#include "LinkerFix.h"

using namespace Pip2;
using namespace Pip2::Test;

TEST_CASE("LDQ: Load 16-bit immediate", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint16_t> rand;

    std::uint16_t randed = rand.next();

    std::vector<Instruction> instructions = {
            make_word_instruction(Opcode::LDQ, Register::P0, randed),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDq", instructions, std::move(pool_items), 0);
    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int16_t>(randed)));
}

TEST_CASE("LDI: Load 32-bit immediate", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_pool_constant_random_generator<std::uint32_t>();

    std::uint32_t randed = rand.next();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDI, Register::P0, Register::P0),
            make_pool_ref(pool_items.get(randed)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDI", instructions, std::move(pool_items), 0);
    env.run();

    REQUIRE(env.reg(Register::P0) == randed);
}

TEST_CASE("LDW: Load 32-bit word", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDWd, Register::P0, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDW", instructions, std::move(pool_items), 0, 20);

    auto *heap = reinterpret_cast<std::uint32_t*>(env.heap());
    const auto randed = rand.next();
    heap[4] = randed;

    env.reg(Register::P0, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == randed);
}

TEST_CASE("LDHd: Load 16-bit signed to 32-bit register", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDHd, Register::P0, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDHd", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::int16_t> rand_16;

    auto *heap = reinterpret_cast<std::uint16_t*>(env.heap());
    const auto randed = rand_16.next();

    heap[8] = randed;

    env.reg(Register::P0, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(randed));
}

TEST_CASE("LDHUd: Load 16-bit unsigned to 32-bit register", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDHUd, Register::P0, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDHUd", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::uint16_t> rand_16;

    auto *heap = reinterpret_cast<std::uint16_t*>(env.heap());
    const auto randed = rand_16.next();

    heap[8] = randed;

    env.reg(Register::P0, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(randed));
}

TEST_CASE("LDBd: Load 8-bit signed to 32-bit register", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDBd, Register::P0, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDBd", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::int8_t> rand_8;

    auto *heap = reinterpret_cast<std::uint8_t*>(env.heap());
    const auto randed = rand_8.next();

    heap[16] = randed;

    env.reg(Register::P0, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(randed));
}

TEST_CASE("LDBUd: Load 8-bit unsigned to 32-bit register", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::LDBUd, Register::P0, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("LDBUd", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::uint8_t> rand_8;

    auto *heap = reinterpret_cast<std::uint8_t*>(env.heap());
    const auto randed = rand_8.next();

    heap[16] = randed;

    env.reg(Register::P0, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(randed));
}

TEST_CASE("STWd: Store 32-bit word", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::STWd, Register::P1, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("STWd", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::uint32_t> rand_32;

    auto *heap = reinterpret_cast<std::uint32_t*>(env.heap());
    const auto randed = rand_32.next();

    env.reg(Register::P0, env.heap_address() + 4);
    env.reg(Register::P1, randed);

    env.run();

    REQUIRE(heap[4] == randed);
}

TEST_CASE("STHd: Store 16-bit word", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::STHd, Register::P1, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("STHd", instructions, std::move(pool_items), 0, 20);

    auto *heap = reinterpret_cast<std::uint16_t*>(env.heap());
    auto randed = rand.next();

    env.reg(Register::P0, env.heap_address() + 4);
    env.reg(Register::P1, randed);

    env.run();

    REQUIRE(heap[8] == (randed & 0xFFFF));
    REQUIRE(heap[9] == 0);
}

TEST_CASE("STBd: Store 8-bit word", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_normal_constant_random_generator<std::uint32_t>();

    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::STBd, Register::P1, Register::P0),
            make_constant(12),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("STBd", instructions, std::move(pool_items), 0, 20);

    auto *heap = reinterpret_cast<std::uint8_t*>(env.heap());
    auto randed = rand.next();

    env.reg(Register::P0, env.heap_address() + 4);
    env.reg(Register::P1, randed);

    env.run();

    REQUIRE(heap[16] == (randed & 0xFF));
    REQUIRE(heap[17] == 0);
    REQUIRE(heap[18] == 0);
    REQUIRE(heap[19] == 0);
}

TEST_CASE("STORE: Store registers to stack", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;

    std::vector<Instruction> instructions = {
            make_range_reg_instruction(Opcode::STORE, Register::P0, 16),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("STORE", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::uint32_t> rand_32;
    std::uint32_t value = rand_32.next();
    std::uint32_t value2 = rand_32.next();
    std::uint32_t value3 = rand_32.next();
    std::uint32_t value4 = rand_32.next();

    env.reg(Register::SP, env.heap_address() + 20);
    env.reg(Register::P0, value);
    env.reg(Register::P1, value2);
    env.reg(Register::P2, value3);
    env.reg(Register::P3, value4);

    env.run();

    auto *heap = reinterpret_cast<std::uint32_t*>(env.heap());

    REQUIRE(heap[1] == value);
    REQUIRE(heap[2] == value2);
    REQUIRE(heap[3] == value3);
    REQUIRE(heap[4] == value4);
}

TEST_CASE("RESTORE: Restore registers from stack", "[PIP2][LoadStore][Single]") {
    ModifiablePoolItems pool_items;

    std::vector<Instruction> instructions = {
            make_range_reg_instruction(Opcode::RESTORE, Register::P3, 16),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("RESTORE", instructions, std::move(pool_items), 0, 20);

    RandomIntGenerator<std::uint32_t> rand_32;
    std::uint32_t value = rand_32.next();
    std::uint32_t value2 = rand_32.next();
    std::uint32_t value3 = rand_32.next();
    std::uint32_t value4 = rand_32.next();

    auto *heap = reinterpret_cast<std::uint32_t*>(env.heap());
    heap[1] = value;
    heap[2] = value2;
    heap[3] = value3;
    heap[4] = value4;

    env.reg(Register::SP, env.heap_address() + 4);
    env.run();

    REQUIRE(env.reg(Register::P0) == value);
    REQUIRE(env.reg(Register::P1) == value2);
    REQUIRE(env.reg(Register::P2) == value3);
    REQUIRE(env.reg(Register::P3) == value4);
}