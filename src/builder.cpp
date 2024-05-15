#include "builder.h"
#include <boost/program_options.hpp>
#include <iostream>

Builder::Builder(int argc, char* argv[]) {
    boost::program_options::options_description desc("命令行说明");
    desc.add_options()
        ("help,h", "输出此帮助信息")
        ("stage,s", boost::program_options::value<std::vector<std::string>>(), "匹配阶段，同阶段仅匹配一条规则")
        ("write,w", boost::program_options::value<std::vector<std::string>>(), "写入/输入")
        ("expect,e", boost::program_options::value<std::vector<std::string>>(), "匹配内容")
        ("next,n", boost::program_options::value<std::vector<bool>>()->zero_tokens(), "进入下一匹配阶段")
        ("goto,g", boost::program_options::value<std::vector<std::string>>(), "跳转指定匹配阶段")
        ("done", boost::program_options::bool_switch(), "结束匹配过程");
    
    auto parsed = boost::program_options::command_line_parser(argc, argv).options(desc).run();
    std::string field, value;
    for (auto& opt: parsed.options) {
        if (opt.string_key == "help") {
            std::cout << desc << "\n";
            ::exit(1);
            return;
        } else if (opt.string_key.empty()) {
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
    std::cout << "append " << field << " => " << value << "\n";
    if (field == "stage") {
        stage_ = &executor_.stages_.emplace_back();
        stage_->name_ = value;
    } else if (field == "expect") {
        entry_ = &stage_->entries_.emplace_back();
        entry_->expect_ = value;
    } else if (field == "write") {
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