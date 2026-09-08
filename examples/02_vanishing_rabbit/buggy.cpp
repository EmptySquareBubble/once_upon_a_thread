// The Rabbit Hat — individually locked operations do not make the
// check-then-act sequence indivisible.
#include <cstdio>
#include <mutex>
#include <queue>
#include <thread>

struct rabbit {
    int id;
};

class rabbit_hat {
    std::queue<rabbit> hat;
    mutable std::mutex m;

public:
    void add(rabbit r) {
        std::lock_guard lock(m);
        hat.push(r);
    }

    bool is_hat_empty() const {
        std::lock_guard lock(m);
        return hat.empty();
    }

    rabbit hat_pop() {
        std::lock_guard lock(m);
        auto r = hat.front();
        hat.pop();
        return r;
    }
};

int main() {
    rabbit_hat hat;
    hat.add({1});
    hat.add({2});

    auto visitor = [&] {
        if (!hat.is_hat_empty()) {
            auto rabbit_to_pet = hat.hat_pop();
            std::printf("visitor pets rabbit %d\n", rabbit_to_pet.id);
        }
    };

    std::jthread alice(visitor);
    std::jthread bob(visitor);
    std::jthread charlie(visitor);
}
