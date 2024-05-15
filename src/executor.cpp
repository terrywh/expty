#include "executor.h"
#include <iostream>

void Executor::run(boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    stage_ = &stages_.front();
    start_read(src);
    start_exec(src, run);
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

void Executor::start_read(boost::asio::posix::stream_descriptor& src) {
    src.async_read_some(boost::asio::buffer(buffer_), [this, &src] (const boost::system::error_code& error, std::size_t size) {
        if (error) return;
        std::cout << "<< " << std::string_view(buffer_.data(), size) << " >>\n";
        output_.append(buffer_.data(), size);
        start_read(src);
    });
}

void Executor::start_exec(boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run) {
    run.expires_after(std::chrono::milliseconds(240));
    run.async_wait([this, &src, &run] (const boost::system::error_code& error) {
        if (error) return;
        
        const Stage* stage = stage_;
        do {
            std::cout << "run " << stage->name() << "\n";
            stage = (*stage)(*this, src);
            if (stage == stage_) {
                start_exec(src, run);
                break; // 匹配无变化时等待更多的输出
            }
            stage_ = stage;
        } while(stage != nullptr);

        if (stage == nullptr) {
            std::cout << "close\n";
            src.close();
        }
    });
}

void Executor::clear() {
    std::cout << "clear\n";
    output_.clear();
}