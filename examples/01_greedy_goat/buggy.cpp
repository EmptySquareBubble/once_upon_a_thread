// The Greedy Goat — two gates race to update one plain visitor counter.
#include <cstdio>
#include <thread>

constexpr int per_gate = 10'000;
int visitors = 0;

void gate() {
    for (int i = 0; i < per_gate; ++i)
        ++visitors;
}

int main() {
    {
        std::jthread north_gate(gate);
        std::jthread south_gate(gate);
    }

    std::printf("expected: %d\n", 2 * per_gate);
    std::printf("counted:  %d\n", visitors);
}
