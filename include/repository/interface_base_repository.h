#pragma once

#include <optional>
#include <vector>

namespace lynx::repository
{

template <typename Entity, typename IdType = int>
class IBaseRepository
{

    IBaseRepository();

    virtual std::vector<Entity> find_all() const = 0;
    virtual std::optional<Entity> find_by_id(IdType id) const = 0;
    virtual Entity create(const Entity &entity) = 0;
    virtual void remove(IdType id) = 0;
};

} // namespace lynx::repository
