// The Scavenger Jackal, tamed — weak ownership makes a late callback a no-op.
#include <cstdio>
#include <functional>
#include <memory>
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
    void feed() {
        tray.refill(prepare_jackal_meal());
    }

    int food_remaining() const { return tray.portions; }

private:
    static int prepare_jackal_meal() { return 21; }
};

std::shared_ptr<jackals> open_jackal_enclosure(zoo& owner) {
    auto enclosure = std::make_shared<jackals>();
    std::weak_ptr<jackals> weak = enclosure;
    owner.on_feeding_time([weak] {
        if (auto live = weak.lock())
            live->feed();
    });
    return enclosure;
}

int main() {
    zoo owner;
    auto enclosure = open_jackal_enclosure(owner);

    owner.announce_feeding_time();
    std::printf("jackal food available: %d portions\n",
                enclosure->food_remaining());
    enclosure.reset();
    owner.announce_feeding_time();
    std::puts("enclosure closed; the late callback safely did nothing");
}
