#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include "AbstractInterp4Command.hh"

using namespace std;

class LibInterface {
public:
    LibInterface() {};
    LibInterface(const std::string &libPath, const std::string &CmdName)
        : _CmdName(CmdName), _LibHandler(nullptr), _pCreateCmd(nullptr) {}
    
    ~LibInterface() {
        if (_LibHandler) {
            dlclose(_LibHandler);
        }
    }

    // Метод для создания команды
    /*std::shared_ptr<AbstractInterp4Command> CreateCommand() const {
        return std::shared_ptr<AbstractInterp4Command>(_pCreateCmd());
    }*/
    AbstractInterp4Command *(*CreateCommand()) (void) {
      return _pCreateCmd;
    };
    
    int InitLib(const string &libPath, const string &CmdName) {
      _LibHandler = dlopen(libPath.c_str(), RTLD_LAZY);
        if (!_LibHandler) {
	    cerr << "!!! Brak biblioteki: " << libPath << endl;
	    return 1;
        }

        void *pFun = dlsym(_LibHandler, "CreateCmd");
        if (!pFun) {
	    cerr << "!!! Nie znaleziono funkcji CreateCmd w bibliotece " << libPath << endl;
	    return 1;
        }

        _pCreateCmd = reinterpret_cast<AbstractInterp4Command *(*)(void)>(pFun);
        
        return 0;
    };

private:
    std::string _CmdName;
    void *_LibHandler;
    AbstractInterp4Command *(*_pCreateCmd)();
};

