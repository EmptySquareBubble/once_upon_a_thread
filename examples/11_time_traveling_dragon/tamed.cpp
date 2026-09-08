// Off-route bonus, tamed: sequential consistency puts all four operations in
// one global order, so at least one dragon must see the other's flag.
#include <atomic>
#include <cstdio>
#include <thread>

std::atomic<bool> red_flag{false};
std::atomic<bool> blue_flag{false};
bool red_saw_blue = false;
bool blue_saw_red = false;

int main() {
    std::jthread red([] {
        red_flag.store(true, std::memory_order_seq_cst);
        red_saw_blue = blue_flag.load(std::memory_order_seq_cst);
    });
    std::jthread blue([] {
        blue_flag.store(true, std::memory_order_seq_cst);
        blue_saw_red = red_flag.load(std::memory_order_seq_cst);
    });
    red.join();
    blue.join();

    std::printf("red saw blue: %s, blue saw red: %s\n",
                red_saw_blue ? "yes" : "no",
                blue_saw_red ? "yes" : "no");
    if (!red_saw_blue && !blue_saw_red) {
        std::puts("unexpected: seq_cst forbids this result");
        return 1;
    }
}
