#include "VMEngine.h"

using namespace Pip2;

#define PIP2_API __attribute__((visibility("default")))

extern "C" {
    PIP2_API VMEngine *vm_engine_create(const char *module_name, const Pip2::VMConfig *config, const Pip2::VMOptions *options) {
        return new Pip2::VMEngine(module_name, *config, *options);
    }

    PIP2_API void vm_engine_destroy(VMEngine *engine) {
        delete engine;
    }

    PIP2_API void vm_engine_execute(VMEngine *engine, HleHandler handler) {
        engine->execute(handler);
    }

    PIP2_API std::uint32_t vm_engine_reg(VMEngine *engine, Pip2::Register reg) {
        return engine->reg(reg);
    }

    PIP2_API void vm_engine_set_reg(VMEngine *engine, Pip2::Register reg, std::uint32_t value) {
        engine->reg(reg, value);
    }
}