#include <iostream>
#include "Interp4Rotate.hh"


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Rotate"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Rotate::CreateCmd();
}


/*!
 *
 */
Interp4Rotate::Interp4Rotate(): _NazwaOb("Ob_B"), _NazwaOsi("OY"), _SpeedR_degS(0), _Angle_deg(0)
{}


/*!
 *
 */
void Interp4Rotate::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _NazwaOb << " " << _NazwaOsi << " " << _SpeedR_degS << " " << _Angle_deg << endl;
}


/*!
 *
 */
const char* Interp4Rotate::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Rotate::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  return true;
}


/*!
 *
 */
bool Interp4Rotate::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if (!(Strm_CmdsList >> _NazwaOb)) {
      cerr << "Blad: nie udalo sie przeczytac _NazwaOb" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _NazwaOsi)) {
      cerr << "Blad: nie udalo sie przeczytac _NazwaOsi" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _SpeedR_degS)) {
      cerr << "Blad: nie udalo sie przeczytac _SpeedR_degS" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _Angle_deg)) {
      cerr << "Blad: nie udalo sie przeczytac _Angle_deg" << endl;
      return false;
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Rotate::CreateCmd()
{
  return new Interp4Rotate();
}


/*!
 *
 */
void Interp4Rotate::PrintSyntax() const
{
  cout << "   Rotate  NazwaObiektu  Os  Szybkosc_katowa[deg/s]  Kat[deg]" << endl;
}
