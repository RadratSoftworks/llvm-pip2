#include "ObjectCache.h"
#include "Constants.h"

#include <fstream>
#include <filesystem>

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/FileSystem.h>

#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace Pip2 {
    ObjectCache::ObjectCache(const std::string &cache_root_path)
        : cache_root_path_(cache_root_path) {
    }

    std::filesystem::path ObjectCache::get_cache_path(const std::string &module_name)
    {
        return cache_root_path_ / std::filesystem::path(module_name).replace_extension(".obj");
    }

    std::filesystem::path ObjectCache::get_cache_meta_path(const std::string &module_name)
    {
        return cache_root_path_ / std::filesystem::path(module_name).replace_extension(".obj.meta");
    }

    std::unique_ptr<llvm::MemoryBuffer> ObjectCache::load(const std::string &module_name, bool *does_module_use_task) {
        auto cache_meta_path = get_cache_meta_path(module_name);
        if (!std::filesystem::exists(cache_meta_path)) {
            return nullptr;
        }

        std::ifstream meta_file_stream(cache_meta_path.string());
        try {
            json meta_file = json::parse(meta_file_stream);
            if (meta_file["version"] != Pip2::CACHE_VERSION) {
                // Force recompile
                return nullptr;
            }
            if (does_module_use_task != nullptr) {
                *does_module_use_task = meta_file.value<bool>("use_task", false);
            }
        } catch (std::exception &ex) {
        }

        auto cache_path = get_cache_path(module_name);
        if (!std::filesystem::exists(cache_path)) {
            return nullptr;
        }

        std::error_code ec;
        auto buffer = llvm::MemoryBuffer::getFile(cache_path.string(), -1, false);
        if (buffer) {
            return std::move(*buffer);
        }
        return nullptr;
    }

    bool ObjectCache::does_cache_exist(const std::string &module_name) {
        return std::filesystem::exists(get_cache_path(module_name).string());
    }

    void ObjectCache::notifyObjectCompiled(const llvm::Module *M, llvm::MemoryBufferRef Obj) {
        std::string module_name = M->getName().str();
        std::string cache_path = get_cache_path(module_name).string();
        std::error_code ec;
        llvm::raw_fd_ostream os(cache_path, ec, llvm::sys::fs::OF_None);
        os.write(Obj.getBufferStart(), Obj.getBufferSize());

        {
            std::string cache_meta_path = get_cache_meta_path(M->getName().str()).string();
            std::ofstream meta_file_stream(cache_meta_path);
            json meta_file;
            meta_file["version"] = Pip2::CACHE_VERSION;
            meta_file["use_task"] =
                    (use_task_modules_.find(module_name) != use_task_modules_.end()) && use_task_modules_[module_name];
            meta_file_stream << meta_file.dump(4);
        }
    }

    std::unique_ptr<llvm::MemoryBuffer> ObjectCache::getObject(const llvm::Module *M) {
        return load(M->getName().str());
    }

    void ObjectCache::mark_module_use_task(const std::string &module_name, bool use_task) {
        if (use_task_modules_.find(module_name) != use_task_modules_.end()) {
            use_task_modules_[module_name] = use_task;
        } else {
            use_task_modules_.emplace(module_name, use_task);
        }
    }
}