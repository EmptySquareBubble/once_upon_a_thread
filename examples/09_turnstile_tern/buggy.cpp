// The Turnstile Tern: correct, but unrelated attractions share one gate.
#include <chrono>
#include <cstdio>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

constexpr unsigned visits = 200;
std::mutex zoo_mutex;
unsigned aquarium_visitors = 0;
unsigned carousel_riders = 0;

void explore_aquarium() { std::this_thread::sleep_for(100us); }
void buy_ice_cream() { std::this_thread::sleep_for(100us); }

void visit_aquarium() {
    for (unsigned i = 0; i < visits; ++i) {
        std::lock_guard lock(zoo_mutex);
        ++aquarium_visitors;
        explore_aquarium();
    }
}

void ride_carousel() {
    for (unsigned i = 0; i < visits; ++i) {
        std::lock_guard lock(zoo_mutex);
        ++carousel_riders;
        buy_ice_cream();
    }
}

int main() {
    const auto start = std::chrono::steady_clock::now();
    {
        std::jthread aquarium(visit_aquarium);
        std::jthread carousel(ride_carousel);
    }
    const auto elapsed = std::chrono::steady_clock::now() - start;

    std::printf("aquarium=%u carousel=%u, one shared gate: %lld ms\n",
                aquarium_visitors, carousel_riders,
                static_cast<long long>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        elapsed).count()));
}
