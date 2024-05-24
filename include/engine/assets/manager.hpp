#pragma once

#include <archive.h>
#include <archive_entry.h>

#include <filesystem>
#include <functional>

#include <map>
#include <memory>

#include <entt/entt.hpp>

#include "engine/misc.hpp"

#define EXPECT_ARCHIVE_OK()                                                    \
  EXPECT(errc == ARCHIVE_OK || errc == ARCHIVE_WARN, "Err {} {}",              \
         archive_errno(archive), archive_error_string(archive))

namespace engine::assets {

template <typename A> struct resolve_asset {};

struct AssetManager;

using AssetIndex = hash;

using BinaryAssetResolver = std::function<void(
    AssetManager &manager, std::string_view asset_path, std::span<u8> raw)>;

template <typename A> struct AssetHandle : std::shared_ptr<A> {
  using std::shared_ptr<A>::shared_ptr;

  operator A*() {
    return this->get();
  }
};

struct AssetManager {
  AssetManager() {}

  template <typename A> void register_asset_type() {
    using resolver = resolve_asset<A>;
    for (auto ext : resolver::extensions)
      resolvers[entt::hashed_string(ext)] = resolver::resolve;
  }

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

      auto resolver_it =
          resolvers.find(entt::hashed_string(path.extension().c_str()));
      if (resolver_it == resolvers.end()) {
        spdlog::warn("Resolver for asset {} not detected, skipping",
                     path.c_str());
        continue;
      }
      auto &resolver = resolver_it->second;

      auto asset_name = path.replace_extension("");
      auto hash = entt::hashed_string(asset_name.c_str());
      spdlog::trace("Loading {} with hash {}", asset_name.c_str(),
                    hash.value());

      blob.resize(blob.size() + entry_size);
      u8 *head = blob.data() + blob.size() - entry_size;
      archive_read_data(archive, head, entry_size);

      resolver(*this, asset_name.c_str(), {head, entry_size});

      path.clear();
    }

    EXPECT_ARCHIVE_OK();
  }

  template <typename A> AssetHandle<A> add_asset(AssetIndex index, A *asset) {
    auto type_index = entt::type_id<A>().index();
    auto it = asset_map.find(type_index);
    if (it == asset_map.end())
      asset_map[type_index] = new erased_map<A>();

    auto handle = AssetHandle<A>(asset);
    ((erased_map<A> *)asset_map[type_index])->insert_or_assign(index, handle);
    return handle;
  }

  template <typename A> AssetHandle<A> get_asset(AssetIndex index) {
    auto type_index = entt::type_id<A>().index();
    return ((erased_map<A> *)asset_map[type_index])->at(index);
  }

  AssetManager(AssetManager &&) = delete;
  AssetManager(const AssetManager &) = delete;

protected:
  template <typename A> using erased_map = std::map<AssetIndex, AssetHandle<A>>;

  std::map<hash, BinaryAssetResolver> resolvers;

  /// @brief Associates a type-erased `std::map<AssetIndex, T>` with each
  /// `entt::type_id<T>()`
  // TODO: better type erasure?
  std::map<entt::id_type, void *> asset_map;
};

} // namespace engine::assets
