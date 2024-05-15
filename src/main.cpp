#include "builder.h"
#include "executor.h"
#include <boost/process.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <pty.h>

int main(int argc, char* argv[]) {
    Builder builder(argc - 1, argv + 1);
    Executor executor = builder.build();

    if (argv[1][0] == '-') {
        std::cerr << "error: invalid command '" << argv[1] << "'\n";
        return -1;
    }

    int master;
    char name[256];
    struct winsize winp;
    const int pid = ::forkpty(&master, name, nullptr, &winp);
    if (pid == 0) { // child process
        try {
            return boost::process::system(argv[1]);
        } catch(const std::exception& ex) {
            std::cerr << "error: failed to spawn '" << argv[1] << "'\n";
            return -1;
        }
    } else { // parent process
        boost::asio::io_context io;
        boost::asio::posix::stream_descriptor src(io, master);
        boost::asio::steady_timer run(io);
        executor.run(pid, src, run);
        io.run();
        ::kill(pid, SIGTERM);
        // ::waitpid(pid, nullptr, 0);
        ::wait(nullptr);
    }
    return 0;
}