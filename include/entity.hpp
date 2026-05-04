#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>

namespace entity {
  static constexpr size_t kChunkSize{64};
  enum class EntityStatus : uint8_t {
    Active,
    Disabled
  };

  struct EntityIdInner {
    uint64_t _verisionedIdx;
    size_t _clusterId;
    size_t _idx;
  };

  struct EnitityId {
    EntityStatus _status;
    std::variant<EntityIdInner, std::array<char, sizeof(EntityIdInner)>> _data;
  };
}  // namespace entity
