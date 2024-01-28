#pragma once

#include <llvm/ExecutionEngine/ObjectCache.h>

#include <filesystem>
#include <map>
#include <string>

namespace Pip2 {
    class ObjectCache: public llvm::ObjectCache {
    private:
        std::filesystem::path cache_root_path_;

        std::filesystem::path get_cache_path(const std::string &module_name);
        std::filesystem::path get_cache_meta_path(const std::string &module_name);

        std::map<std::string, bool> use_task_modules_;

    public:
        explicit ObjectCache(const std::string &cache_root_path_);

        std::unique_ptr<llvm::MemoryBuffer> load(const std::string &module_name, bool *does_module_use_task = nullptr);
        bool does_cache_exist(const std::string &module_name);

        void notifyObjectCompiled(const llvm::Module *M, llvm::MemoryBufferRef Obj) override;
        std::unique_ptr<llvm::MemoryBuffer> getObject(const llvm::Module *M) override;

        void mark_module_use_task(const std::string &module_name, bool use_task);
    };
}