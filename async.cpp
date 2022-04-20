#include "async.h"

namespace async {

handle_t connect([[maybe_unused]]std::size_t bulk) {
    return nullptr;
}

void receive([[maybe_unused]]handle_t handle, [[maybe_unused]]const char *data, [[maybe_unused]]std::size_t size) {
}

void disconnect([[maybe_unused]]handle_t handle) {
}

}
