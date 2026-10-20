
#include <iostream>
#include <memory_resource>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <typeindex>

int main() {
    try {
        std::size_t persistentBufferSize = 32 * 1024 * 1024;
        void* persistentBuffer = std::malloc(persistentBufferSize);
        auto* persistentArena = new std::pmr::monotonic_buffer_resource(
            persistentBuffer, persistentBufferSize, std::pmr::null_memory_resource()
        );
        auto* persistentPool = new std::pmr::synchronized_pool_resource(persistentArena);
        std::pmr::vector<int> v(persistentPool);
        v.reserve(16);
        std::pmr::unordered_map<std::type_index, int> m(persistentPool);
        m.reserve(16);
        std::cout << 'Pool works!' << std::endl;
    } catch (const std::exception& e) {
        std::cout << 'Exception: ' << e.what() << std::endl;
    }
    return 0;
}
