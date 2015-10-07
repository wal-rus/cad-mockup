#pragma once

struct Vector2;

//Basic description of the parameters for a CNC machine
//Defined as a separate structure to allow serialization.
struct MachineInfo  {
  const double padding; //In inches
  const double max_speed; //In inches per second
  const double cost_per_s; //In dollars per second
  const double cost_per_sq_in; //In dollars per square inch.
};

double ComputeCost(const MachineInfo& tooling, const Vector2& bounds, double cutTime);