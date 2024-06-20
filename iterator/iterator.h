#pragma once

#include "../db/status.h"
#include "../sstable/Buf.h"

namespace bokket 
{

class Iterator
{
    public:
        Iterator()=default;

        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;

        virtual ~Iterator()=default;

        [[nodiscard]] virtual bool Valid() const = 0;

        // virtual void SeekToFirst() = 0;

        // virtual void SeekToLast() = 0;

        virtual void Seek(const Buf& target) = 0;

        virtual void SeekToFrist() = 0;

        virtual void Next() = 0;

        [[nodiscard]] virtual std::string key() const = 0;

        [[nodiscard]] virtual std::string value() const = 0;

        [[nodiscard]] virtual Status status() const = 0;
};


}
