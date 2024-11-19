#ifndef CUBOID_HH
#define CUBOID_HH

#include "AbstractMobileObj.hh"

using namespace std;

class Cuboid: public AbstractMobileObj {
	private:
		double Ang_Roll_deg;
		double Ang_Pitch_deg;
		double Ang_Yaw_deg;
		Vector3D Position_m;
		string Name;

	public:
		Cuboid();
		virtual double GetAng_Roll_deg() const override;
		virtual double GetAng_Pitch_deg() const override;
		virtual double GetAng_Yaw_deg() const override;
		virtual void SetAng_Roll_deg(double Ang_Roll_deg) override;
		virtual void SetAng_Pitch_deg(double Ang_Pitch_deg) override;
		virtual void SetAng_Yaw_deg(double Ang_Yaw_deg) override;
		virtual const Vector3D & GetPositoin_m() const override;
		virtual void SetPosition_m(const Vector3D &rPos) override;
		virtual void SetName(const char* sName) override;
		virtual const string & GetName() const override;

};

#endif


