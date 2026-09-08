// The Runaway Ferret, tamed — the keeper owns both the string and the thread.
#include <cstdio>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

std::string load_menu() {
    return "feed the ferret five portions";
}

std::string prepare_tomorrow_menu() {
    return "feed the parrots four portions";
}

void follow_menu(std::string_view menu) {
    std::printf("following menu: %.*s\n",
                static_cast<int>(menu.size()), menu.data());
}

void manage_zoo() {
    std::string menu = load_menu();

    std::jthread keeper([menu = std::move(menu)] {
        follow_menu(menu);
    });

    menu = prepare_tomorrow_menu();
}

int main() {
    manage_zoo();
}
