// The Gallant Reindeer, first rule — hold your ground. Two reindeer enter
// a narrow bridge from opposite ends. Each occupies the near half and waits
// for the far half. Depending on scheduling, they may deadlock or both cross.
#include <atomic>
#include <cstdio>
#include <mutex>
#include <thread>

std::mutex west_half, east_half;
std::atomic<int> crossings = 0;

void cross_from_west() {
    std::lock_guard west(west_half);
    std::lock_guard east(east_half);
    ++crossings;
}

void cross_from_east() {
    std::lock_guard east(east_half);
    std::lock_guard west(west_half);
    ++crossings;
}

int main() {
    std::jthread westbound(cross_from_west);
    std::jthread eastbound(cross_from_east);
    westbound.join();
    eastbound.join();
    std::printf("crossings completed: %d\n", crossings.load());
}
