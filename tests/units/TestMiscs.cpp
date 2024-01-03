#include <catch2/catch_test_macros.hpp>
#include "TestEnvironment.h"
#include "RandomIntGenerator.h"
#include "LinkerFix.h"

using namespace Pip2;
using namespace Pip2::Test;

TEST_CASE("EXSB: Sign-extended byte", "[PIP2][Miscs][Single]") {
    WHEN("Destination and source are same") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::EXSB, Register::P0, Register::P0),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;
        const std::uint32_t p1 = rand.next();

        TestEnvironment env("EXSB", instructions, std::move(pool_items), 0);
        env.reg(Register::P0, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int8_t>(p1 & 0xFF)));
    }

    WHEN("Destination and source are different") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::EXSB, Register::P0, Register::P1),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;
        const std::uint32_t p1 = rand.next();

        TestEnvironment env("EXSB", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int8_t>(p1 & 0xFF)));
    }
}

TEST_CASE("EXSH: Sign-extended halfword", "[PIP2][Miscs][Single]") {
    WHEN("Destination and source are same") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::EXSH, Register::P0, Register::P0),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;
        const std::uint32_t p1 = rand.next();

        TestEnvironment env("EXSH", instructions, std::move(pool_items), 0);
        env.reg(Register::P0, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int16_t>(p1 & 0xFFFF)));
    }

    WHEN("Destination and source are different") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::EXSH, Register::P0, Register::P1),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;
        const std::uint32_t p1 = rand.next();

        TestEnvironment env("EXSH", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int16_t>(p1 & 0xFFFF)));
    }
}

TEST_CASE("NEG: Negate", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::NEG, Register::P0, Register::P1),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::int32_t> rand;
    const std::int32_t p1 = rand.next();

    TestEnvironment env("NEG", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(-p1));
}

TEST_CASE("NOT: Bitwise not", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_unary_instruction(Opcode::NOT, Register::P0, Register::P1),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;
    const std::uint32_t p1 = rand.next();

    TestEnvironment env("NOT", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == ~p1);
}