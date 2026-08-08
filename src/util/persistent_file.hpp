#pragma once

#include <cstddef>
#include <span>
#include <optional>
#include <stdexcept>
#include <fcntl.h>

#include "binary_stream_util.hpp"

namespace fvp
{

namespace Utility
{

class PersistentFile
{
public:
  // These are public so users can set these
  enum class Permissions
  {
    READ,
    READ_WRITE
  };

  enum class CreateFile
  {
    CREATE_FILE,
    NO_CREATE_FILE
  }; 

  class create_file_exception : std::exception
  {
  private:
    std::string_view message_;

  public:
    explicit create_file_exception(std::string_view msg) : message_(msg) {}
    const char *what() const noexcept override
    {
      return message_.data();
    }
  };

  class no_write_allowed_exception : std::exception
  {
  private:
    std::string_view message_;

  public:
    explicit no_write_allowed_exception(std::string_view msg) : message_(msg) {}
    const char *what() const noexcept override
    {
      return message_.data();
    }
  };

  static std::optional<PersistentFile> Construct(const std::string_view path, Permissions permissions, CreateFile create_file);
private: 
  // Controlled on the class so it is private here
  enum class Mapped
  {
    IS_MAPPED,
    IS_NOT_MAPPED
  };

  struct FileInfo
  {
    int file_descriptor;
    size_t file_size;
  };

  struct FileProperties
  {
    Permissions permissions;
    CreateFile create_file_property;
    Mapped map_property;
  }; 

  struct Key
  {
  private:
    Key() = default;
    friend PersistentFile;
  };

  using BinaryStream = std::span<std::byte>;
  using ConstBinaryStream = std::span<const std::byte>;

  BinaryStream data_;
  FileProperties properties_;
  FileInfo file_info_;
  std::string path_;

  // This is used for when dynamically allocated files need to be expanded to a bigger region of memory for writing purposes. This will also work for mmaped files by unmapping and remapping the data. 
  void Expand(); 

public:  
  PersistentFile(Key, const std::string_view path, Permissions permissions, CreateFile create_file);
  ~PersistentFile();
  PersistentFile(const PersistentFile &) = delete;
  PersistentFile(PersistentFile &&) noexcept;
  PersistentFile &operator=(const PersistentFile &) = delete;
  PersistentFile &operator=(PersistentFile &&) noexcept;

  inline ConstBinaryStream Data() const noexcept { return data_; }

  // Should be a size_t parameter, but not sure if I will need to use integers, shorts etc. This way atleast it allows those types and allows for checks to ensure no negative values
  template <Gettable T, typename P> [[nodiscard]] inline T Get(P offset) const
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get<T>(data_, offset); 
  } 

  template <typename P> [[nodiscard]] inline ConstBinaryStream Get(P offset, size_t size) const
  {
    static_assert(std::is_integral_v<P>);
    if(offset < 0) return {};
    return Utility::Get(data_, offset, size); 
  }

  template <Gettable T, typename P> [[nodiscard]] inline T GetAndIncrement(P &offset) const
  {
    static_assert(std::is_integral_v<P>);
    T value{Get<T>(offset)};
    offset += sizeof(T);
    return value;
  }
  
  template <Gettable T> inline void Write(size_t offset, const T &value)
  {
    if(properties_.permissions < Permissions::READ_WRITE)
    {
      throw no_write_allowed_exception("Attempted to write to a file open in read only mode! Path is: " + path_); 
    }

    try
    {
      Utility::Write<T>(data_, offset, value);
    }
    // Only out of range error, on error, expand the file and then call Write() again. the stream shouldn't be nullptr *probably
    catch (std::out_of_range &r)
    {
      Expand();
      Write(offset, value);
    } 
  }
};
} // namespace Formats

} // namespace fvp
