#include <catch2/catch_test_macros.hpp>
#include "TestEnvironment.h"
#include "RandomIntGenerator.h"
#include "LinkerFix.h"

using namespace Pip2;
using namespace Pip2::Test;

TEST_CASE("ADD: Add two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ADD", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1 + p2);
}

TEST_CASE("ADDi: Add a 32-bit register and a 32-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_pool_constant_random_generator<std::uint32_t>();

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDi, Register::P0, Register::P1, Register::P2),
        make_pool_ref(pool_items.get(p2)),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1 + p2);
}

TEST_CASE("ADDH: Add two 16-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDH, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ADDH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) ==
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(p1 & 0xFFFF) + static_cast<std::uint16_t>(p2 & 0xFFFF)));
}

TEST_CASE("ADDHi: Add a 16-bit register and a 16-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand;
    RandomIntGenerator<std::uint8_t> rand_byte;

    const std::uint32_t p1 = rand.next();
    const std::uint8_t p2 = rand_byte.next();

    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDHi, Register::P0, Register::P1, static_cast<Register>(p2)),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDHi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) ==
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(p1 & 0xFFFF) + static_cast<std::int8_t>(p2)));
}

TEST_CASE("ADDB: Add two 8-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDB, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ADDB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) ==
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(p1 & 0xFF) + static_cast<std::uint8_t>(p2 & 0xFF)));
}

TEST_CASE("ADDBi: Add a 8-bit register and a 8-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand;
    RandomIntGenerator<std::uint8_t> rand_byte;

    const std::uint32_t p1 = rand.next();
    const std::uint8_t p2 = rand_byte.next();

    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDBi, Register::P0, Register::P1, static_cast<Register>(p2)),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDBi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) ==
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(p1 & 0xFF) + p2));
}

TEST_CASE("SUB: Subtract two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SUB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1 - p2);
}

TEST_CASE("SUBi: Subtract a 32-bit register and a 32-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_pool_constant_random_generator<std::uint32_t>();

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::SUBi, Register::P0, Register::P1, Register::P2),
        make_pool_ref(pool_items.get(p2)),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SUBi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1 - p2);
}

TEST_CASE("SUBH: Subtract two 16-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::SUBH, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SUBH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) ==
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(p1 & 0xFFFF) - static_cast<std::uint16_t>(p2 & 0xFFFF)));
}

TEST_CASE("SUBB: Subtract two 8-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::SUBB, Register::P0, Register::P1, Register::P2),
        make_unary_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SUBB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) ==
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(p1 & 0xFF) - static_cast<std::uint8_t>(p2 & 0xFF)));
}

