// The Gallant Reindeer, first rule tamed — reserve both halves together.
// std::scoped_lock avoids deadlock even when callers list them oppositely.
#include <atomic>
#include <cstdio>
#include <mutex>
#include <thread>

std::mutex west_half, east_half;
std::atomic<int> crossings = 0;

void cross_from_west() {
    std::scoped_lock bridge(west_half, east_half);
    ++crossings;
}

void cross_from_east() {
    std::scoped_lock bridge(east_half, west_half); // reversed — still safe
    ++crossings;
}

int main() {
    std::jthread westbound(cross_from_west);
    std::jthread eastbound(cross_from_east);
    westbound.join();
    eastbound.join();

    std::printf("crossings completed: %d — nobody held half the bridge\n",
                crossings.load());
}
