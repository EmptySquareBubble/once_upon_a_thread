// The ABA Dragon: CAS proves that the same face is at the gate, not that its
// history is unchanged. Nodes are persistent and links are atomic so this
// teaching run avoids reclamation UB; real lock-free code must also solve
// safe reclamation.
#include <array>
#include <atomic>
#include <cstdio>
#include <thread>

struct Head {
    char name;
    std::atomic<Head*> next{nullptr};
    std::atomic<unsigned> generation{0}; // Observation only; not part of CAS.
};

Head a{'A'}, b{'B'}, c{'C'};
std::atomic<Head*> current_head{nullptr};
std::atomic<unsigned> accepted_aba{0};

void return_head(Head* head) {
    head->generation.fetch_add(1, std::memory_order_relaxed);
    Head* old_head = current_head.load();
    do {
        head->next.store(old_head);
    } while (!current_head.compare_exchange_weak(old_head, head));
}

Head* try_cut_head() {
    Head* old_head = current_head.load();
    if (!old_head)
        return nullptr;

    const unsigned old_generation = old_head->generation.load();
    Head* next_head = old_head->next.load();
    if (current_head.compare_exchange_strong(old_head, next_head)) {
        if (old_head->generation.load() != old_generation)
            ++accepted_aba; // The head left and returned during this attempt.
        return old_head;
    }
    return nullptr;
}

void knight() {
    for (unsigned fight = 0; fight < 50'000; ++fight) {
        if (Head* defeated = try_cut_head()) {
            if ((fight & 63U) == 0)
                std::this_thread::yield();
            return_head(defeated);
        }
    }
}

int main() {
    return_head(&c);
    return_head(&b);
    return_head(&a); // A -> B -> C

    std::array<std::jthread, 4> knights;
    for (auto& fighter : knights)
        fighter = std::jthread(knight);
    for (auto& fighter : knights)
        fighter.join();

    Head* head = current_head.load();
    std::printf("head at gate: %c; ABA accepted during this run: %u\n",
                head ? head->name : '-', accepted_aba.load());
    std::puts("zero is normal: the buggy interleaving is possible, not forced");
}
