#include <iostream>
#include "async.h"
#include <thread>



int main(int, char**)
{
    auto h = async::connect(5);
    auto h2 = async::connect(50);
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    std::thread t = std::thread(async::receive, std::ref(h2), "100\n", 4);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    std::thread t2 = std::thread(async::receive, std::ref(h2), "200\n", 4);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);

    for(std::size_t i = 0; i < 200; ++i)
    {
        async::receive(h2, "666\n", 4);
    }
    async::disconnect(h);
    async::disconnect(h2);
    t.join();
    t2.join();
}