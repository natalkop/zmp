#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include "AbstractInterp4Command.hh"

using namespace std;

class LibInterface {
public:
    // Конструктор загружает библиотеку и извлекает указатель на функцию CreateCmd
    LibInterface(const std::string &libPath, const std::string &CmdName)
        : _CmdName(CmdName), _LibHandler(nullptr), _pCreateCmd(nullptr) {
        _LibHandler = dlopen(libPath.c_str(), RTLD_LAZY);
        if (!_LibHandler) {
	    cerr << "!!! Brak biblioteki: " << libPath << endl;
        }

        // Извлекаем указатель на функцию CreateCmd
        void *pFun = dlsym(_LibHandler, "CreateCmd");
        if (!pFun) {
	    cerr << "!!! Nie znaleziono funkcji CreateCmd w bibliotece " << libPath << endl;
	    dlclose(_LibHandler);
        }

        _pCreateCmd = reinterpret_cast<AbstractInterp4Command *(*)(void)>(pFun);
    }

    // Деструктор освобождает библиотеку
    ~LibInterface() {
        if (_LibHandler) {
            dlclose(_LibHandler);
        }
    }

    // Метод для создания команды
    std::shared_ptr<AbstractInterp4Command> CreateCommand() const {
        return std::shared_ptr<AbstractInterp4Command>(_pCreateCmd());
    }

    // Получение имени команды
    /*const std::string &GetCmdName() const {
        return cmdName;
    }*/

private:
    std::string _CmdName;
    void *_LibHandler;
    AbstractInterp4Command *(*_pCreateCmd)();
};

