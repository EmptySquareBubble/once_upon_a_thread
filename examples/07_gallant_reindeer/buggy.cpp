// The Gallant Reindeer, second rule — always step aside. Each reindeer enters
// its near half, tries the far half, backs out, bows, and retries. Matching
// schedules can livelock; other runs complete normally.
#include <atomic>
#include <cstdio>
#include <mutex>
#include <thread>

std::mutex west_half, east_half;
std::atomic<int> crossings = 0;
std::atomic<int> polite_bows = 0;

bool cross_politely(std::mutex& near_half, std::mutex& far_half) {
    constexpr int max_attempts = 1'000;
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        std::unique_lock near(near_half);
        std::unique_lock far(far_half, std::defer_lock);
        if (far.try_lock()) {
            ++crossings;
            return true;
        }
        near.unlock();
        ++polite_bows;
    }
    return false;
}

int main() {
    bool west_crossed = false;
    bool east_crossed = false;
    std::jthread westbound([&] {
        west_crossed = cross_politely(west_half, east_half);
    });
    std::jthread eastbound([&] {
        east_crossed = cross_politely(east_half, west_half);
    });
    westbound.join();
    eastbound.join();

    std::printf("crossings: %d, polite bows: %d\n",
                crossings.load(), polite_bows.load());
    if (!west_crossed || !east_crossed)
        std::puts("a reindeer exhausted its retries without crossing");
}
