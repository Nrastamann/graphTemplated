#pragma once

#include <bitset>
#include <cassert>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <span>
#include <utility>
#include <vector>
#include "entity.hpp"

namespace components {
  static size_t counter = 0;

  constexpr size_t kResizableTag{std::numeric_limits<size_t>::max()};

  template <typename T, size_t N>
  struct ComponentTraits {
   private:
    static constexpr size_t kRounding{63};
    static_assert((N + kRounding) / entity::kChunkSize > 0,
                  "Need to pick greater number of elements");
    using ComponentType = T;
    using FlagType      = std::bitset<entity::kChunkSize>;
    using DataType      = std::array<ComponentType, entity::kChunkSize>;

   public:
    struct ComponentsRange {
      std::span<DataType> _components;
      std::span<FlagType> _flags;
    };

    static constexpr bool kIsResizable = false;

    using data_container =
        std::array<DataType, (N + kRounding) / entity::kChunkSize>;
    using flags_container =
        std::array<FlagType, (N + kRounding) / entity::kChunkSize>;
  };

  template <typename T>
  struct ComponentTraits<T, kResizableTag> {
   private:
    using ComponentType = T;
    using FlagType      = std::bitset<entity::kChunkSize>;
    using DataType      = std::array<ComponentType, entity::kChunkSize>;

   public:
    struct ComponentsRange {
      std::span<DataType> _components;
      std::span<FlagType> _flags;
    };

    static constexpr bool kIsResizable = true;

    using data_container               = std::vector<DataType>;
    using flags_container              = std::vector<FlagType>;
  };

  struct BaseComponentHandler {
    template <typename U>
    void
    setComponent(this auto& self, const entity::EntityIdInner& index,
                 U&& component)
    {
      return self.setComponentImpl(index, std::forward<U>(component));
    }

    void
    removeComponent(this auto& self, const entity::EntityIdInner& index)
    {
      self.removeComponentImpl(index);
    }

    auto
    getComponents(this auto& self)
    {
      return self.getComponentsImpl();
    }

    virtual ~BaseComponentHandler() = default;
  };

  template <typename T, size_t N>
  class ComponentHandler : public BaseComponentHandler {
    using Traits = ComponentTraits<T, N>;

   public:
    template <std::same_as<typename Traits::ComponentType> U>
    void
    setComponentImpl(const entity::EntityIdInner& index, U&& component)
    {
      if (index._clusterId >= _pool.size()) {
        if constexpr (!Traits::kIsResizable) {
          std::unreachable();  // временно
        }

        _pool.resize(index._clusterId + 1);
        _bitmap.resize(index._clusterId + 1);
      }

      _pool[index._clusterId][index._idx]   = std::forward<U>(component);
      _bitmap[index._clusterId][index._idx] = true;
    }

    void
    removeComponentImpl(const entity::EntityIdInner& index)
    {
      _bitmap[index._clusterId][index._idx] = false;
    }

    Traits::ComponentsRange
    getComponentsImpl()
    {
      return {std::span(_pool.data(), _pool.size()),
              std::span(_bitmap.data(), _bitmap.size())};
    }

   private:
    Traits::data_container _pool;
    Traits::flags_container _bitmap;
  };
}  // namespace components
