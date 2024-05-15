#ifndef EXPTY_EXPECT_H
#define EXPTY_EXPECT_H
#include "stage.h"
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <string>
#include <vector>

class Executor {
    std::vector<Stage> stages_;
    const Stage*        stage_;
    std::string            output_;
    std::array<char, 4096> buffer_;
    void start_read(boost::asio::posix::stream_descriptor& s);
    void on_read(const boost::system::error_code& error, std::size_t size);
    void start_exec(boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);

public:
    Executor() = default;
    Executor(Executor&& e) = default;
    ~Executor() = default;

    void run(boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);
    const Stage* curr() const { return stage_; }
    const Stage* next(const std::string& name) const;
    const std::string& output() const { return output_; }
    void clear();

    friend class Builder;
};

#endif // EXPTY_EXPECT_H
