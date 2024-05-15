#include "builder.h"
#include "executor.h"
#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <pty.h>

int main(int argc, char* argv[]) {

    boost::program_options::options_description desc("全局选项");
    desc.add_options()
        ("help,h", "显示帮助信息")
        ("debug", "执行过程");
    boost::program_options::variables_map vm;
    boost::program_options::parsed_options options = 
        boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    boost::program_options::store(options, vm);

    boost::program_options::options_description rule("规则定义");
    rule.add_options()
        ("stage,s", boost::program_options::value<std::vector<std::string>>(), "匹配阶段，同阶段仅匹配一条规则")
        ("write,w", boost::program_options::value<std::vector<std::string>>(), "写入/输入")
        ("expect,e", boost::program_options::value<std::vector<std::string>>(), "匹配内容")
        ("next,n", boost::program_options::value<std::vector<bool>>()->zero_tokens(), "进入下一匹配阶段")
        ("goto,g", boost::program_options::value<std::vector<std::string>>(), "跳转指定匹配阶段")
        ("done", boost::program_options::bool_switch(), "结束匹配过程");

    if (vm.count("help") > 0) {
        std::cout << "命令用法：\n  " << argv[0] << " [全局选项] <COMMAND> [规则定义]\n\n";
        std::cout << desc << "\n";
        std::cout << rule << "\n";
        return 1;
    }
    std::vector<std::string> rules = 
        boost::program_options::collect_unrecognized(options.options, boost::program_options::include_positional);

    Builder builder(vm, rule, rules);
    Executor executor = builder.build();

    int master;
    char name[256];
    struct winsize winp;
    const int pid = ::forkpty(&master, name, nullptr, &winp);
    if (pid == 0) { // child process
        try {
            return boost::process::system(rules[0]);
        } catch(const std::exception& ex) {
            std::cerr << "错误: 执行 '" << rules[0] << "' 失败 (" << ex.what() << ")\n";
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