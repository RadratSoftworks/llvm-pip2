#include "ObjectCache.h"
#include <filesystem>

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/FileSystem.h>

#include <llvm/IR/Module.h>

namespace Pip2 {
    ObjectCache::ObjectCache(const std::string &cache_root_path)
        : cache_root_path_(cache_root_path) {
    }

    std::filesystem::path ObjectCache::get_cache_path(const std::string &module_name)
    {
        return cache_root_path_ / std::filesystem::path(module_name).replace_extension(".obj");
    }

    std::unique_ptr<llvm::MemoryBuffer> ObjectCache::load(const std::string &module_name) {
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
        std::string cache_path = get_cache_path(M->getName().str()).string();
        std::error_code ec;
        llvm::raw_fd_ostream os(cache_path, ec, llvm::sys::fs::OF_None);
        os.write(Obj.getBufferStart(), Obj.getBufferSize());
    }

    std::unique_ptr<llvm::MemoryBuffer> ObjectCache::getObject(const llvm::Module *M) {
        return load(M->getName().str());
    }
}