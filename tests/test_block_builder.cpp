//
// Created by bokket on 2024/2/1.
//

#include <memory>
#include <gtest/gtest.h>

#include "../sstable/block_builder.h"


using namespace bokket;
using namespace std;


TEST(block_builder, add) {
    auto builder=std::make_unique<BlockBuilder>();
    builder->add("key_0","value_0");
    builder->add("key_1","value_1");
    builder->add("key_2","value_2");
    builder->add("key_2","value_2");

    LOG_INFO("{}",builder->data());

    EXPECT_EQ(builder->data().substr(12,12),"key_0value_0");

    auto s=DecodeFixed32(builder->data().substr(8,4).data());
    LOG_INFO("{}",s);

    builder->finish();
    LOG_INFO("{}",builder->data());
    s=DecodeFixed32(builder->data().substr(91,4).data());
    LOG_INFO("{}",s);
    // <record_num><BlockHandle>
    //      4B          8B

    std::string result;
    builder->final(result);
    LOG_INFO("{}",result);
    s=DecodeFixed32(result.substr(99,4).data());
    LOG_INFO("{},{}",s,result.size());
}

TEST(block_builder, restart_point)
{
    auto builder=std::make_unique<BlockBuilder>();
    builder->add("key_0","value_0");
    builder->add("key_1","value_1");
    builder->add("key_2","value_2");
    builder->add("key_2","value_2");
    builder->add("key_1","value_2");

    LOG_INFO("{}",builder->data());

    std::string result;
    builder->final(result);
    LOG_INFO("{}",result);

    std::string_view data{result};
    LOG_INFO("{},{}",data.length(),data.size());
    LOG_INFO("{},{}",result.length(),result.size());
    size_t restarts_len_offset=result.size()- sizeof(int32_t);
    const char* buf=result.data();
    int32_t restarts_len=DecodeFixed32(buf+restarts_len_offset);
    LOG_INFO("{}",restarts_len);

    int32_t restarts_offset_=restarts_len_offset-restarts_len*sizeof(int32_t);
    auto data_buf_=result.substr(0,restarts_offset_);

    LOG_INFO("{}",data_buf_);

    auto s= DecodeFixed32(result.substr(111,4).data());
    LOG_INFO("{}",s);

    s= DecodeFixed32(result.substr(115,4).data());
    LOG_INFO("{}",s);

    s= DecodeFixed32(result.substr(119,4).data());
    LOG_INFO("{}",s);

    s= DecodeFixed32(result.substr(123,4).data());
    LOG_INFO("{}",s);
}

TEST(block_builder, parse_restart_point) {
    auto builder=std::make_unique<BlockBuilder>();
    builder->add("key_0","value_0");
    builder->add("key_1","value_1");
    builder->add("key_2","value_2");
    builder->add("key_2","value_2");
    builder->add("key_1","value_2");

    LOG_INFO("{}",builder->data());

    std::string result;
    builder->final(result);
    LOG_INFO("{}",result);


    size_t restarts_len_offset=result.size()- sizeof(int32_t);
    const char* buf=result.data();

    int32_t restarts_len=DecodeFixed32(buf+restarts_len_offset);
    LOG_INFO("{}",restarts_len);

    int32_t restarts_offset_=restarts_len_offset-restarts_len*sizeof(int32_t);
    auto data_buf_=result.substr(0,restarts_offset_);

    LOG_INFO("{}",data_buf_);

    std::string_view data{data_buf_};
    LOG_INFO("{}",data);

    auto restart_cord=data_buf_.data()+44;

    int32_t shared_key_len=DecodeFixed32(restart_cord);
    LOG_INFO("{}",shared_key_len);
    int32_t unshared_key_len=DecodeFixed32(restart_cord+sizeof(int32_t));

    int32_t value_len= DecodeFixed32(restart_cord+sizeof(int32_t)*2);

    std::string_view restarts_key={restart_cord+ sizeof(int32_t)*3,static_cast<std::string_view::size_type>(unshared_key_len)};
    std::string_view restarts_value={restart_cord+sizeof(int32_t)*3+unshared_key_len,static_cast<std::string_view::size_type>(value_len)};

    LOG_INFO("{},{},{},{}",unshared_key_len,value_len,restarts_key,restarts_value);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}