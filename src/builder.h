#ifndef EXPTY_BUILDER_H
#define EXPTY_BUILDER_H
#include "executor.h"
#include <string>

class Builder {
    Executor executor_;
    Stage* stage_ {};
    Entry* entry_ {};

public:
    Builder(int argc, char* argv[]);
    Builder& append(const std::string& field, const std::string& value);
    Executor build();
};

#endif // EXPTY_BUILDER_H
