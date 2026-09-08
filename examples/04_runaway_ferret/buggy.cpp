// The Runaway Ferret — the detached keeper owns a view, not its characters.
#include <cstdio>
#include <string>
#include <string_view>
#include <thread>

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

    std::thread keeper([menu = std::string_view{menu}] {
        follow_menu(menu);
    });
    keeper.detach();

    menu = prepare_tomorrow_menu();
}

int main() {
    manage_zoo();
}
