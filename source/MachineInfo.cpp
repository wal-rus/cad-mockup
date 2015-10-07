#include "MachineInfo.h"
#include "Vector2.h"
double ComputeCost(const MachineInfo& tooling, const Vector2& bounds, double cutTime) {
  
  const auto area = (bounds.x + tooling.padding) * (bounds.y + tooling.padding);
  
  return (area * tooling.cost_per_sq_in) + (cutTime * tooling.cost_per_s);
}