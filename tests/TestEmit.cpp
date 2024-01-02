#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <ProgramAnalysis.h>
#include <Translator.h>
#include <VMEngine.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>

#include <LinkerFix.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Not enough arguments!";
        return -1;
    }

    const char *path_program_info = argv[1];
    const char *path_program_binary = argv[2];

    try
    {
        std::ifstream program_binary_stream{path_program_binary, std::ios::binary};
        std::ifstream program_info_stream{path_program_info};

        nlohmann::json program_info = nlohmann::json::parse(program_info_stream);
        if (!program_info["poolItems"].is_array())
        {
            std::cout << "Pool items are not an array!";
            return -1;
        }

        std::vector<std::uint32_t> pool_items;
        for (const auto &pool_item : program_info["poolItems"])
        {
            pool_items.push_back(pool_item.get<std::uint32_t>());
        }

        std::uint32_t start_offset = program_info["startOffset"].get<std::uint32_t>();

        std::vector<std::uint8_t> program_binary;

        program_binary_stream.seekg(0, std::ios::end);
        program_binary.resize(program_binary_stream.tellg());
        program_binary_stream.seekg(0, std::ios::beg);
        program_binary_stream.read(reinterpret_cast<char *>(program_binary.data()), program_binary.size());

        Pip2::PoolItems pool_items_obj{pool_items.data(), pool_items.size()};
        Pip2::ProgramAnalysis program_analysis{reinterpret_cast<std::uint32_t *>(program_binary.data()),
                                               start_offset,
                                               program_binary.size() - start_offset,
                                               pool_items_obj};

        auto funcs = program_analysis.analyze(0);

        llvm::LLVMContext temp_context;

        Pip2::VMConfig config(program_binary.data(), program_binary.size(), pool_items.data(), pool_items.size());
        Pip2::VMOptions options {
            .divide_by_zero_result_zero = true,
            .cache_ = false
        };

        Pip2::Translator translator(temp_context, config, options);
        auto module = translator.translate(std::filesystem::path(path_program_binary).filename().string(), funcs);

        Pip2::VMEngine::default_optimize(*module);

        std::error_code error_code;
        llvm::raw_fd_ostream bitcode_stream(std::filesystem::path(path_program_binary).replace_extension(".bc").string(), error_code, llvm::sys::fs::OF_None);
        llvm::WriteBitcodeToFile(*module, bitcode_stream);

        module->dump();
    }
    catch (const std::exception &ex)
    {
        std::cout << "Program info parse failed with error: " << ex.what();
    }

    return 0;
}