#include "mapped_file.hpp"

#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <utility>

namespace fvp
{

namespace Utility
{

MappedFile::MappedFile(const std::string_view path, Permissions permissions, CreateFile create_file)
    : path_(path)
{
  properties_.permissions = permissions;
  properties_.create_file_property = create_file;

  file_info_.file_descriptor =
      open(path_.data(),
           O_RDWR | (properties_.create_file_property == CreateFile::CREATE_FILE ? O_CREAT : 0));

  if(file_info_.file_descriptor == -1)
  {
    // We fail to open due to user permissions specified (e.g. no O_CREAT set).
    if(errno == ENOENT)
    {
      throw create_file_exception(
          "File does not exist, but CreateFile::NO_CREATE_FILE was passed!");
    }

    // Other ones
    throw std::runtime_error("Unable to open file! The filepath given was " + path_);
  }

  struct stat file_stat
  {
  };

  if(fstat(file_info_.file_descriptor, &file_stat) == -1)
  {
    throw std::system_error(errno, std::generic_category(),
                            "Unable to get file stats about " + path_);
  }

  file_info_.file_size = file_stat.st_size;

  std::byte *buffer{nullptr};
  buffer = reinterpret_cast<std::byte *>(mmap(0, file_info_.file_size, PROT_READ | PROT_WRITE,
                                              MAP_SHARED, file_info_.file_descriptor, 0));

  if(buffer == MAP_FAILED)
  {
    throw std::system_error(errno, std::generic_category(),
                            "Error memory mapping and allocating a block for file data at " +
                                path_);
  }

  data_ = std::span<std::byte>(buffer, static_cast<size_t>(file_info_.file_size));
};

MappedFile::~MappedFile() { munmap((reinterpret_cast<void *>(data_.data())), data_.size()); }

MappedFile::MappedFile(MappedFile &&other) noexcept
    : data_(std::exchange(other.data_, {})), properties_(other.properties_),
      file_info_(other.file_info_), path_(std::move(other.path_))
{
}

MappedFile &MappedFile::operator=(MappedFile &&other) noexcept
{
  data_ = other.data_;
  other.data_ = {};
  properties_ = other.properties_;
  file_info_ = other.file_info_;
  path_ = std::move(other.path_);

  return *this;
}

void MappedFile::Expand()
{
  std::byte *buffer{nullptr};

  if(ftruncate(file_info_.file_descriptor, file_info_.file_size * 2) == -1)
  {
    throw std::system_error(errno, std::generic_category(),
                            "Unable to truncate file! File is at path: " + path_);
  }

  munmap(reinterpret_cast<void *>(data_.data()), data_.size());
  buffer = reinterpret_cast<std::byte *>(mmap(0, file_info_.file_size * 2, PROT_READ | PROT_WRITE,
                                              MAP_SHARED, file_info_.file_descriptor, 0));

  if(buffer == MAP_FAILED)
  {
    throw std::system_error(errno, std::generic_category(),
                            "Unable to memmap after expanded file! File is at path: " + path_);
  }

  data_ = std::span<std::byte>(buffer, static_cast<size_t>(file_info_.file_size * 2));
  file_info_.file_size *= 2;
}

} // namespace Utility

} // namespace fvp
