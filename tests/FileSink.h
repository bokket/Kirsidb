//
// Created by bokket on 24-4-12.
//

#pragma once
#include <cassert>
#include <cstring>
#include <string>
#include <utility>


#include "../Log/Log.h"
#include "../db/status.h"
#include "../file/readableFile.h"
#include "../file/writableFile.h"

namespace bokket
{

class FileSource final : public ReadableFile
{
public:
    explicit FileSource(std::string content)
                        :content_(std::move(content)) {}

    Status read(uint64_t offset, size_t n, std::string *result) override {
        assert(offset < content_.length());

        n = (offset + n < content_.length() ? n : content_.length() - offset);
        memcpy(result->data(), content_.data() + offset, n);
//        (*result) = std::string (dst, n);

        LOG_INFO("{}",*result);

        return Status::OK();
    }

private:
    std::string content_;
};

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
