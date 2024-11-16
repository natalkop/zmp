#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include <string>
#include <cstdio>
#include <memory>
#include "AbstractInterp4Command.hh"
#include <sstream>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xmlinterp.hh"
#include <list>
#include <vector>

#define LINE_SIZE 500

using namespace std;
using namespace xercesc;

/*!
 * Czyta z pliku opis poleceń i dodaje je do listy komend,
 * które robot musi wykonać.
 * \param sFileName - (\b we.) nazwa pliku z opisem poleceń.
 * \param CmdList - (\b we.) zarządca listy poleceń dla robota.
 * \retval true - jeśli wczytanie zostało zrealizowane poprawnie,
 * \retval false - w przeciwnym przypadku.
 */
bool ReadFile(const char* sFileName, Configuration &rConfig)
{
   try {
            XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            cerr << "Error during initialization! :\n";
            cerr << "Exception message is: \n"
                 << message << "\n";
            XMLString::release(&message);
            return 1;
   }

   SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();

   pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
   pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
   pParser->setFeature(XMLUni::fgXercesDynamic, false);
   pParser->setFeature(XMLUni::fgXercesSchema, true);
   pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

   pParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

   DefaultHandler* pHandler = new XMLInterp4Config(rConfig);
   pParser->setContentHandler(pHandler);
   pParser->setErrorHandler(pHandler);

   try {
     
     if (!pParser->loadGrammar("config/config.xsd",
                              xercesc::Grammar::SchemaGrammarType,true)) {
       cerr << "!!! Plik grammar/actions.xsd, '" << endl
            << "!!! ktory zawiera opis gramatyki, nie moze zostac wczytany."
            << endl;
       return false;
     }
     pParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
     pParser->parse(sFileName);
   }
   catch (const XMLException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            cerr << "Informacja o wyjatku: \n"
                 << "   " << sMessage << "\n";
            XMLString::release(&sMessage);
            return false;
   }
   catch (const SAXParseException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            char* sSystemId = xercesc::XMLString::transcode(Exception.getSystemId());

            cerr << "Blad! " << endl
                 << "    Plik:  " << sSystemId << endl
                 << "   Linia: " << Exception.getLineNumber() << endl
                 << " Kolumna: " << Exception.getColumnNumber() << endl
                 << " Informacja: " << sMessage 
                 << endl;

            XMLString::release(&sMessage);
            XMLString::release(&sSystemId);
            return false;
   }
   catch (...) {
            cout << "Zgloszony zostal nieoczekiwany wyjatek!\n" ;
            return false;
   }

   delete pParser;
   delete pHandler;
   return true;
}

bool ExecPreprocesor( const char *NazwaPliku, istringstream &IStrm4Cmds )
{
	string Cmd4Preproc = "cpp -P ";
	char Line[LINE_SIZE];
	ostringstream OTmpStrm;
	Cmd4Preproc += NazwaPliku;
	FILE* pProc = popen(Cmd4Preproc.c_str(),"r");
	if (!pProc) return false;
	while (fgets(Line,LINE_SIZE,pProc)) {
		OTmpStrm << Line;
	}
	IStrm4Cmds.str(OTmpStrm.str());
 	//cout << IStrm4Cmds.str() << endl;  // Test
	return pclose(pProc) == 0;
}

void *pLibHnd_Move = dlopen("libInterp4Move.so", RTLD_LAZY);
void *pLibHnd_Rotate = dlopen("libInterp4Rotate.so", RTLD_LAZY);
void *pLibHnd_Pause = dlopen("libInterp4Pause.so", RTLD_LAZY);
void *pLibHnd_Set = dlopen("libInterp4Set.so",RTLD_LAZY);
vector<void*> tablica = {pLibHnd_Move, pLibHnd_Rotate, pLibHnd_Pause, pLibHnd_Set};

int main()
{
 /*{
  void *pLibHnd_Move = dlopen("libInterp4Move.so",RTLD_LAZY);
  AbstractInterp4Command *(*pCreateCmd_Move)(void);
  void *pFun;

  if (!pLibHnd_Move) {
    cerr << "!!! Brak biblioteki: Interp4Move.so" << endl;
    return 1;
  }


  pFun = dlsym(pLibHnd_Move,"CreateCmd");
  if (!pFun) {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }
  pCreateCmd_Move = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


  AbstractInterp4Command *pCmd = pCreateCmd_Move();

  cout << endl;
  cout << pCmd->GetCmdName() << endl;
  cout << endl;
  pCmd->PrintSyntax();
  cout << endl;
  pCmd->PrintCmd();
  cout << endl;
  
  delete pCmd;

  dlclose(pLibHnd_Move);
}

{
  void *pLibHnd_Rotate = dlopen("libInterp4Rotate.so",RTLD_LAZY);
  AbstractInterp4Command *(*pCreateCmd_Rotate)(void);
  void *pFun;

  if (!pLibHnd_Rotate) {
    cerr << "!!! Brak biblioteki: Interp4Rotate.so" << endl;
    return 1;
  }


  pFun = dlsym(pLibHnd_Rotate,"CreateCmd");
  if (!pFun) {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }
  pCreateCmd_Rotate = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


  AbstractInterp4Command *pCmd = pCreateCmd_Rotate();

  cout << endl;
  cout << pCmd->GetCmdName() << endl;
  cout << endl;
  pCmd->PrintSyntax();
  cout << endl;
  pCmd->PrintCmd();
  cout << endl;

  delete pCmd;

  dlclose(pLibHnd_Rotate);
} 

{ 
  void *pLibHnd_Pause = dlopen("libInterp4Pause.so",RTLD_LAZY);
  AbstractInterp4Command *(*pCreateCmd_Pause)(void);
  void *pFun;

  if (!pLibHnd_Pause) {
    cerr << "!!! Brak biblioteki: Interp4Pause.so" << endl;
    return 1;
  }


  pFun = dlsym(pLibHnd_Pause,"CreateCmd");
  if (!pFun) {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }
  pCreateCmd_Pause = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


  AbstractInterp4Command *pCmd = pCreateCmd_Pause();

  cout << endl;
  cout << pCmd->GetCmdName() << endl;
  cout << endl;
  pCmd->PrintSyntax();
  cout << endl;
  pCmd->PrintCmd();
  cout << endl;

  delete pCmd;

  dlclose(pLibHnd_Pause);
}

{
  void *pLibHnd_Set = dlopen("libInterp4Set.so",RTLD_LAZY);
  AbstractInterp4Command *(*pCreateCmd_Set)(void);
  void *pFun;

  if (!pLibHnd_Set) {
    cerr << "!!! Brak biblioteki: Interp4Set.so" << endl;
    return 1;
  }


  pFun = dlsym(pLibHnd_Set,"CreateCmd");
  if (!pFun) {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }
  pCreateCmd_Set = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


  AbstractInterp4Command *pCmd = pCreateCmd_Set();

  cout << endl;
  cout << pCmd->GetCmdName() << endl;
  cout << endl;
  pCmd->PrintSyntax();
  cout << endl;
  pCmd->PrintCmd();
  cout << endl;

  delete pCmd;

  dlclose(pLibHnd_Set);
}*/

 Configuration   Config;
 if (!ReadFile("config/config.xml",Config)) return 1;

 const char* filename = "komendy_sterowania.cmd";
 istringstream IStrm4Cmds;

 if (ExecPreprocesor(filename, IStrm4Cmds)) {
     string word;
     while (IStrm4Cmds >> word) {
                if (word == "Move") {
		    //void *pLibHnd_Move = dlopen("libinterp4Move", RTLD_LAZY);
                    AbstractInterp4Command *(*pCreateCmd_Move)(void);
                    void *pFun;

                    if (!tablica[0]) {
                       cerr << "!!! Brak biblioteki: Interp4Move.so" << endl;
                       return 1;
                    }


                    pFun = dlsym(tablica[0],"CreateCmd");
                    if (!pFun) {
                       cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
                       return 1;
                    }
                    pCreateCmd_Move = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


                    AbstractInterp4Command *pCmd = pCreateCmd_Move();

		    pCmd->ReadParams(IStrm4Cmds);
                    cout << endl;
                    cout << pCmd->GetCmdName() << endl;
                    cout << endl;
                    pCmd->PrintSyntax();
                    cout << endl;
                    pCmd->PrintCmd();
                    cout << endl;
  
                    delete pCmd;

                    //dlclose(pLibHnd_Move);
                } else if (word == "Rotate") {
		    void *pLibHnd_Rotate = dlopen("libInterp4Rotate.so", RTLD_LAZY);
                    AbstractInterp4Command *(*pCreateCmd_Rotate)(void);
                    void *pFun;

                    if (!pLibHnd_Rotate) {
                       cerr << "!!! Brak biblioteki: Interp4Rotate.so" << endl;
                       return 1;
                    }


                    pFun = dlsym(pLibHnd_Rotate,"CreateCmd");
                    if (!pFun) {
                       cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
                       return 1;
                    }
                    pCreateCmd_Rotate = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


                    AbstractInterp4Command *pCmd = pCreateCmd_Rotate();

		    pCmd->ReadParams(IStrm4Cmds);
                    cout << endl;
                    cout << pCmd->GetCmdName() << endl;
                    cout << endl;
                    pCmd->PrintSyntax();
                    cout << endl;
                    pCmd->PrintCmd();
                    cout << endl;
  
                    delete pCmd;

                    dlclose(pLibHnd_Rotate);
                } else if (word == "Pause") {
	            void *pLibHnd_Pause = dlopen("libInterp4Pause.so", RTLD_LAZY);
                    AbstractInterp4Command *(*pCreateCmd_Pause)(void);
                    void *pFun;

                    if (!pLibHnd_Pause) {
                       cerr << "!!! Brak biblioteki: Interp4Pause.so" << endl;
                       return 1;
                    }


                    pFun = dlsym(pLibHnd_Pause,"CreateCmd");
                    if (!pFun) {
                       cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
                       return 1;
                    }
                    pCreateCmd_Pause = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


                    AbstractInterp4Command *pCmd = pCreateCmd_Pause();

		    pCmd->ReadParams(IStrm4Cmds);
                    cout << endl;
                    cout << pCmd->GetCmdName() << endl;
                    cout << endl;
                    pCmd->PrintSyntax();
                    cout << endl;
                    pCmd->PrintCmd();
                    cout << endl;
  
                    delete pCmd;

                    dlclose(pLibHnd_Pause);
                } else if (word == "Set") {
		    void *pLibHnd_Set = dlopen("libInterp4Set.so", RTLD_LAZY);
                    AbstractInterp4Command *(*pCreateCmd_Set)(void);
                    void *pFun;

                    if (!pLibHnd_Set) {
                       cerr << "!!! Brak biblioteki: Interp4Set.so" << endl;
                       return 1;
                    }


                    pFun = dlsym(pLibHnd_Set,"CreateCmd");
                    if (!pFun) {
                       cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
                       return 1;
                    }
                    pCreateCmd_Set = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);


                    AbstractInterp4Command *pCmd = pCreateCmd_Set();

		    pCmd->ReadParams(IStrm4Cmds);
                    cout << endl;
                    cout << pCmd->GetCmdName() << endl;
                    cout << endl;
                    pCmd->PrintSyntax();
                    cout << endl;
                    pCmd->PrintCmd();
                    cout << endl;
  
                    delete pCmd;

                    dlclose(pLibHnd_Set);
		} else {
                    cout << "Nieznane polecenie: " << word << endl;
                }
     }
 } else {
     cerr << "Blad preprocesora" << endl;
 }

 return 0; 
 
}
