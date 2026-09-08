// The Phantom Bellringer, tamed — wait for the delivery state, not the bell.
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <queue>
#include <thread>

struct crate {
    int id;
};

std::mutex m;
std::condition_variable bell;
std::queue<crate> deliveries;

void stock(crate delivery) {
    std::printf("stocked crate %d\n", delivery.id);
}

void night_stocker() {
    std::unique_lock lock(m);
    bell.wait(lock, [] { return !deliveries.empty(); });
    stock(deliveries.front());
    deliveries.pop();
}

void delivery_truck(crate delivery) {
    {
        std::lock_guard lock(m);
        deliveries.push(delivery);
    }
    bell.notify_one();
}

int main() {
    std::jthread truck(delivery_truck, crate{42});
    night_stocker();
}
