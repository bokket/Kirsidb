#include "tinyio.h"

using namespace tinyio;

bokket::Status fileManager::NewRandomAccessFile(std::string_view filename,bokket::ReadableFile **result)
{
    *result=nullptr;
    bokket::Status s;

    LOG_INFO("{}",filename);

    auto fd=::open(filename.data(), O_RDONLY);
    if(fd<0) {
        return bokket::Status::IOError(std::strerror(errno));
    }

    auto file_size=bokket::FileTool::GetFileSize(filename);
    LOG_INFO("{}",file_size);

    auto mmap_base=::mmap(nullptr, file_size, PROT_READ, MAP_SHARED, fd, 0);
    if(mmap_base==MAP_FAILED)
    {
        LOG_INFO("{}",s.ToString());
        s=bokket::Status::IOError(std::strerror(errno));
    }else {
        *result=new bokket::MmapReaderFile(
            filename,static_cast<char*>(mmap_base),file_size
        );
    }
    ::close(fd);
    LOG_INFO("{}",s.ToString());
    return s;
}

namespace tinyio::detail 
{

inline std::error_code last_error() noexcept
{
    std::error_code error;

    error.assign(errno, std::system_category());

    return error;
}

inline int open_file(const fs::path& path,const access_mode& mode,std::error_code& error)
{
    error.clear();

    if(path.empty()) {
        error=std::make_error_code(std::errc::invalid_argument);
        return -1;
    }
    
    const auto fd=::open(path.c_str(),
     (mode==access_mode::read?O_RDONLY:O_RDWR) | O_CREAT ,S_IRUSR | S_IWUSR);

    if(fd==-1) {
        error=last_error();
    }

    return fd;
}

inline int open_file(const fs::path& path,const access_mode& mode)
{
    std::error_code error;
    
    auto fd=open_file(path,mode,error);
    if(error) {
        throw std::ios_base::failure{"tinyio::open_file error",error};
    }
    return fd;
}

inline size_t file_size(int fd,std::error_code& error)
{
    error.clear();

    struct stat file_stat{};
    if(::fstat(fd, &file_stat)==-1) {
        error=last_error();
        return 0;
    }
    return file_stat.st_size;
}

inline size_t file_size(std::string_view path,std::error_code& error)
{
    error.clear();

    auto size=fs::file_size(path,error);
    if(error) {
        throw std::ios_base::failure{"tinyio::file_size error",error};
    }
    return size;
}


}

File::~File()
{
    close();
}

void File::open(const fs::path& path,const access_mode& mode)
{
    close();
    
    fd_=detail::open_file(path,mode);
}

void File::close()
{
    if(fd_!=-1) {
        ::close(fd_);

        fd_=-1;
    }
}

uint64_t File::size() const
{
    std::error_code error;
    auto size=detail::file_size(fd_,error);
    if(error) {
        throw std::ios_base::failure{"tinyio::file::size error",error};
    }
    return size;
}

size_t File::read(void *buf,size_t size,std::error_code& error) const
{   
    error.clear();
    auto read_size=::read(fd_, buf, size);
    if(read_size<0) {
        error=detail::last_error();
        return 0;
    }
    return read_size;
}

size_t File::read(void *buf,size_t size) const
{
    std::error_code error;
    auto res=read(buf,size,error);
    if(error) {
        throw std::ios_base::failure{"tinyio::file::read",error};
    }
    return res;
}

size_t File::write(const void* buf,size_t size,std::error_code& error) const
{
    error.clear();

    auto write_len=::write(fd_, buf, size);
    if(write_len<=0) {
        error=detail::last_error();
        return 0;
    }
    return write_len;
}

size_t File::write(const void* buf,size_t size) const
{
    std::error_code error;
    auto res=write(buf, size,error);
    if(error) {
        throw std::ios_base::failure{"tinyio::file::write",error};
    }
    return res;
}


void File::seekg(uint64_t pos,std::error_code& error)
{
    error.clear();

    auto cur_pos=::lseek(fd_, static_cast<off_t>(pos), SEEK_SET);

    if(cur_pos==-1) {
        error=detail::last_error();
    }
}

void File::seekg(uint64_t pos)
{
    std::error_code error;
    seekg(pos,error);

    if(error) {
        throw std::ios_base::failure{"tinyio::file::seekg",error};
    }
}

void File::sync(std::error_code& error) const
{
    error.clear();
    //https://www.bigran.cn/2022/12/21/rxkya8wv/
    auto res=::fdatasync(fd_);
    if(res==-1) {
        error=detail::last_error();
    }
}

void File::sync() const
{
    std::error_code error;
    sync(error);

    if(error) {
        throw std::ios_base::failure{"tinyio::file::sync",error};
    }
}