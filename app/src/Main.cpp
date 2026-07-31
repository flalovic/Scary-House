#include <MineApp.hpp>
#include <engine/core/Engine.hpp>

/**
 * Start here...
 */
int main(int argc, char **argv) {
    return std::make_unique<app::MineApp>()->run(argc, argv);
}
