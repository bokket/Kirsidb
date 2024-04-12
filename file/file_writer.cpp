//
// Created by bokket on 2024/1/31.
//

#include "file_writer.h"

using namespace bokket;
namespace fs=std::filesystem;

Status RandomAccessFile::read(size_t n, size_t offset, char *dst, std::string* result)  {
    fd_= open(filename_.data(),O_RDONLY);
    if(fd_<0) {
        LOG_INFO("?");
    }
    auto r= pread(fd_,dst,n,static_cast<off_t>(offset));
    *result=std::string {dst,static_cast<std::string::size_type>(r<0?0:n)};

//    LOG_INFO("{}",*result);

    if(r<0) [[unlikely]]
        return Status::IOError("RandomAccessFile read error");
    return Status::OK();
}

Writer::Writer(std::string_view filename)
              :filename_{filename}
{
    file_= fopen(filename.data(),"w");


    if(file_== nullptr) [[unlikely]] {
        LOG_ERROR("Writer::Writer fopen error");
    }
}

Writer::~Writer() noexcept {
    if(file_!= nullptr) {
//        fclose(file_);
//        file_= nullptr;
        close();
    }
}

Status Writer::append(std::string_view data) {
    if(fwrite_unlocked(data.data(),1,data.size(),file_)!=data.size())
        return Status::Corruption("Writer::append fwrite error");

    return Status::OK();
}

Status Writer::sync() {
    if(fflush_unlocked(file_)!=0) {
        //Faster version when locking is not required
        if(::fdatasync(fileno_unlocked(file_))!=0)
            return Status::OK();
    }
    return Status::Corruption("Writer::sync() error");
}

Status Writer::flush() {
    if(fflush_unlocked(file_)!=0)
        return Status::Corruption("Writer::flush() error");

    return Status::OK();
}

Status Writer::close() {
    if(fclose(file_)!=0)
        return Status::Corruption("Writer::close() error");

    file_= nullptr;
    return Status::OK();
}

FileWriter::FileWriter(std::string_view filename)
                      :file_path_{filename}
                      ,pos_{0}
                      ,closed_{false}
{
    //int fd=::open(filename.data(),O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC ,0644);
    int mode=O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC;
//    if (append) {
//        mode|=O_APPEND;
//    } else {
//        mode |= O_TRUNC;
//    }
    fs::path p{filename};
    LOG_INFO("file_writer path :{}",p.c_str());

    if(!fs::exists(p)) {
        fs::create_directories(p.parent_path());
    }

    this->fd_=::open(p.c_str(),mode,0644);
    assert(::access(p.c_str(),F_OK )==F_OK );
}


FileWriter::~FileWriter()  {
    if(!closed_)
        close();
}

Status FileWriter::append(std::string_view data) {
    auto len=data.size();

    if(pos_+len < KBufferSize) {
        ::memset(buf_,0, sizeof(buf_));
        ::memcpy(buf_+pos_,data.data(),len);

        pos_+=len;

        LOG_INFO("{},{},{},{}",buf_, strlen(buf_),pos_,len);

        return Status::OK();
    }

    auto written=KBufferSize-pos_;

    ::memcpy(buf_+pos_,data.data(),written);

    pos_+=written;

    auto left=len-written;

    LOG_INFO("{},{},{}",written,pos_,left);

    auto s=flush();
//    if(s!=DB::OK)
//        return s;
    if(!s)
        return s;

    LOG_INFO("{},{},{}",written,pos_,left);

    if(left>=KBufferSize) {
        auto w=::write(fd_,data.data()+written,left);
        assert(w==left);
        LOG_INFO("{}",data.data()+written);
    } else {
        ::memset(buf_,0, sizeof(buf_));
        ::memcpy(buf_,data.data()+written,left);

        pos_+=left;

        LOG_INFO("{},{},{}",buf_,data.data()+written, strlen(buf_));
    }
    return Status::OK();
}

Status FileWriter::close() {
    if(pos_>0) {
        auto s=flush();
        LOG_INFO("{}", s.ToString());
        return s;
    }

//    int ret=::fdatasync(fd_);
//    LOG_INFO("sync:{}", ret);
//    sync();

    if(fd_!=-1) {
        int ret=::close(fd_);
        if(ret<0)
            return Status::IOError("FileWriter close error");
    }
    closed_= true;
    return Status::OK();
}

Status FileWriter::flush() {
    if(pos_>0) {
//        LOG_INFO("buf_:{}",buf_);

        size_t ret=::write(fd_,buf_,pos_);

        LOG_INFO("ret:{},len:{}",ret,pos_);

        //assert(ret==pos_);

        pos_=0;
    }

    return Status::OK();
}

Status FileWriter::sync() {
    auto s=flush();

    //fsync
    int ret=::fdatasync(fd_);
    LOG_INFO("sync:{}", ret);
    if(ret<0) {
        return Status::Corruption("FileWriter::sync() fdatasync error");
    }

    return Status::OK();
}

//FileWriter::FileWriter(std::string_view path, bool append) {
//    int mode=O_CREAT | O_WRONLY;
//    if (append) {
//        mode|=O_APPEND;
//    } else {
//        mode |= O_TRUNC;
//    }
//    fs::path p{path};
//    LOG_INFO("file_writer path :{}",p.c_str());
//
//    if(!fs::exists(p)) {
//        fs::create_directories(p.parent_path());
//    }
//
//    this->fd_=::open(p.c_str(),mode,0644);
//    assert(::access(p.c_str(),F_OK )==F_OK );
// //   assert(::access(p.c_str(),W_OK)==W_OK );
//}
//
//FileWriter::~FileWriter() {
//    sync();
//
//    ::close(fd_);
//    fd_=-1;
//}
//
//
//
//DB FileWriter::append(std::optional<const char *> data, size_t len, bool flush) {
//    int data_offset={};
//    if (!data.has_value()) {
//        return DB::IoError;
//    }
//    if (len==0)
//        return DB::OK;
//
//    auto remain_buf_size=BUFFER_SIZE-buffer_offset;
//
//    if(len < remain_buf_size) {
//        ::memcpy(buffer+buffer_offset,data.value()+data_offset,len);
//
//        buffer_offset+=len;
//        data_offset+=len;
//    } else {
//        ::memcpy(buffer+buffer_offset,data.value(),remain_buf_size);
//        len-=remain_buf_size;
//        buffer_offset+=remain_buf_size;
//        data_offset+=remain_buf_size;
//
//        auto resp= persist(buffer,BUFFER_SIZE);
//        assert(resp==DB::OK);
//
//        while(len>0) {
//            if(is_buffer_full()) {
//                resp= persist(buffer,BUFFER_SIZE);
//                assert(resp==DB::OK);
//            }
//            auto copy=std::min(len,BUFFER_SIZE-buffer_offset);
//            ::memcpy(buffer+buffer_offset,data.value()+data_offset,copy);
//            data_offset+=copy;
//            len-=copy;
//            buffer_offset+=copy;
//            if(len==0) {
//                break;
//            }
//        }
//    }
//    if(flush)
//        persist(buffer,buffer_offset);
//
//    return DB::OK;
//}
//
//DB FileWriter::flush() {
//    if(buffer_offset>0) {
//        auto resp= persist(buffer,buffer_offset);
//        assert(resp==DB::OK);
//    }
//    return DB::OK;
//}
//
//void FileWriter::sync() {
//    flush();
//    fsync(fd_);
//}
//
//void FileWriter::close() {
//    flush();
//    ::close(fd_);
//    fd_=-1;
//}
//
//DB FileWriter::persist(const char *data, int len) {
//
//    auto ret=::write(fd_,data,len);
//
//    LOG_INFO("ret:{},len:{}",ret,len);
//    assert(ret==len);
//    buffer_offset=0;
//    return DB::OK;
//}




