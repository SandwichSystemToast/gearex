#pragma once

#include <string>

#include "engine/misc.hpp"
#include "manager.hpp"

#include <stb_image.h>

namespace engine::assets {

struct ShaderSourceAsset {
  std::string name;
  std::string source;

  ShaderSourceAsset(std::string_view name, std::string_view source)
      : name(name), source(source) {}
};

struct ImageAsset {
  vu2 dimensions;
  u32 channels;
  std::vector<u8> raw;

  ImageAsset(u8 *raw, vu2 dimensions, u32 channels)
      : dimensions(dimensions), channels(channels) {
    z size = dimensions.x * dimensions.y * channels;
    this->raw.resize(size);
    std::memcpy(this->raw.data(), raw, size);
  }
};

template <> struct resolve_asset<ImageAsset> {
  static constexpr std::array<const char *, 2> extensions = {".png", ".jpg"};
  static void resolve(AssetManager &assets, std::string_view asset_name,
                      std::span<u8> raw) {
    i32 width, height, channels;
    u8 *raw_image = stbi_load_from_memory(raw.data(), raw.size_bytes(), &width,
                                          &height, &channels, 0);
    assets.add_asset(entt::hashed_string(asset_name.data()).value(),
                     new ImageAsset(raw_image, {width, height}, channels));
    stbi_image_free(raw_image);
  }
};

template <> struct resolve_asset<ShaderSourceAsset> {
  static constexpr std::array<const char *, 1> extensions = {".glsl"};
  static void resolve(AssetManager &assets, std::string_view asset_name,
                      std::span<u8> raw) {
    std::string name(asset_name.begin(), asset_name.end());
    std::string source(raw.begin(), raw.end());

    assets.add_asset(entt::hashed_string(asset_name.data()).value(),
                     new ShaderSourceAsset(name, source));
  }
};

} // namespace engine::assets
