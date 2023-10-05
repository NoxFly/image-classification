#include "file.hpp"

std::size_t fileCountInDir(std::filesystem::path path) {
    using std::filesystem::directory_iterator;
    return std::distance(directory_iterator(path), directory_iterator{});
}