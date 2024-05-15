#ifndef EXPTY_STAGE_H
#define EXPTY_STAGE_H
#include <boost/asio/posix/stream_descriptor.hpp>
#include <string>
#include <vector>

class Executor;
class Stage;

class Entry {
    std::string expect_;
    std::string  write_;
    std::string   next_;
    bool         clear_;
public:
    const Stage* operator()(Executor& e, boost::asio::posix::stream_descriptor& src) const;
    friend class Builder;
};

class Stage {
    std::string           name_;
    std::vector<Entry> entries_;
public:
    const Stage* operator()(Executor& e, boost::asio::posix::stream_descriptor& src) const;
    const std::string& name() const { return name_; }
    friend class Builder;
};

#endif // EXPTY_STAGE_H
