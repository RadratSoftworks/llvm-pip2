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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDHi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) ==
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(p1 & 0xFFFF) + static_cast<std::uint16_t>(static_cast<std::int8_t>(p2))));
}

TEST_CASE("ADDB: Add two 8-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::ADDB, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDBi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) ==
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(p1 & 0xFF) + p2));
}


TEST_CASE("ADDQ: Add one 32-bit register and one 8-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand;
    RandomIntGenerator<std::uint8_t> rand_byte;

    const std::uint32_t p1 = rand.next();
    const std::uint8_t p2 = rand_byte.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ADDQ, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ADDQ", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(p1 + static_cast<std::uint32_t>(static_cast<std::int8_t>(p2))));
}

TEST_CASE("SUB: Subtract two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
        make_binary_instruction(Opcode::SUB, Register::P0, Register::P1, Register::P2),
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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
        make_single_argument_instruction(Opcode::JPr, Register::RA)
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

TEST_CASE("AND: Bitwise AND two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::AND, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("AND", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == (p1 & p2));
}

TEST_CASE("ANDi: Bitwise AND a 32-bit register and a 32-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::uint32_t> rand = make_pool_constant_random_generator<std::uint32_t>();

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ANDi, Register::P0, Register::P1, Register::P2),
            make_pool_ref(pool_items.get(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("ANDi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == (p1 & p2));
}

TEST_CASE("ANDH: Bitwise AND two 16-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ANDH, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ANDH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) ==
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(p1 & 0xFFFF) & static_cast<std::uint16_t>(p2 & 0xFFFF)));
}

TEST_CASE("ANDB: Bitwise AND two 8-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ANDB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ANDB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) ==
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(p1 & 0xFF) & static_cast<std::uint8_t>(p2 & 0xFF)));
}

TEST_CASE("DIV: Divide two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    WHEN("Divisor is not zero") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::DIV, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::int32_t> rand;

        const std::int32_t p1 = rand.next();
        std::int32_t p2 = rand.next();

        while (p2 == 0) {
            p2 = rand.next();
        }

        TestEnvironment env("DIV", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p2);

        env.run();

        REQUIRE(env.reg(Register::P0) == (p1 / p2));
    }

    AND_WHEN("Divisor is zero") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::DIV, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::int32_t> rand;

        const std::int32_t p1 = rand.next();
        const std::int32_t p2 = 0;

        TestEnvironment env("DIV", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p2);

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }
}

TEST_CASE("DIVU: Divide two unsigned 32-bit registers", "[PIP2][Arithmetic][Single]") {
    WHEN("Divisor is not zero") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::DIVU, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::int32_t> rand;

        const std::int32_t p1 = rand.next();
        std::int32_t p2 = rand.next();

        while (p2 == 0) {
            p2 = rand.next();
        }

        TestEnvironment env("DIVU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, static_cast<std::uint32_t>(p1));
        env.reg(Register::P2, static_cast<std::uint32_t>(p2));

        env.run();

        REQUIRE(env.reg(Register::P0) == (static_cast<std::uint32_t>(p1) / static_cast<std::uint32_t>(p2)));
    }

    AND_WHEN("Divisor is zero") {
        ModifiablePoolItems pool_items;
        std::vector<Instruction> instructions = {
                make_binary_instruction(Opcode::DIVU, Register::P0, Register::P1, Register::P2),
                make_single_argument_instruction(Opcode::JPr, Register::RA)
        };

        RandomIntGenerator<std::uint32_t> rand;

        const std::uint32_t p1 = rand.next();
        const std::uint32_t p2 = 0;

        TestEnvironment env("DIVU", instructions, std::move(pool_items), 0);
        env.reg(Register::P1, p1);
        env.reg(Register::P2, p2);

        env.run();

        REQUIRE(env.reg(Register::P0) == 0);
    }
}

TEST_CASE("MOV: Move a 32-bit register to another 32-bit register", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MOV, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;

    const std::uint32_t p1 = rand.next();

    TestEnvironment env("MOV", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(env.reg(Register::P0) == p1);
}

TEST_CASE("MOVH: Move a 16-bit register to another 16-bit register", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MOVH, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint16_t> rand;

    const std::uint16_t p1 = rand.next();

    TestEnvironment env("MOVH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint16_t>(env.reg(Register::P0) & 0xFFFF) == p1);
}

TEST_CASE("MOVB: Move a 8-bit register to another 8-bit register", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MOVB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint8_t> rand;

    const std::uint8_t p1 = rand.next();

    TestEnvironment env("MOVB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE(static_cast<std::uint8_t>(env.reg(Register::P0) & 0xFF) == p1);
}

TEST_CASE("MUL: Multiply two 32-bit registers", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MUL, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::int32_t> rand;

    const std::int32_t p1 = rand.next();
    const std::int32_t p2 = rand.next();

    TestEnvironment env("MUL", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(p1 * p2));
}

TEST_CASE("MULi: Multiply a 32-bit register and a 32-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::int32_t> rand = make_pool_constant_random_generator<std::int32_t>();

    const std::int32_t p1 = rand.next();
    const std::int32_t p2 = rand.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MULi, Register::P0, Register::P1, Register::P2),
            make_pool_ref(pool_items.get(static_cast<std::uint32_t>(p2))),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("MULi", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(p1 * p2));
}

TEST_CASE("MULQ: Multiply a 32-bit register and a 8-bit immediate", "[PIP2][Arithmetic][Single]") {
    ModifiablePoolItems pool_items;
    RandomIntGenerator<std::int32_t> rand;
    RandomIntGenerator<std::uint8_t> rand_byte;

    const std::int32_t p1 = rand.next();
    const std::uint8_t p2 = rand_byte.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::MULQ, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("MULQ", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, static_cast<std::uint32_t>(p1));

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(p1 * p2));
}


TEST_CASE("OR: Bitwise or two 32-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::OR, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("OR", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == (p1 | p2));
}

TEST_CASE("ORB: Bitwise or two 8-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ORB, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint8_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ORB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFF) == ((p1 & 0xFF) | (p2 & 0xFF)));
}

TEST_CASE("ORH: Bitwise or two 16-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::ORH, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint16_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("ORH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFFFF) == ((p1 & 0xFFFF) | (p2 & 0xFFFF)));
}

TEST_CASE("SLL: Shift left logical two 32-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SLL, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SLL", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == (p1 << p2));
}

TEST_CASE("SLLB: Shift left logical two 8-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;

    RandomIntGenerator<std::uint8_t> rand;
    const std::uint8_t p1 = rand.next();
    const std::uint8_t p2 = rand.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SLLB, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SLLB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFF) == static_cast<std::uint8_t>(p1 << (p2 & 0x7)));
}

TEST_CASE("SLLH: Shift left logical two 16-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;

    RandomIntGenerator<std::uint16_t> rand;
    const std::uint16_t p1 = rand.next();
    const auto p2 = static_cast<std::uint8_t>(rand.next());

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SLLH, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SLLH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);

    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFFFF) == static_cast<std::uint16_t>(p1 << (p2 & 0xF)));
}

TEST_CASE("SRL: Shift right logical two 32-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SRL, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::uint32_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SRL", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == (p1 >> p2));
}

TEST_CASE("SRLB: Shift right logical two 8-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;

    RandomIntGenerator<std::uint8_t> rand;
    const std::uint8_t p1 = rand.next();
    const std::uint8_t p2 = rand.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SRLB, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SRLB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFF) == static_cast<std::uint8_t>(p1 >> (p2 & 0x7)));
}

TEST_CASE("SRLH: Shift right logical two 16-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;

    RandomIntGenerator<std::uint16_t> rand;
    const std::uint16_t p1 = rand.next();
    const auto p2 = static_cast<std::uint8_t>(rand.next());

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SRLH, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SRLH", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFFFF) == static_cast<std::uint16_t>(p1 >> (p2 & 0xF)));
}

TEST_CASE("SRA: Shift right arithmetic two 32-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;
    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SRA, Register::P0, Register::P1, Register::P2),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    RandomIntGenerator<std::int32_t> rand;
    const std::uint32_t p1 = rand.next();
    const std::uint32_t p2 = rand.next();

    TestEnvironment env("SRA", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.reg(Register::P2, p2);

    env.run();

    REQUIRE(env.reg(Register::P0) == static_cast<std::uint32_t>(static_cast<std::int32_t>(p1) >> p2));
}

TEST_CASE("SRAB: Shift right arithmetic two 8-bit registers", "[PIP2][Miscs][Single]") {
    ModifiablePoolItems pool_items;

    RandomIntGenerator<std::uint8_t> rand;
    const std::uint8_t p1 = rand.next();
    const std::uint8_t p2 = rand.next();

    std::vector<Instruction> instructions = {
            make_binary_instruction(Opcode::SRAB, Register::P0, Register::P1, static_cast<Register>(p2)),
            make_single_argument_instruction(Opcode::JPr, Register::RA)
    };

    TestEnvironment env("SRAB", instructions, std::move(pool_items), 0);
    env.reg(Register::P1, p1);
    env.run();

    REQUIRE((env.reg(Register::P0) & 0xFF) == static_cast<std::uint8_t>(static_cast<std::int8_t>(p1) >> (p2 & 0x7)));
}