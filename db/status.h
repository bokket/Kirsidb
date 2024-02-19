//
// Created by bokket on 2024/1/30.
//

#pragma once

#include <variant>
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

inline std::string_view Status(DB code) {
    switch (code) {
        case DB::OK:
            return "OK";
        case DB::Delete:
            return "Delete";
        case DB::IoError:
            return "IoError";
        case DB::InvalidArgs:
            return "InvalidArgs";
        case DB::ExecFailed:
            return "ExecFailed";
        case DB::Unsupported:
            return "Unsupported";
        case DB::FormatError:
            return "FormatError";
        case DB::NotImpl:
            return "NotImpl";
        case DB::NotFound:
            return "NotFound";
    }
}


}