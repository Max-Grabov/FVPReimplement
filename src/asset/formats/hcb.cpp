#include "hcb.hpp"

#include "util/binary_stream_util.hpp"

#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdint>
#include <vector>
#include <string_view>

namespace fvp
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

void HcbFormat::GetSysCallMap()
{
  meta_data_.sys_call_start = Utility::Get<uint32_t>(data_, 0); 

  if(data_.size() < meta_data_.sys_call_start)
  {
    return;
  }

  uint32_t ptr{meta_data_.sys_call_start + 10};
  ptr += Utility::Get<uint8_t>(data_, ptr);
  ++ptr;
  uint16_t function_count{Utility::Get<uint16_t>(data_, ptr)};
  ptr += 2; 

  uint8_t name_length{};

  // For now just dump it in some std::vector idk
  std::vector<std::string_view> function_names(function_count);
  for(uint16_t i{}; i < function_count; ++i)
  {
    // Every function seems to have a byte representing length and some other byte, not sure what the other one is yet...
    // For now skip it
    ++ptr;
    name_length = Utility::Get<uint8_t>(data_, ptr++); 
    function_names[i] = std::string_view(reinterpret_cast<const char *>(data_.data() + ptr), name_length);
    ptr += name_length;
  }

  for(const std::string_view s : function_names)
  {
    std::cout << s << "\n";
  }
}
} // namespace Formats
} // namespace fvp
