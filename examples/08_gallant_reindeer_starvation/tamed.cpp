// The Gallant Reindeer, third rule tamed — once the other direction waits,
// new arrivals stop joining the convoy and the empty bridge is handed over.
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>

enum class direction { west, east };

class fair_bridge {
    std::mutex bridge_mutex;
    std::condition_variable bridge_changed;
    std::array<int, 2> waiting{};
    direction turn = direction::west;
    direction traffic = direction::west;
    int reindeer_on_bridge = 0;
    bool empty = true;

    static int index(direction value) {
        return value == direction::west ? 0 : 1;
    }

    static direction opposite(direction value) {
        return value == direction::west ? direction::east : direction::west;
    }

public:
    void cross(direction mine) {
        const direction other = opposite(mine);
        {
            std::unique_lock lock(bridge_mutex);
            ++waiting[index(mine)];
            bridge_changed.wait(lock, [&] {
                if (empty)
                    return waiting[index(other)] == 0 || turn == mine;
                return traffic == mine && waiting[index(other)] == 0;
            });
            --waiting[index(mine)];
            empty = false;
            traffic = mine;
            ++reindeer_on_bridge;
        }

        // Cross outside the admission lock.

        {
            std::lock_guard lock(bridge_mutex);
            if (--reindeer_on_bridge == 0) {
                empty = true;
                if (waiting[index(other)] > 0)
                    turn = other;
                bridge_changed.notify_all();
            }
        }
    }
};

int main() {
    fair_bridge crossing;
    std::atomic<int> west_crossings = 0;
    std::atomic<int> east_crossings = 0;
    std::vector<std::jthread> herd;

    for (int i = 0; i < 4; ++i) {
        herd.emplace_back([&] {
            for (int crossing_number = 0; crossing_number < 2'000;
                 ++crossing_number) {
                crossing.cross(direction::west);
                ++west_crossings;
            }
        });
    }
    herd.emplace_back([&] {
        for (int crossing_number = 0; crossing_number < 200; ++crossing_number) {
            crossing.cross(direction::east);
            ++east_crossings;
        }
    });
    herd.clear(); // joins every finite worker

    std::printf("westbound crossings: %d\n", west_crossings.load());
    std::printf("eastbound crossings: %d\n", east_crossings.load());
    std::puts("fair handoff completed every requested crossing");
}
