// The Gallant Reindeer, third rule — arrivals may join traffic already moving
// their way. A continuing convoy can starve the other direction.
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>

enum class direction { none, west, east };

class bridge {
    std::mutex bridge_mutex;
    std::condition_variable bridge_changed;
    direction traffic = direction::none;
    int reindeer_on_bridge = 0;

public:
    void cross(direction mine) {
        {
            std::unique_lock lock(bridge_mutex);
            bridge_changed.wait(lock, [&] {
                return traffic == direction::none || traffic == mine;
            });
            traffic = mine;
            ++reindeer_on_bridge;
        }

        // Cross outside the admission lock.

        {
            std::lock_guard lock(bridge_mutex);
            if (--reindeer_on_bridge == 0) {
                traffic = direction::none;
                bridge_changed.notify_all();
            }
        }
    }
};

int main() {
    bridge crossing;
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
}
