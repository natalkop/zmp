#ifndef SET4LIBINTERFACES_HH
#define SET4LIBINTERFACES_HH

#include <map>
#include <memory>
#include "AbstractInterp4Command.hh"
#include "LibInterface.hh"

using namespace std;

class Set4LibInterfaces {
  private:
    map<string, shared_ptr<LibInterface>> _LibManager;
  public:
    void setMap(string CmdName, shared_ptr<LibInterface> Lib) {
      _LibManager[CmdName] = Lib;
    };
    
    int getMap(string CmdName, AbstractInterp4Command *&pCmd) {
      AbstractInterp4Command *(*pCreateCmd)(void);
      auto it = _LibManager.find(CmdName);
      if (it != _LibManager.end()) {
        pCreateCmd = it->second->CreateCommand();
        pCmd = pCreateCmd();
        return 0;
      } else return 1;
    };

};

#endif
