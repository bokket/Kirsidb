#include <gtest/gtest.h>
#include <string>

#include "../sstable/Buf.h"
#include "../write_batch/write_batch.h"
#include "../write_batch/write_batch_impl.h"

#include "../memtable/internalKeys.h"


using namespace bokket;

class WriteBatchPrinter:public WriteBatch::Handle
{   
    public:
        void Put(const Buf& key,const Buf& value) override
        {
            state_.append("Put(");
            state_.append(key.data(),key.size());
            state_.append(", ");
            state_.append(value.data(),value.size());
            state_.append(")");
        }

        void Delete(const Buf& key) override
        {
            state_.append("Delete(");
            state_.append(key.data(), key.size());
            state_.append(")");
        }

        [[nodiscard]] std::string ToString() const
        {
            return state_;
        }
    private:
        std::string state_;
};

TEST(Basic,Init)
{
    WriteBatchImpl batch;
    WriteBatchPrinter printer;

    auto stat=batch.Iterate(&printer);

    EXPECT_TRUE(stat.OK())<<stat.ToString();

    EXPECT_EQ(printer.ToString(), "");
    EXPECT_EQ(batch.count(),0);
}

TEST(Basic,Multi)
{
    WriteBatchImpl batch;
    WriteBatchPrinter printer;
    batch.PutRecord(Buf("foo"), Buf("bar"));
    batch.DeleteRecord(Buf("box"));
    batch.PutRecord(Buf("baz"), Buf("boo"));

    auto stat=batch.Iterate(&printer);

    EXPECT_TRUE(stat.OK())<<stat.ToString();

    EXPECT_EQ(printer.ToString(), "Put(foo, bar)Delete(box)Put(baz, boo)");
    EXPECT_EQ(batch.count(),3);
}

static std::string PrintContens(WriteBatch* b)
{
    InternalKeyComparator cmp(newByteComparator());
    auto mem=new Memtable(cmp);

    std::string state;
    int count=0;
    auto s=b->InsertInto(mem);

    auto iter=mem->NewIterator();

    for(iter->SeekToFrist();iter->Valid();iter->Next())
    {
        
        ParsedInternalKey ikey;
        EXPECT_TRUE(Parsed(iter->key(), &ikey));

        LOG_INFO("{} | {}| {}",iter->Valid(),iter->key(),iter->value());

        LOG_INFO("{}",ikey.seq_);

        switch (ikey.type_) {
            case ValueType::kTypeDelete:
                state.append("Delete(");
                state.append(ikey.user_key_);
                state.append(")");
                ++count;
                break;
            case ValueType::kTypePut:
                state.append("Put(");
                state.append(ikey.user_key_);
                state.append(", ");
                state.append(iter->value());
                state.append(")");
                ++count;
                break;
        }
        state.append("@");
        state.append(NumberToString(ikey.seq_));
    }

    delete iter;

    delete mem;

    if(s.isOK()==false) {
        state.append("ParseError()");
    } else if(count!=b->Count()) {
        state.append("CountMismatch()");
    }



    return state;
}


TEST(WriteBatchTest, Empty)
{
    WriteBatch batch;
    EXPECT_EQ("", PrintContens(&batch));
    EXPECT_EQ(0, batch.Count());
}


TEST(WriteBatchTest, Multiple)
{
    WriteBatch batch;
    batch.Put(Buf("foo"), Buf("bar"));
    batch.Delete(Buf("box"));
    batch.Put(Buf("baz"), Buf("boo"));

    batch.setSequence(100);

    LOG_INFO("{}",PrintContens(&batch));

    EXPECT_EQ(100, batch.Sequence());
    EXPECT_EQ(3,batch.Count());
    EXPECT_EQ("Put(baz, boo)@102""Delete(box)@101""Put(foo, bar)@100", 
    PrintContens(&batch));
}

TEST(WriteBatchTest,Corruption)
{
    WriteBatch batch;
    batch.Put(Buf("foo"), Buf("bar"));
    batch.Delete(Buf("box"));
    batch.setSequence(200);

    auto contents=batch.Contents();

    LOG_INFO("{}",contents);

    batch.SetContents(std::string(contents.data(),contents.size()-1));

    LOG_INFO("{}",PrintContens(&batch));

    EXPECT_EQ("Put(foo, bar)@200""ParseError()", 
    PrintContens(&batch));
}

TEST(WriteBatchTest,Append)
{
    WriteBatch b1,b2;

    b1.setSequence(200);
    b2.setSequence(300);
    b1.Append(b2);
    EXPECT_EQ("", PrintContens(&b1));


    b2.Put(Buf("a"), Buf("va"));
    b1.Append(b2);
    EXPECT_EQ("Put(a, va)@200", PrintContens(&b1));



    b2.Clear();


    b2.Put(Buf("b"), Buf("vb"));
    b1.Append(b2);
    EXPECT_EQ(
        "Put(a, va)@200"
        "Put(b, vb)@201",
        PrintContens(&b1));


    LOG_INFO("{}",PrintContens(&b1));

    b2.Delete(Buf("foo"));
    b1.Append(b2);

    LOG_INFO("{}",PrintContens(&b1));
    // EXPECT_EQ(
    //     "Put(a, va)@200"
    //     "Put(b, vb)@202"
    //     "Put(b, vb)@201"
    //     "Delete(foo)@203",
    //     PrintContens(&b1));

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}