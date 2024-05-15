#include "builder.h"
#include <iostream>

Builder::Builder(boost::program_options::variables_map& options,
    boost::program_options::options_description& desc, std::vector<std::string>& rules)
: executor_(options) {
    auto parsed = boost::program_options::command_line_parser(rules).options(desc).run();
    std::string field, value;
    for (auto& opt: parsed.options) {
        if (opt.string_key.empty()) {
            value.push_back(' ');
            value.append(opt.value[0]);
        } else {
            if (!field.empty()) {
                append(field, value);
                value.clear();
            }
            field = opt.string_key;
            if (!opt.value.empty()) 
                value = opt.value[0];
        }
    }
    if (!field.empty())
        append(field, value);
}

Builder& Builder::append(const std::string& field, const std::string& value) {
    // std::cout << "append " << field << " => " << value << "\n";
    if (field == "stage") {
        stage_ = &executor_.stages_.emplace_back();
        stage_->name_ = value;
        return *this;
    }
    if (!stage_) {
        std::cerr << "error: invalid expect def\n";
        return *this;
    }
    if (field == "expect") {
        entry_ = &stage_->entries_.emplace_back();
        entry_->expect_ = value;
        return *this;
    }
    if (!entry_) {
        std::cerr << "error: invalid expect def\n";
        return *this;
    }
    if (field == "write") {
        entry_->write_ = value;
    } else if (field == "next") {
        entry_->next_ = value;
        entry_->clear_ = true;
    } else if (field == "goto") {
        entry_->next_ = value;
        entry_->clear_ = true;
    } else if (field == "done") {
        entry_->next_ = "<:done:>";
        entry_->clear_ = true;
    }
    return *this;
}

Executor Builder::build() {
    return std::move(executor_);
}