#include <catch2/catch_test_macros.hpp>
#include "TestEnvironment.h"
#include "RandomIntGenerator.h"
#include "LinkerFix.h"

using namespace Pip2;
using namespace Pip2::Test;

TEST_CASE("BEQ: Branch if equals", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::BEQ, Register::P1, Register::P2, Register::P2),
        make_constant(20),
        make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPl, Register::RA),
        make_constant(12),
        make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();

    TestEnvironment env("BEQ", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == 0);
}

TEST_CASE("BNE: Branch if not equals", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BNE, Register::P1, Register::P2, Register::P2),
            make_constant(20),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();

    TestEnvironment env("BEQ", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1 * 2);
}

TEST_CASE("BLTU: Branch if less than (unsigned)", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::BLTU, Register::P1, Register::P2, Register::P2),
        make_constant(20),
        make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPl, Register::RA),
        make_constant(12),
        make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("BLTU", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 < p2) ? static_cast<std::uint32_t>(p1 - p2) : (p1 + p2)));
}

TEST_CASE("BLT: Branch if less than (signed)", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BLT, Register::P1, Register::P2, Register::P2),
            make_constant(20),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::int32_t> rand;

    const std::int32_t p1 = rand.next();
    const std::int32_t p2 = rand.next();

    TestEnvironment env("BLT", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));
    env.reg(Register::P2, static_cast<std::uint32_t>(p2));

    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 < p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
}

TEST_CASE("BLEU: Branch if less than or equal (unsigned)", "[PIP2][ControlFlow][Single]") {
    WHEN("Two values are equal") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BLEU, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::uint32_t p1 = rand.next();

        TestEnvironment env("BLEU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }

    WHEN("Randomized values") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BLEU, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::uint32_t p1 = rand.next();
        const std::uint32_t p2 = rand.next();

        TestEnvironment env("BLEU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p2);

        env.run();

        REQUIRE(env.reg(Register::P0) == ((p1 <= p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
    }
}

TEST_CASE("BLE: Branch if less than or equal (signed)", "[PIP2][ControlFlow][Single]") {
    WHEN("Two values are equal") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BLE, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::int32_t> rand;

        const std::int32_t p1 = rand.next();

        TestEnvironment env("BLE", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, static_cast<std::uint32_t>(p1));
        env.reg(Register::P2, static_cast<std::uint32_t>(p1));

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }

    WHEN("Randomized values") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BLE, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::int32_t p1 = rand.next();
        const std::int32_t p2 = rand.next();

        TestEnvironment env("BLE", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, static_cast<std::uint32_t>(p1));
        env.reg(Register::P2, static_cast<std::uint32_t>(p2));

        env.run();

        REQUIRE(env.reg(Register::P0) == ((p1 <= p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
    }
}

TEST_CASE("BGTU: Branch if greater than (unsigned)", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BGTU, Register::P1, Register::P2, Register::P2),
            make_constant(20),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;
    auto p1 = rand.next();
    auto p2 = rand.next();

    TestEnvironment env("BGTU", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);
    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 > p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
}

TEST_CASE("BGT: Branch if greater than (signed)", "[PIP2][ControlFlow][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BGT, Register::P1, Register::P2, Register::P2),
            make_constant(20),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::int32_t> rand;
    auto p1 = rand.next();
    auto p2 = rand.next();

    TestEnvironment env("BGT", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));
    env.reg(Register::P2, static_cast<std::uint32_t>(p2));
    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 > p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
}

TEST_CASE("BGEU: Branch if greater than or equal (unsigned)", "[PIP2][ControlFlow][Single]") {
    WHEN("Two values are equal") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BGEU, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::uint32_t p1 = rand.next();

        TestEnvironment env("BGEU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p1);

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }

    WHEN("Randomized values") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BGEU, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::uint32_t p1 = rand.next();
        const std::uint32_t p2 = rand.next();

        TestEnvironment env("BGEU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p2);

        env.run();

        REQUIRE(env.reg(Register::P0) == ((p1 >= p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
    }
}

TEST_CASE("BGE: Branch if greater than or equal (signed)", "[PIP2][ControlFlow][Single]") {
    WHEN("Two values are equal") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BGE, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::int32_t> rand;

        const std::int32_t p1 = rand.next();

        TestEnvironment env("BGE", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, static_cast<std::uint32_t>(p1));
        env.reg(Register::P2, static_cast<std::uint32_t>(p1));

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }

    WHEN("Randomized values") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::BGE, Register::P1, Register::P2, Register::P2),
                make_constant(20),
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPl, Register::RA),
                make_constant(12),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::int32_t p1 = rand.next();
        const std::int32_t p2 = rand.next();

        TestEnvironment env("BGE", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, static_cast<std::uint32_t>(p1));
        env.reg(Register::P2, static_cast<std::uint32_t>(p2));

        env.run();

        REQUIRE(env.reg(Register::P0) == ((p1 >= p2) ? static_cast<std::uint32_t>(p1 - p2) : static_cast<std::uint32_t>(p1 + p2)));
    }
}

TEST_CASE("BGTi: Compare one 32-bit register and one immediate (signed)", "[PIP2][ControlFlow][Single]") {
    RandomIntGenerator<std::int32_t> rand;
    auto p1 = rand.next();
    auto p3 = rand.next();

    RandomIntGenerator<std::int8_t> rand_8;
    auto p2 = rand_8.next();

    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BGTI, Register::P1, static_cast<Register>(p2), static_cast<Register>(4)),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("BGTI", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));
    env.reg(Register::P2, static_cast<std::uint32_t>(p3));
    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 > p2) ? static_cast<std::uint32_t>(p1 - p3) : static_cast<std::uint32_t>(p1 + p3)));
}

TEST_CASE("BGTIB: Compare one 8-bit register and one immediate (signed)", "[PIP2][ControlFlow][Single]") {
    RandomIntGenerator<std::int8_t> rand_8;
    auto p1 = rand_8.next();
    auto p2 = rand_8.next();

    RandomIntGenerator<std::uint32_t> rand_32;
    auto p3 = rand_32.next();
    auto p4 = rand_32.next();

    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::BGTIB, Register::P1, static_cast<Register>(p2), static_cast<Register>(4)),
            make_binary_instruction(Opcode::ADD, Register::P0, Register::P3, Register::R0),
            make_single_argument_instruction(Opcode::JPl, Register::RA),
            make_constant(12),
            make_binary_instruction(Opcode::SUB, Register::P0, Register::P3, Register::R0),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("BGTIB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));
    env.reg(Register::P2, static_cast<std::uint32_t>(p2));
    env.reg(Register::P3, p3);
    env.reg(Register::R0, p4);
    env.run();

    REQUIRE(env.reg(Register::P0) == ((p1 > p2) ? static_cast<std::uint32_t>(p3 - p4) : static_cast<std::uint32_t>(p3 + p4)));
}

TEST_CASE("CALLl: Call a function", "[PIP2][ControlFlow][Single]") {
    WHEN("Function is HLE") {
        struct TemporaryData {
            TestEnvironment *env_pointer = nullptr;
            std::uint32_t collected_value_ = 0;
        } temporary_data;

        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::ADD, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::CALLl, Register::RA),
                make_pool_ref(pool_items.get([](void *userdata) {
                        // Test registers flushing before CALL
                        TemporaryData *temporary_data = reinterpret_cast<TemporaryData*>(userdata);
                        if (temporary_data != nullptr) {
                            temporary_data->collected_value_ = temporary_data->env_pointer->reg(Register::P0);
                        }
                    }, &temporary_data)),
                make_binary_instruction(Opcode::SUB, Register::P0, Register::P0, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand_32;
        auto p3 = rand_32.next();
        auto p4 = rand_32.next();

        TestEnvironment env("CALLl", instructions, std::move(pool_items), 0);
        temporary_data.env_pointer = &env;

        env.reg(Register::P1, p3);
        env.reg(Register::P2, p4);
        env.run(&modifiable_pool_items_hle_handler, &pool_items);

        REQUIRE(env.reg(Register::P0) == p3);
        REQUIRE(temporary_data.collected_value_ == p3 + p4);
    }
}