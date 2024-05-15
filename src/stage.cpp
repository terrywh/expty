#include "stage.h"
#include "executor.h"
#include <boost/asio/write.hpp>

const Stage* Stage::operator()(Executor& e, boost::asio::posix::stream_descriptor& s) const {
    const Stage* stage = e.curr();
    for (auto& entry: entries_) {
        stage = entry(e, s);
        if(stage != e.curr()) break; // 准备切换到下一个 Stage 目标
    }
    return stage;
}

const Stage* Entry::operator()(Executor& e, boost::asio::posix::stream_descriptor& s) const {
    const Stage* stage = e.curr();
    const std::string& output = e.output();
    bool matched = false;
    if (expect_[0] == '~') {
        // TODO 正则匹配
    } else if (output.find_last_of(expect_) != output.npos) {
        matched = true;
        stage = e.next(next_);
        if (clear_)
            e.clear();
        if (!write_.empty()) {
            boost::asio::write(s, boost::asio::buffer(write_));
            boost::asio::write(s, boost::asio::buffer("\r"));
        }
    }
    return stage;
}
