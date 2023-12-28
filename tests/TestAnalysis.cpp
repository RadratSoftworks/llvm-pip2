#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <ProgramAnalysis.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Not enough arguments!";
        return -1;
    }

    const char *path_program_info = argv[1];
    const char *path_program_binary = argv[2];

    try
    {
        std::ifstream program_binary_stream{ path_program_binary, std::ios::binary };
        std::ifstream program_info_stream{ path_program_info };

        nlohmann::json program_info = nlohmann::json::parse(program_info_stream);
        if (!program_info["poolItems"].is_array()) {
            std::cout << "Pool items are not an array!";
            return -1;
        }

        std::vector<std::uint32_t> pool_items;
        for (const auto &pool_item : program_info["poolItems"]) {
            pool_items.push_back(pool_item.get<std::uint32_t>());
        }

        std::uint32_t start_offset = program_info["startOffset"].get<std::uint32_t>();

        std::vector<std::uint8_t> program_binary;

        program_binary_stream.seekg(0, std::ios::end);
        program_binary.resize(program_binary_stream.tellg());
        program_binary_stream.seekg(0, std::ios::beg);
        program_binary_stream.read(reinterpret_cast<char *>(program_binary.data()), program_binary.size());

        Pip2::PoolItems pool_items_obj{ pool_items.data(), pool_items.size() };
        Pip2::ProgramAnalysis program_analysis{ reinterpret_cast<std::uint32_t*>(program_binary.data()),
                                                start_offset,
                                                program_binary.size() - start_offset,
                                                pool_items_obj };

        program_analysis.analyze(0);
    }
    catch (const std::exception &ex)
    {
        std::cout << "Program info parse failed with error: " << ex.what();
    }


    return 0;
}