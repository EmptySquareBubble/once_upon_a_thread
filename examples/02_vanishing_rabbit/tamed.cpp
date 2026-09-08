// The Rabbit Hat, tamed — checking and taking a rabbit is one locked operation.
#include <cstdio>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

struct rabbit {
    int id;
};

class rabbit_hat {
    std::queue<rabbit> hat;
    std::mutex m;

public:
    void add(rabbit r) {
        std::lock_guard lock(m);
        hat.push(r);
    }

    std::optional<rabbit> try_pull_rabbit() {
        std::lock_guard lock(m);
        if (hat.empty())
            return {};
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
        if (auto rabbit_to_pet = hat.try_pull_rabbit())
            std::printf("visitor pets rabbit %d\n", rabbit_to_pet->id);
        else
            std::puts("the hat is empty");
    };

    std::jthread alice(visitor);
    std::jthread bob(visitor);
    std::jthread charlie(visitor);
}
