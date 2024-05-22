#pragma once

#include <archive.h>
#include <archive_entry.h>

#include <entt/entt.hpp>

#include <filesystem>
#include <optional>
#include <span>

#include "misc.hpp"

#define EXPECT_ARCHIVE_OK()                                                    \
  {                                                                            \
    EXPECT(errc == ARCHIVE_OK || errc == ARCHIVE_WARN, "Err {} {}",            \
           archive_errno(archive), archive_error_string(archive));             \
  }

struct Assets {
  Assets(std::filesystem::path path) {
    blob.reserve(1024 * 16);
    archive = archive_read_new();

    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);

    int errc;
    errc = archive_read_open_filename(archive, path.c_str(), 1024);
    EXPECT_ARCHIVE_OK();

    struct archive_entry *entry;
    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
      path = archive_entry_pathname(entry);

      u64 entry_size = archive_entry_size(entry);
      if (entry_size == 0) {
        // Skip all directories, they have size 0
        archive_read_data_skip(archive);
        continue;
      }

      auto resource_name = path.replace_extension("");
      auto hash = entt::hashed_string(resource_name.c_str());
      spdlog::trace("Loading {} with hash {}", resource_name.c_str(),
                    hash.value());

      blob.resize(blob.size() + entry_size);
      u8 *head = blob.data() + blob.size() - entry_size;
      archive_read_data(archive, head, entry_size);

      assets[hash] = std::span(head, entry_size);

      path.clear();
    }

    EXPECT_ARCHIVE_OK();
  }

  std::optional<std::span<u8>> get_binary(entt::hashed_string name) {
    auto it = assets.find(name);
    if (it == assets.cend())
      return std::nullopt;
    else
      return it->second;
  }

  ~Assets() {
    int errc = archive_read_free(archive);
    EXPECT_ARCHIVE_OK();
  }

private:
  std::map<entt::hashed_string, std::span<u8>> assets;
  std::vector<u8> blob;
  struct archive *archive;
};
