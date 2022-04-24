#include "async.h"
#include "model.h"
#include <future>
#include <iostream>


namespace async {

handle_t connect(std::size_t bulk) {

    auto model = new Model(bulk);
    model->start();
    return model;
}

void receive(handle_t handle, const char *data, std::size_t size) {

    auto model = static_cast<Model*>(handle);
    // std::cout << " test\n";
    // model->send(data, size);
    // std::cout << "get test\n";
    auto res = std::async(std::launch::async, &Model::send, model, data, size);

}

void disconnect(handle_t handle) {
    auto model = static_cast<Model*>(handle);
    model->send_end(protocol::eof);
    delete model;
}

}
