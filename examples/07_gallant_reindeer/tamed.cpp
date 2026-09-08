// The Gallant Reindeer, second rule tamed — the hand-rolled back-out-and-bow
// loop is replaced by blocking acquisition of both bridge halves.
#include <atomic>
#include <cstdio>
#include <mutex>
#include <thread>

std::mutex west_half, east_half;
std::atomic<int> crossings = 0;

void cross_bridge(std::mutex& near_half, std::mutex& far_half) {
    std::scoped_lock bridge(near_half, far_half);
    ++crossings;
}

int main() {
    std::jthread westbound([] { cross_bridge(west_half, east_half); });
    std::jthread eastbound([] { cross_bridge(east_half, west_half); });
    westbound.join();
    eastbound.join();

    std::printf("crossings completed: %d — no courtesy retry loop\n",
                crossings.load());
}
