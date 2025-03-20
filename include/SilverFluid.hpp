#ifndef SILVER_FLUID_HPP
#define SILVER_FLUID_HPP
#include <variant>
#include <vector>

void SetAsFluidBlocker(const std::variant<int, std::vector<int>> &IDs);

#endif
