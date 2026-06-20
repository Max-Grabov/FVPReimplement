#include "hcb.hpp"

#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace AstralAir
{

namespace Formats
{
HcbFormat::HcbFormat(const std::string &path)
{
  // We set up the memory mapping here, if it fails, we fallback to using new to allocate a buffer,
  // this way we don't just crash for no reason.
  is_mmaped_ = true;
  const std::byte *buffer{nullptr};
  struct stat file_stat
  {
  };
  int fd{};

  // If we fail to open the file, there is probably something catastrophic going on (e.g. no file),
  // so we throw a runtime exception
  fd = open(path.c_str(), O_RDONLY);
  if(fd == -1)
  {
    throw std::runtime_error("Unable to open file! The filepath given was " + path);
  }

  if(fstat(fd, &file_stat) == -1)
  {
    throw std::runtime_error("Unable to get file stats about " + path);
  }

  buffer = reinterpret_cast<const std::byte *>(
      mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0));

  // Fallback to new
  if(buffer == MAP_FAILED)
  {
    buffer = nullptr;
    is_mmaped_ = false;
    buffer = new std::byte[static_cast<size_t>(file_stat.st_size)];
    if(!buffer)
    {
      throw std::runtime_error("Error memory mapping and allocating a block for file data at " +
                               path);
    }

    // Now load the data into the buffer, mmap will already have it since OS will load data into
    // virtual memory, but new obv won't Yes this const cast looks like shit, idk another workaround
    // since the data itself is const and we have to change it to read it in
    read(fd, const_cast<void *>(reinterpret_cast<const void *>(buffer)), sizeof(buffer));
  }

  data_ = std::span<const std::byte>(buffer, static_cast<size_t>(file_stat.st_size));
};

HcbFormat::~HcbFormat()
{
  if(is_mmaped_)
  {
    // We have a pointer to const memory, but munmap takes a normal void *, so we use const cast
    // here to get rid of the constness. We are intending to delete the object anyway since this is
    // the destructor, so this shouldn't be UB by accessing data_.data after (and esp. because it is
    // unmaped)
    munmap(const_cast<void *>(reinterpret_cast<const void *>(data_.data())), data_.size());
  }
  else
  {
    delete data_.data();
  }
}

void HcbFormat::OpenAndRead() {}
} // namespace Formats
} // namespace AstralAir
