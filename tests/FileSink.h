//
// Created by bokket on 24-4-12.
//

#pragma once
#include <string>

#include "../db/status.h"
#include "../file/writableFile.h"

namespace bokket
{

class FileSink final :public WritableFile
{
public:
    FileSink()
            :closed_{false}
    {}

    Status flush() override {
        assert(!closed_);
        return Status::OK();
    }

    Status close() override {
        assert(!closed_);
        closed_ = true;
        return Status::OK();
    }

    Status sync() override {
        assert(!closed_);
        return Status::OK();
    }

    Status append(std::string_view data) override {
        assert(!closed_);
        content_.append(data.data(), data.length());
        return Status::OK();
    }

    const std::string &Content() const {
        return content_;
    }

private:
    std::string content_;
    bool closed_;
};


}
