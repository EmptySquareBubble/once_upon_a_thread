// The ABA Dragon, tamed with the slide's recommended default: a simple mutex.
// The persistent nodes keep reclamation out of this example; production code
// that truly needs lock-free progress should use a proven algorithm with a
// proven reclamation scheme.
#include <array>
#include <cstdio>
#include <mutex>
#include <thread>

struct Head {
    char name;
    Head* next = nullptr;
};

Head a{'A'}, b{'B'}, c{'C'};
Head* current_head = nullptr;
std::mutex dragon_mutex;

void return_head(Head* head) {
    std::lock_guard lock(dragon_mutex);
    head->next = current_head;
    current_head = head;
}

Head* try_cut_head() {
    std::lock_guard lock(dragon_mutex);
    Head* old_head = current_head;
    if (!old_head)
        return nullptr;
    current_head = old_head->next;
    return old_head;
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

    std::lock_guard lock(dragon_mutex);
    std::printf("head at gate: %c; mutex keeps each cut coherent\n",
                current_head ? current_head->name : '-');
}
