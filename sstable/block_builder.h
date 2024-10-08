//
// Created by bokket on 2024/2/1.
//

#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "format.h"
#include "coding.h"

#include "../Log/Log.h"
#include "../db/status.h"

namespace bokket
{

class BlockBuilder
{
private:
    std::string data_;
    std::string pre_key_;

    bool finished_;

    int32_t record_num_=0;

    std::vector<int32_t> restarts_;
public:
    //restart
    static constexpr int32_t RESTART_INTERVAL=2;

public:
    BlockBuilder();
    ~BlockBuilder()=default;

    void add( std::string_view key,std::string_view value);

    Status finish();

    Status final(std::string& result);

    std::string_view final();

    std::string_view data() {
        return data_;
    }

    void clear();

    auto size() {
        return data_.size();
    }

    [[nodiscard]] bool empty() const;

private:
    auto restart_size() {
        return data_.size();
    }
};

}
