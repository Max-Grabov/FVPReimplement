#include "persistent_file.hpp"

#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace fvp
{

namespace Formats
{

// TODO Don't throw runtime errors. Instead return a std::span with a null ptr and 0 size, so we can check for invalid opens on read only files easily instead of crashing. Also make options work for both mmap and fread
PersistentFile::PersistentFile(const std::string_view path, int file_options) : path_(path), file_options_(file_options)
{
  // We set up the memory mapping here, if it fails, we fallback to using new to allocate a buffer,
  // this way we don't just crash for no reason.
  is_mmaped_ = true; 

  file_info_.file_descriptor = open(path_.data(), file_options); 
  if(file_info_.file_descriptor == -1)
  {
    throw std::runtime_error(std::string("Unable to open file! The filepath given was ") + path_);
  }

  struct stat file_stat
  {
  };

  if(fstat(file_info_.file_descriptor, &file_stat) == -1)
  {
    throw std::runtime_error(std::string("Unable to get file stats about ") + std::string(path_));
  } 

  file_info_.file_size = file_stat.st_size;

  // TODO add options to parameter list, or add it to class members*
  std::byte *buffer{nullptr};  

  buffer = reinterpret_cast<std::byte *>(
      mmap(0, file_info_.file_size, PROT_READ, MAP_SHARED, file_info_.file_descriptor, 0));

  // Fallback to new
  if(buffer == MAP_FAILED)
  {
    buffer = nullptr;
    is_mmaped_ = false;
    buffer = new std::byte[static_cast<size_t>(file_info_.file_size)];
    if(!buffer)
    {
      throw std::runtime_error(std::string("Error memory mapping and allocating a block for file data at ") + path_);
    }
 
    read(file_info_.file_descriptor, reinterpret_cast<void *>(buffer), sizeof(buffer));
  }
  
  data_ = std::span<std::byte>(buffer, static_cast<size_t>(file_info_.file_size)); 
};

PersistentFile::~PersistentFile()
{
  if(is_mmaped_)
  { 
    munmap((reinterpret_cast<void *>(data_.data())), data_.size());
  }
  else
  {
    delete[] data_.data();
  }
}

void PersistentFile::Expand()
{
  std::byte *buffer{nullptr}; 

  if(ftruncate(file_info_.file_descriptor, file_info_.file_size * 2) == -1)
  {
    throw std::system_error(errno, std::generic_category(), "Unable to truncate file! File is at path: " + path_); 
  } 

  if(is_mmaped_)
  {
    munmap(reinterpret_cast<void *>(data_.data()), data_.size());
    buffer = reinterpret_cast<std::byte *>(
      mmap(0, file_info_.file_size * 2, PROT_READ | PROT_WRITE, MAP_SHARED, file_info_.file_descriptor, 0));  

    if(buffer == MAP_FAILED)
    {
      throw std::system_error(errno, std::generic_category(), "Unable to memmap after expanded file! File is at path: " + path_);
    }
  }
  else
  {
    buffer = new std::byte[static_cast<size_t>(file_info_.file_size * 2)];
    if(!buffer)
    {
      throw std::system_error(errno, std::generic_category(), "Unable to alloc more after expanded file! File is at path: " + path_);
    } 
    memcpy(buffer, data_.data(), data_.size()); 
    delete[] data_.data();
  } 

  data_ = std::span<std::byte>(buffer, static_cast<size_t>(file_info_.file_size * 2));
  file_info_.file_size *= 2;
}

} // namespace Formats

} // namespace fvp
