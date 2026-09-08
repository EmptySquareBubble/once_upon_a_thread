// The Scavenger Jackal — the zoo remembers a callback after its enclosure closes.
#include <cstdio>
#include <functional>
#include <utility>
#include <vector>

struct food_tray {
    int portions = 0;

    void refill(int portion_count) { portions += portion_count; }
};

class zoo {
    std::vector<std::function<void()>> feeding_callbacks_;

public:
    void on_feeding_time(std::function<void()> callback) {
        feeding_callbacks_.push_back(std::move(callback));
    }

    void announce_feeding_time() {
        for (auto& callback : feeding_callbacks_)
            callback();
    }
};

class jackals {
    food_tray tray;

public:
    void open(zoo& zoo) {
        zoo.on_feeding_time([this] {
            tray.refill(prepare_jackal_meal());
        });
    }

private:
    static int prepare_jackal_meal() { return 21; }
};

int main() {
    zoo owner;
    {
        jackals enclosure;
        enclosure.open(owner);
    }

    owner.announce_feeding_time(); // undefined behaviour: dangling `this`
    std::puts("the late callback returned (it may appear to work)");
}
