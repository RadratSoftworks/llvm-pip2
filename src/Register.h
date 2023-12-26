#pragma once

#include <cstdint>

namespace Pip2
{
    enum Register : std::uint8_t
    {
        ZR,
        SP = 4,
        RA = 8,
        FP = 12,

        S0 = 16,
        S1 = 20,
        S2 = 24,
        S3 = 28,
        S4 = 32,
        S5 = 36,
        S6 = 40,
        S7 = 44,

        P0 = 48,
        P1 = 52,
        P2 = 56,
        P3 = 60,

        G0 = 64,
        G1 = 68,
        G2 = 72,
        G3 = 76,
        G4 = 80,
        G5 = 84,
        G6 = 88,
        G7 = 92,
        G8 = 96,
        G9 = 100,
        G10 = 104,
        G11 = 108,
        G12 = 112,
        G13 = 116,

        R0 = 120,
        R1 = 124,

        PC = 128,

        TotalCount = PC / 4
    };
}
