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
    std::vector<int32_t> restarts_;
    std::string pre_key_;
    std::string data_;

    int32_t record_num_=0;
public:
    //restart
    static constexpr int32_t RESTART_INTERVAL=16;

public:
    BlockBuilder()=default;
    ~BlockBuilder()=default;

    DB add(std::string_view key,std::string_view value);

    DB finish();

    DB final(std::string& result);

    std::string_view data() {
        return data_;
    }

    void clear();

    auto size() {
        return data_.size();
    }

private:
    auto restart_size() {
        return data_.size();
    }
};

}
