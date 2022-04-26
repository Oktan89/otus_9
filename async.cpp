#include "async.h"
#include "model.h"
#include "loger.h"

namespace async
{

    handle_t connect(std::size_t bulk)
    {
        auto model = new Model(bulk);
        model->start();
        return model;
    }

    void receive(handle_t handle, const char *data, std::size_t size)
    {

        auto model = static_cast<Model *>(handle);
        model->send(data, size);
    }

    void disconnect(handle_t handle)
    {
        auto model = static_cast<Model *>(handle);
        model->send_end();
        delete model;
    }

}
