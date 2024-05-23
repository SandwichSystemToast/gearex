#pragma once

#include <archive.h>
#include <archive_entry.h>

#include <filesystem>
#include <optional>

#include <stb_image.h>

#include <entt/entt.hpp>

#include "engine/misc.hpp"

#define EXPECT_ARCHIVE_OK()                                                    \
  EXPECT(errc == ARCHIVE_OK || errc == ARCHIVE_WARN, "Err {} {}",              \
         archive_errno(archive), archive_error_string(archive))

namespace engine::assets {

enum struct AssetKind {
  Image,
};

using AssetIndex = entt::hashed_string::hash_type;

struct Asset {
  virtual AssetKind kind() = 0;
  virtual bytes raw() = 0;
  virtual ~Asset() {}

protected:
  Asset() {}
};

struct ImageAsset : Asset {
  ImageAsset(bytes data, u32 width, u32 height, u32 channels)
      : width(width), height(height), channels(channels),
        blob(data.begin(), data.end()) {}

  virtual AssetKind kind() { return AssetKind::Image; }
  virtual bytes raw() { return blob; }

  vu2 dimensions() { return {width, height}; }

  u32 channel_count() { return channels; }

protected:
  u32 width, height, channels;
  std::vector<u8> blob;
};

std::optional<AssetKind> inline get_kind_from_extension(
    const std::filesystem::path &ext) {
  if (ext == ".png" || ext == ".jpg")
    return AssetKind::Image;

  return std::nullopt;
}

struct AssetManager {
  AssetManager() {}

  void load_from_file(std::filesystem::path &&path) {
    std::vector<u8> blob;
    blob.reserve(1024 * 16);

    struct archive *archive = archive_read_new();

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

      auto kind = get_kind_from_extension(path.extension());
      if (!kind.has_value()) {
        spdlog::warn("Resource kind for {} not detected, skipping",
                     path.c_str());
        continue;
      }

      auto resource_name = path.replace_extension("");
      auto hash = entt::hashed_string(resource_name.c_str());
      spdlog::trace("Loading {} with hash {}", resource_name.c_str(),
                    hash.value());

      blob.resize(blob.size() + entry_size);
      u8 *head = blob.data() + blob.size() - entry_size;
      archive_read_data(archive, head, entry_size);

      Asset *out = nullptr;
      switch (*kind) {
      case AssetKind::Image: {
        i32 width, height, channels;
        u8 *data = stbi_load_from_memory(head, entry_size, &width, &height,
                                         &channels, 0);
        out = new ImageAsset(std::span(data, width * height* channels), width, height,
                             channels);
        stbi_image_free(data);
      }
      }

      assets[hash] = out;
      path.clear();
    }

    EXPECT_ARCHIVE_OK();
  }

  ImageAsset *get_image(AssetIndex idx) {
    auto it = assets.find(idx);
    if (it == assets.end())
      return nullptr;
    return (ImageAsset *)it->second;
  }

  AssetManager(AssetManager &&) = delete;
  AssetManager(const AssetManager &) = delete;

  ~AssetManager() {
    for (auto &[_, asset] : assets)
      delete asset;
  }

protected:
  std::map<AssetIndex, Asset *> assets;
};

} // namespace engine::assets
