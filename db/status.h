//
// Created by bokket on 2024/1/30.
//

#pragma once

#include <iostream>

namespace bokket
{


//using DBStatus = std::variant<int, const char*>;

// 定义一个枚举类型，表示不同的状态码
enum class DB {
    OK =1,
    Delete,
    IoError,
    InvalidArgs,
    ExecFailed,
    Unsupported,
    FormatError,
    NotImpl ,
    NotFound ,
};

//inline std::string_view Status(DB code) {
//    switch (code) {
//        case DB::OK:
//            return "OK";
//        case DB::Delete:
//            return "Delete";
//        case DB::IoError:
//            return "IoError";
//        case DB::InvalidArgs:
//            return "InvalidArgs";
//        case DB::ExecFailed:
//            return "ExecFailed";
//        case DB::Unsupported:
//            return "Unsupported";
//        case DB::FormatError:
//            return "FormatError";
//        case DB::NotImpl:
//            return "NotImpl";
//        case DB::NotFound:
//            return "NotFound";
//    }
//}


struct Status
{
    enum class Code {
        OK = 0,
        Delete,
        IoError,
        Corruption,
        InvalidArgs,
        ExecFailed,
        Unsupported,
        FormatError,
        NotImpl,
        NotFound,
    };

    Status() noexcept=default;

    // copyable
    Status(const Status &rhs) noexcept {
        copy(rhs);
    }

    Status &operator=(const Status &rhs) noexcept {
        copy(rhs);
        return (*this);
    }

    Status &operator=(Status &&) = default;

    ~Status()=default;

    static Status OK() {
        return Status();
    }

    explicit operator bool() const {
        return code_==Code::OK;
    }

    [[nodiscard]] bool isOK() const
    {
        return code_==Code::OK;
    }

    [[nodiscard]] Code code() const {
        return code_;
    }

    [[nodiscard]] std::string ToString() const {
        std::string ret;
        switch (code()) {
            case Code::OK:
                ret = "OK";
                break;
            case Code::Corruption:
                ret = "Corruption";
                break;
            case Code::IoError:
                ret = "IOError";
                break;
            default:
                ret = "Unknown ErrorCode";
        }

        if(msg_.empty())
            return ret;

        return ret + ": " + this->msg_.data();
    }

    static Status IOError(std::string_view msg) {
        return {Code::IoError,msg};
    }

    static Status Corruption(std::string_view msg) {
        return {Code::Corruption,msg};
    }

private:
    Status(Code code,std::string_view msg)
            :code_{code}
            ,msg_{msg}
    {}

    void copy(const Status &rhs) {
        code_=rhs.code_;
        msg_=rhs.msg_;
    }
private:
    std::string_view msg_;
    Code code_;
};


}