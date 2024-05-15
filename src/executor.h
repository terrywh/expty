#ifndef EXPTY_EXPECT_H
#define EXPTY_EXPECT_H
#include "stage.h"
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>
#include <vector>

class Executor {
    bool debug_ {};
    std::vector<Stage>     stages_;
    const Stage*            stage_;
    std::string            output_;
    std::array<char, 4096> buffer_;
    void on_read(const boost::system::error_code& error, std::size_t size);
    void start_read(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);
    void start_exec(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);
    void start_quit(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);

public:
    Executor(boost::program_options::variables_map& options);
    Executor(Executor&& e) = default;
    ~Executor() = default;

    void run(const int pid, boost::asio::posix::stream_descriptor& src, boost::asio::steady_timer& run);
    const Stage* curr() const { return stage_; }
    const Stage* next(const std::string& name) const;
    const std::string& output() const { return output_; }
    void clear();

    friend class Builder;
};

#endif // EXPTY_EXPECT_H
