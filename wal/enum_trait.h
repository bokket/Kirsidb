#pragma once

#include <format>
#include <type_traits>



// Sketchy
template<typename T, typename C> 
requires(std::is_enum_v<T>)
struct std::formatter<T, C> : std::formatter<std::underlying_type_t<T>, C> {
    template<typename Ctx>
    auto format(const T& t, Ctx& ctx) const {
        
        return std::formatter<std::underlying_type_t<T>, C>::format(std::underlying_type_t<T>(t), ctx);
    }
};


// template<typename EnumType>
// requires std::is_enum_v<EnumType>
// struct std::formatter<EnumType>: std::formatter<std::underlying_type_t<EnumType>>
// {
//     template<typename FormatContext>
//     auto format(const EnumType& value,FormatContext& ctx) const
//     {
//         return std::formatter<std::underlying_type_t<EnumType>>::format(
//             static_cast<std::underlying_type_t<EnumType>>(value),ctx
//         );
//     }
// };

template<typename T>
constexpr auto format_as(T val)
{
    // The same cast, but all is ok
    //auto v = static_cast<std::underlying_type_t<T>>(val);
    auto v=std::to_underlying(val);
    return v;
}
// using namespace bokket::wal;
// template <>
// struct std::formatter<RecordType> : std::formatter<uint8_t> {
//     auto format(RecordType f, auto& ctx) const{
//         //return formatter<uint8_t>::format(static_cast<uint8_t>(f), ctx);
//         return formatter<uint8_t>::format(std::to_underlying(f), ctx);
//   }
// };

