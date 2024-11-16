#include <iostream>
#include "Interp4Set.hh"


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Set::CreateCmd();
}


/*!
 *
 */
Interp4Set::Interp4Set(): _NazwaOb("Ob_A"), _wspX(0), _wspY(0), _wspZ(0), _AngleX_deg(0), _AngleY_deg(0), _AngleZ_deg(0)
{}


/*!
 *
 */
void Interp4Set::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _NazwaOb << " " << _wspX << " " << _wspY << " " << _wspZ << " " << _AngleX_deg << " " << _AngleY_deg << " " << _AngleZ_deg << endl;
}


/*!
 *
 */
const char* Interp4Set::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Set::ExecCmd( AbstractScene      &rScn, 
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
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if (!(Strm_CmdsList >> _NazwaOb)) {
      cerr << "Blad: nie udalo sie przeczytac _NazwaOb" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _wspX)) {
      cerr << "Blad: nie udalo sie przeczytac _wspX" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _wspY)) {
      cerr << "Blad: nie udalo sie przeczytac _wspY" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _wspZ)) {
      cerr << "Blad: nie udalo sie przeczytac _wspZ" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _AngleX_deg)) {
      cerr << "Blad: nie udalo sie przeczytac _AngleX_deg" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _AngleY_deg)) {
      cerr << "Blad: nie udalo sie przeczytac _AngleY_deg" << endl;
      return false;
  }
  if (!(Strm_CmdsList >> _AngleZ_deg)) {
      cerr << "Blad: nie udalo sie przeczytac _AngleZ_deg" << endl;
      return false;
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Set::CreateCmd()
{
  return new Interp4Set();
}


/*!
 *
 */
void Interp4Set::PrintSyntax() const
{
  cout << "   Set  NazwaObiektu  wspX wspY wspZ Kat_OX Kat_OY Kat_OZ" << endl;
}
