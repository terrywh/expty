#ifndef EXPTY_BUILDER_H
#define EXPTY_BUILDER_H
#include "executor.h"
#include <boost/program_options.hpp>
#include <string>

class Builder {
    Executor executor_;
    Stage* stage_ {};
    Entry* entry_ {};

public:
    Builder(boost::program_options::variables_map& options, 
        boost::program_options::options_description& desc, std::vector<std::string>& rules);
    Builder& append(const std::string& field, const std::string& value);
    Executor build();
};

#endif // EXPTY_BUILDER_H
