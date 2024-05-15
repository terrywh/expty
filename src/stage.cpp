#include "stage.h"
#include "executor.h"
#include <boost/asio/write.hpp>
#include <iostream>

const Stage* Stage::operator()(Executor& e, boost::asio::posix::stream_descriptor& s, bool debug) const {
    if (debug)
        std::cerr << ">> stage: '" << name() << "' <<\n";

    const Stage* stage = e.curr();
    for (auto& entry: entries_) {
        stage = entry(e, s, debug);
        if(stage != e.curr()) break; // 准备切换到下一个 Stage 目标
    }
    return stage;
}

const Stage* Entry::operator()(Executor& e, boost::asio::posix::stream_descriptor& s, bool debug) const {
    const Stage* stage = e.curr();
    const std::string& output = e.output();
    bool matched = false;
    if (expect_[0] == '~') {
        // TODO 正则匹配
    } else if (output.find_last_of(expect_) != output.npos) {
        if (debug)
            std::cerr << "  >> match: '" << expect_ << "' <<\n";
        matched = true;
        stage = e.next(next_);
        if (clear_)
            e.clear();
        if (!write_.empty()) {
            if (debug)
                std::cerr << "  >> write: '" << write_ << "' <<\n";
            boost::asio::write(s, boost::asio::buffer(write_));
            boost::asio::write(s, boost::asio::buffer("\r"));
        }
    }
    return stage;
}
