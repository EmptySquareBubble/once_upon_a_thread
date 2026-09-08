// The Next-Door Sparrows: four correct atomic counters are packed together.
// Independent writers may still bounce shared cache lines between cores.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <functional>
#include <thread>

constexpr unsigned sticks_per_nest = 2'000'000;

struct Nest {
    std::atomic<unsigned> sticks{0};
    char name[28]{};
};

std::array<Nest, 4> oak{};

void build_nest(Nest& home) {
    for (unsigned i = 0; i < sticks_per_nest; ++i)
        ++home.sticks;
}

int main() {
    const auto start = std::chrono::steady_clock::now();
    std::array<std::jthread, 4> sparrows;
    for (std::size_t i = 0; i < sparrows.size(); ++i)
        sparrows[i] = std::jthread(build_nest, std::ref(oak[i]));
    for (auto& sparrow : sparrows)
        sparrow.join();
    const auto elapsed = std::chrono::steady_clock::now() - start;

    std::printf("packed nests: %u %u %u %u (%lld ms)\n",
                oak[0].sticks.load(), oak[1].sticks.load(),
                oak[2].sticks.load(), oak[3].sticks.load(),
                static_cast<long long>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        elapsed).count()));
}
