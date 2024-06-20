//
// Created by bokket on 24-3-26.
//

#include "../sstable/footer.h"

#include <gtest/gtest.h>

using namespace std;
using namespace bokket;


TEST(footer,encode)
{
    BlockHandle metaBlock{1,2};
    BlockHandle indexBlock{3,4};

    Footer footer{metaBlock,indexBlock};
    std::string s=footer.EncodeToString();

    LOG_INFO("s:{}",s);

    //std::string_view b=s;
    auto status=footer.DecodeFrom(s,footer);

    LOG_INFO("{}",status.ToString());

    footer.DebugString();
//    LOG_INFO("{}",footer.DebugString());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
