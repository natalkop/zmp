#include "Cuboid.hh"

Cuboid::Cuboid(): Ang_Roll_deg(0), Ang_Pitch_deg(0), Ang_Yaw_deg(0)
{}

double Cuboid::GetAng_Roll_deg() const
{
  return Ang_Roll_deg;
}

double Cuboid::GetAng_Pitch_deg() const
{
  return Ang_Pitch_deg;
}

double Cuboid::GetAng_Yaw_deg() const
{
  return Ang_Yaw_deg;
}

void Cuboid::SetAng_Roll_deg(double Ang_Roll_deg_p)
{
  Ang_Roll_deg = Ang_Roll_deg_p;
}

void Cuboid::SetAng_Pitch_deg(double Ang_Pitch_deg_p)
{
  Ang_Pitch_deg = Ang_Pitch_deg_p;
}

void Cuboid::SetAng_Yaw_deg(double Ang_Yaw_deg_p)
{
  Ang_Yaw_deg = Ang_Yaw_deg_p;
}

const Vector3D & Cuboid::GetPositoin_m() const
{
  return Position_m;
}

void Cuboid::SetPosition_m(const Vector3D &rPos)
{
  Position_m = rPos;
}

void Cuboid::SetName(const char* sName)
{
  Name = sName;
}

const std::string & Cuboid::GetName() const
{
  return Name;
}
