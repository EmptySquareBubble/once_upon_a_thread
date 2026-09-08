// Off-route bonus: relaxed StoreLoad ordering permits both dragons to miss
// the other's earlier-in-program-order store. A single run will usually look
// fine; possibility, not reproducibility, is the lesson.
#include <atomic>
#include <cstdio>
#include <thread>

std::atomic<bool> red_flag{false};
std::atomic<bool> blue_flag{false};
bool red_saw_blue = false;
bool blue_saw_red = false;

int main() {
    std::jthread red([] {
        red_flag.store(true, std::memory_order_relaxed);
        red_saw_blue = blue_flag.load(std::memory_order_relaxed);
    });
    std::jthread blue([] {
        blue_flag.store(true, std::memory_order_relaxed);
        blue_saw_red = red_flag.load(std::memory_order_relaxed);
    });
    red.join();
    blue.join();

    std::printf("red saw blue: %s, blue saw red: %s\n",
                red_saw_blue ? "yes" : "no",
                blue_saw_red ? "yes" : "no");
    if (!red_saw_blue && !blue_saw_red)
        std::puts("both missed: allowed by relaxed ordering");
}
