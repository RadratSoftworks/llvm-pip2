# LLVM-PIP2

## Overview

LLVM-PIP2 uplifts Mophun executable that runs on PIP2 Virtual machine to LLVM IR, which will be compile to JIT code that is cacheable.

## Pipelines

LLVM-PIP2 goes through three main pipelines:

- Static analysis: from the entry point and the relocation information, try to find any code subroutine that will be used by the application. The static analyzer will also try to resolve static-analysable indirect branches, such as jump table, which help provide information for further pipelines to produce effecient code.

- Code translation: All subroutines discovered during static analysis will be passed to the code translation to translate to IR codes. Some information such as discovered jump tables are mapped to LLVM IR instruction, to avoid runtime indirect branching.

- Code generation: After all the code are translated, the library will call finalizeObject to make LLVM generate the code. The code will also be cached during this pipeline.

## Assumptions

In order to efficiently analyse and emit code, the library makes some assumptions about the assembly, such as:

- Jump that involves RA, such as **RET RA** or **JP RA**, will always end the subroutine and return to the caller

- Predictable jump table pattern: there are two jump table patterns in total

- **CALL** instruction will always return back to the caller after the subroutine it jumps to finishes execution

## Licenses

LLVM-PIP2 is released under the MIT license.