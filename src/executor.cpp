#include "executor.h"
#include <iostream>
#include <regex>

Executor::Executor(boost::program_options::variables_map& options)
: debug_(options.count("debug") > 0) {}

void Executor::run(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    stage_ = &stages_.front();
    start_read(pid, src, run);
    start_exec(pid, src, run);
}

const Stage* Executor::next(const std::string& name) const {
    int c = 0;
    for (int i=0;i<stages_.size();++i) {
        if (stages_[i].name() == name) {
            return &stages_[i];
        } else if (stage_ == &stages_[i]) {
            c = i;
        }
    }
    if (stages_.size() > c + 1 && name.empty())
        return &stages_[c+1];
    else 
        return nullptr;
}

const std::regex anisEscape (R"REGEX([\u001B\u009B][[\]()#;?]*(?:(?:(?:(?:;[-a-zA-Z\d\/#&.:=?%@~_]+)*|[a-zA-Z\d]+(?:;[-a-zA-Z\d\/#&.:=?%@~_]*)*)?\u0007)|(?:(?:\d{1,4}(?:;\d{0,4})*)?[\dA-PR-TZcf-nq-uy=><~])))REGEX");

void Executor::start_read(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    src.async_read_some(boost::asio::buffer(buffer_), [this, pid, &src, &run] (const boost::system::error_code& error, std::size_t size) {
        if (error) {
            src.close();
            ::kill(pid, SIGTERM);
            run.cancel();
            return;
        }
        // 原始数据中可能包含入颜色、光标移动的控制符号
        std::cout << std::string_view(buffer_.data(), size);
        // 通过正则将可能的控制符号去除（用于匹配）
        buffer_.data()[size] = '\0';
        output_.append(std::regex_replace(buffer_.data(), anisEscape, ""));
        start_read(pid, src, run);
    });
}

void Executor::start_exec(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    run.expires_after(std::chrono::milliseconds(240));
    run.async_wait([this, pid, &src, &run] (const boost::system::error_code& error) {
        if (error) return;
        
        const Stage* stage = stage_;
        do {
            stage = (*stage)(*this, src, debug_);
            if (stage == stage_) {
                start_exec(pid, src, run);
                break; // 匹配无变化时等待更多的输出
            }
            stage_ = stage;
        } while(stage != nullptr);

        if (stage == nullptr)
            start_quit(pid, src, run);
    });
}

void Executor::start_quit(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    run.expires_after(std::chrono::seconds(10));
    run.async_wait([this, pid, &src] (const boost::system::error_code& error) {
        if (error) return;
        if (src.is_open()) {
            ::kill(pid, SIGKILL);
        }
        return;
    });
}

void Executor::clear() {
    output_.clear();
}