#include "builder.h"
#include "executor.h"
#include <boost/process.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <pty.h>

int main(int argc, char* argv[]) {
    Builder builder(argc, argv);
    Executor executor = builder.build();

    int master;
    char name[256];
    struct winsize winp;
    const int pid = ::forkpty(&master, name, nullptr, &winp);
    if (pid == 0) { // child process
        return boost::process::system("bash -i");
    } else { // parent process
        std::cout << "child process: " << pid << "\n";
        boost::asio::io_context io;
        boost::asio::posix::stream_descriptor src(io, master);
        boost::asio::steady_timer run(io);
        executor.run(src, run);
        io.run();
        ::kill(pid, SIGTERM);
        // ::waitpid(pid, nullptr, 0);
        ::wait(nullptr);
    }
    return 0;
}