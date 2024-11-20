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
#include "Set4LibInterfaces.hh"

#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include "AccessControl.hh"
#include "Port.hh"


#define LINE_SIZE 500

using namespace std;
using namespace xercesc;

/*!
 * \brief Wysyła napis do poprzez gniazdo sieciowe.
 *
 * Wysyła napis do poprzez gniazdo sieciowe.
 * \param[in] Sk2Server - deskryptor gniazda sieciowego, poprzez które
 *                        ma zostać wysłany napis w sensie języka C.
 * \param[in] sMesg - zawiera napis, który ma zostać wysłany poprzez
 *                    gniazdo sieciowe.
 */
int Send(int Sk2Server, const char *sMesg)
{
  ssize_t  IlWyslanych;
  ssize_t  IlDoWyslania = (ssize_t) strlen(sMesg);

  while ((IlWyslanych = write(Sk2Server,sMesg,IlDoWyslania)) > 0) {
    IlDoWyslania -= IlWyslanych;
    sMesg += IlWyslanych;
  }
  if (IlWyslanych < 0) {
    cerr << "*** Blad przeslania napisu." << endl;
  }
  return 0;
}

/*!
 * \brief Ilość stanów każdego z obiektów.
 *
 * Ilość stanów każdego z obiektów.
 * Podział na kolejne stany obiektów wykorzystywany jest
 * do animacji ruchu obiektów.
 */
#define STATES_NUMBER   2

/*!
 * Zestaw poleceń odpowiadających kolejnym stanom obiektu nr 1
 * i obrazujących jego ruch.
 */
const char* Cmds4Obj1[STATES_NUMBER] = {
  /*"UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,23)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,26)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,29)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,32)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,35)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,38)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,41)\n",
  "UpdateObj Name=Podstawa1 RotXYZ_deg=(0,-45,44)\n"*/
  "UpdateObj Name=Podstawa1 RGB=(255,0,0)\n",
  "UpdateObj Name=Podstawa1 RGB=(0,255,0)\n"
 };


/*!
 * Zestaw poleceń odpowiadających kolejnym stanom obiektu nr 2
 * i obrazujących jego ruch.
 */
/*const char* Cmds4Obj2[STATES_NUMBER] = {
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-48,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-45,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-42,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-39,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-36,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-33,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-30,0)\n",
  "UpdateObj Name=Podstawa1.Ramie1 RotXYZ_deg=(0,-27,0)\n"
 };*/

/*!
 * Zestaw poleceń odpowiadających kolejnym stanom obiektu nr 3
 * i obrazujących jego ruch.
 */
/*const char* Cmds4Obj3[STATES_NUMBER] = {
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-48,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-51,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-54,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-57,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-60,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-63,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-66,0)\n",
  "UpdateObj Name=Podstawa2.Ramie1 RotXYZ_deg=(0,-69,0)\n"
 };*/

/*!
 * \brief Ta klasa trochę udaję klasę MobileObj, która jest w zadaniu.
 *
 */
class GeomObject {
  /*!
   * \brief Identyfikuje aktualny stan obiektu.
   *
   * Identyfikuje aktualny stan obiektu. Indeks ten pozwala
   * na wybór z tablicy \link GeomObject::_Cmd4StatDesc _Cmd4StatDesc\endlink
   * odpowiedniego polecenia dla serwera, które reprezentuje aktualne
   * położenie i orientację obiektu.
   */
   int _StateIdx = 0;
  /*!
   * \brief Zestaw sekwencji poleceń symulujących ruch obiektu.
   *
   * Zestaw sekwencji poleceń symulujących ruch obiektu.
   * Każde polecenie odpowiada kolejnym stanom obiektu.
   */
   const char** _Cmd4StatDesc = nullptr;

 public:

  /*!
   * \brief Ustawia zestaw poleceń odpowiadających kolejnym stanom
   *        obiektu.
   */
  void SetCmds(const char *CmdsTab[STATES_NUMBER]) { _Cmd4StatDesc = CmdsTab; }
  /*!
   * \brief Udostępnia kolejny zestaw poleceń umożliwiających
   *        zespołu obiektu.
   *
   * Udostępnia kolejny zestaw poleceń umożliwiających
   * zespołu obiektu. Ta metoda "udaje" metodę, która w oryginalnym
   * rozwiązaniu powinna wygenerować odpowiednie polecenie na podstawie
   * przechowywanej informacji o położeniu i orientacji obiektu.
   */
  const char* GetStateDesc() const
  {
    return _Cmd4StatDesc[_StateIdx];
  }
/*!
   * \brief Zwiększa indeks stanu, o ile aktualny opis nie jest pusty.
   *
   *  Zwiększa indeks stanu, o ile aktualny opis nie jest pusty.
   *  Ta metoda "udaje" metodę, która w oryginalnym rozwiązaniu
   *  jest odpowiedzialna za zmianę stanu obiektu, tzn. zmianę jego
   *  położenia lub orientacji.
   */
  bool IncStateIndex() {
    if (_StateIdx >= STATES_NUMBER-1) return false;
    ++_StateIdx;
    return true;
  }
};

/*!
 * \brief Namiastka sceny z prostą kolekcją trzech wizualizowanych obiektów.
 */
class Scene: public AccessControl {
  public:
    Scene(): _Container4Objects(1)
    {
      _Container4Objects[0].SetCmds(Cmds4Obj1);
      //_Container4Objects[1].SetCmds(Cmds4Obj2);
      //_Container4Objects[2].SetCmds(Cmds4Obj3);
    }

  /*!
   * \brief Prosta kolekcja obiektów sceny
   */
   std::vector<GeomObject>   _Container4Objects;
};

/*!
 * \brief Modeluje nadajnik poleceń odzwierciedlających aktualny stan sceny.
 *
 * Modeluje nadajnik poleceń odzwierciedlających aktualny stan sceny.
 * W tym celu monitoruje zmiany na scenie, a gdy je wykryje przesyła
 * informacje o aktualnej pozycji wszystkich obiektów.
 */
class Sender {
  /*!
   * \brief Wartość tego pola decyduje o kontynuacji wykonywania wątku.
   *
   * Wartość tego pola decyduje o kontynuacji wykonywania wątku.
   * W przypadku wartości \p true, pętla wątku będzie wykonywana.
   * Natomiast wartość \p false powoduje przerwanie pętli.
   */
   volatile bool    _ContinueLooping = true;
  /*!
   * \brief Deskryptor gniazda sieciowego, poprzez które wysyłane są polecenia.
   */
   int             _Socket = 0;
  /*!
   * \brief Wskaźnik na scenę, które stan jest przesyłany w postaci
   *        poleceń do serwera graficzneg.
   */
   Scene          *_pScn = nullptr;


 public:
  /*!
   * \brief Inicjalizuje obiekt deskryptorem gniazda i wskaźnikiem
   *        na scenę, na zmianę stanu które będzie ten obiekt reagował.
   */
   Sender(int Socket, Scene *pScn): _Socket(Socket), _pScn(pScn) {}

  /*!
   * \brief Sprawdza, czy pętla wątku może być wykonywana.
   *
   * Sprawdza, czy pętla wątku może być wykonywana.
   * \retval true - pętla wątku może być nadal wykonywana.
   * \retval false - w przypadku przeciwnym.
   */
   bool ShouldCountinueLooping() const { return _ContinueLooping; }
  /*!
   * \brief Powoduje przerwanie działania pętli wątku.
   *
   * Powoduje przerwanie działania pętli wątku.
   * \warning Reakcja na tę operację nie będize natychmiastowa.
   */
   void CancelCountinueLooping() { _ContinueLooping = false; }

  /*!
   * \brief Ta metoda jest de facto treścią wątku komunikacyjnego
   *
   * Przegląda scenę i tworzy odpowiednie polecenia, a następnie
   * wysyła je do serwera.
   * \param[in] Socket - deskryptor gniazda sieciowego, poprzez które
   *                     wysyłane są polecenia.
   */
   void Watching_and_Sending() {
     while (ShouldCountinueLooping()) {
       if (!_pScn->IsChanged())  { usleep(10000); continue; }
       _pScn->LockAccess();

       //------- Przeglądanie tej kolekcji to uproszczony przykład

       for (const GeomObject &rObj : _pScn->_Container4Objects) {
                                     // Ta instrukcja to tylko uproszczony przykład
         cout << rObj.GetStateDesc();
         Send(_Socket,rObj.GetStateDesc()); // Tu musi zostać wywołanie odpowiedniej
                                           // metody/funkcji gerującej polecenia dla serwera.
       }

       _pScn->CancelChange();
       _pScn->UnlockAccess();
     }
   }

};

/*!
 * \brief Funkcja jest treścią wątku komunikacyjnego
 *
 * Funkcja jest treścią wątku komunikacyjnego.
 * \param[in] rSender - odpowiada za śledenie zmian na scenie
 *                      i przesyłanie do serwera graficznego
 *                      aktualnego stanu sceny, gdy uległ on zmianie.
 */
void Fun_CommunicationThread(Sender  *pSender)
{
  pSender->Watching_and_Sending();
}

/*!
 * Otwiera połączenie sieciowe
 * \param[out]  rSocket - deskryptor gniazda, poprzez które może być
 *                        realizowana komunikacja sieciowa.
 */
bool OpenConnection(int &rSocket)
{
  struct sockaddr_in  DaneAdSerw;

  bzero((char *)&DaneAdSerw,sizeof(DaneAdSerw));

  DaneAdSerw.sin_family = AF_INET;
  DaneAdSerw.sin_addr.s_addr = inet_addr("127.0.0.1");
  DaneAdSerw.sin_port = htons(PORT);


  rSocket = socket(AF_INET,SOCK_STREAM,0);

  if (rSocket < 0) {
     cerr << "*** Blad otwarcia gniazda." << endl;
     return false;
  }

  if (connect(rSocket,(struct sockaddr*)&DaneAdSerw,sizeof(DaneAdSerw)) < 0)
   {
     cerr << "*** Brak mozliwosci polaczenia do portu: " << PORT << endl;
     return false;
   }
  return true;
}

/*!
 * \brief Przykład wykonywania pojedynczej operacji z animacją.
 *
 * Przykład wykonywania pojedynczej operacji z animacją.
 * \param[in] pObj - wskaźnik na obiekt, dla którego ma być wykonana dana operacja
 *                   zmiany jego stanu.
 * \param[in] pAccCtrl - wskaźnik na obiekt, poprzez który przekazywana jest informacja
 *                   o zmianie na scenie, zaś w trakcie dokonywania zmianay
 *                   zamykany jest dostęp do całej sceny.
 * \retval true - Jeśli dokonan zosała zmiana stanu wszystkich obiektów.
 * \retval false - w przypadku przeciwnym.
 */
bool ChangeState(Scene &Scn) //GeomObject *pObj, AccessControl  *pAccCtrl)
{
  bool Changed;

  while (true) {
    Scn.LockAccess(); // Zamykamy dostęp do sceny, gdy wykonujemy
                            // modyfikacje na obiekcie.
    for (GeomObject &rObj : Scn._Container4Objects) {
       if (!(Changed = rObj.IncStateIndex())) { Scn.UnlockAccess();  return false; }
    }
    Scn.MarkChange();
    Scn.UnlockAccess();
    usleep(300000);
  }
  return true;
}


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


int main()
{

 cout << "Port: " << PORT << endl;
  Scene               Scn;
  int                 Socket4Sending;

  if (!OpenConnection(Socket4Sending)) return 1;

  Sender   ClientSender(Socket4Sending,&Scn);
  //  thread   Thread4Sending(Fun_Sender, Socket4Sending, &ClientSender);

  thread   Thread4Sending(Fun_CommunicationThread,&ClientSender);
  const char *sConfigCmds =
"Clear\n"
"AddObj Name=Podstawa1 RGB=(20,200,200) Scale=(4,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,20) Trans_m=(-1,3,0)\n"
/*"AddObj Name=Podstawa1.Ramie1 RGB=(200,0,0) Scale=(3,3,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(4,0,0)\n"
"AddObj Name=Podstawa1.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(3,0,0)\n"
"AddObj Name=Podstawa2 RGB=(20,200,200) Scale=(4,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(-1,-3,0)\n"
"AddObj Name=Podstawa2.Ramie1 RGB=(200,0,0) Scale=(3,3,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(4,0,0)\n"
"AddObj Name=Podstawa2.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(3,0,0)\n"*/;


  cout << "Konfiguracja:" << endl;
  cout << sConfigCmds << endl;

  Send(Socket4Sending,sConfigCmds);
  
  cout << "Akcja:" << endl;
  for (GeomObject &rObj : Scn._Container4Objects) {
    usleep(20000);
    ChangeState(Scn);
    Scn.MarkChange();
    usleep(100000);
  }
  usleep(100000);

  //-------------------------------------
  // Należy pamiętać o zamknięciu połączenia.
  // Bez tego serwer nie będzie reagował na
  // nowe połączenia.
  //
  cout << "Close\n" << endl; // To tylko, aby pokazac wysylana instrukcje
  Send(Socket4Sending,"Close\n");
  ClientSender.CancelCountinueLooping();
  Thread4Sending.join();
  close(Socket4Sending);

 /*std::map<std::string, std::shared_ptr<LibInterface>> libMap;

 libMap["Move"] = std::make_shared<LibInterface>("libInterp4Move.so", "Move");
 libMap["Rotate"] = std::make_shared<LibInterface>("libInterp4Rotate.so", "Rotate");
 libMap["Set"] = std::make_shared<LibInterface>("libInterp4Set.so", "Set");
 libMap["Pause"] = std::make_shared<LibInterface>("libInterp4Pause.so", "Pause");*/
 
 AbstractInterp4Command *command;
 Set4LibInterfaces libMap;
 LibInterface lib;
 shared_ptr<LibInterface> pointer;
 
 lib.InitLib("libInterp4Move.so", "Move");
 pointer = make_shared<LibInterface>(lib);
 libMap.setMap("Move", pointer);
 lib.InitLib("libInterp4Rotate.so", "Rotate");
 pointer = make_shared<LibInterface>(lib);
 libMap.setMap("Rotate", pointer);
 lib.InitLib("libInterp4Set.so", "Set");
 pointer = make_shared<LibInterface>(lib);
 libMap.setMap("Set", pointer);
 lib.InitLib("libInterp4Pause.so", "Pause");
 pointer = make_shared<LibInterface>(lib);
 libMap.setMap("Pause", pointer);

 Configuration   Config;
 if (!ReadFile("config/config.xml",Config)) return 1;

 const char* filename = "komendy_sterowania.cmd";
 istringstream IStrm4Cmds;

 if (ExecPreprocesor(filename, IStrm4Cmds)) {
     string word;
     while (IStrm4Cmds >> word) {
            if (!libMap.getMap(word, command)) {;
                command->ReadParams(IStrm4Cmds);
		cout << endl;
                cout << command->GetCmdName() << endl;
                cout << endl;
                command->PrintSyntax();
                cout << endl;
                command->PrintCmd();
                cout << endl;
            } else {
                cout << "Nieznane polecenie: " << word << endl;
            }
                /*if (word == "Move") {
		    auto it = libMap.find("Move");
                    if (it != libMap.end()) {
                        // Создаём команду
                        shared_ptr<AbstractInterp4Command> command = it->second->CreateCommand();
			command->ReadParams(IStrm4Cmds);
			cout << endl;
                        cout << command->GetCmdName() << endl;
                        cout << endl;
                        command->PrintSyntax();
                        cout << endl;
                        command->PrintCmd();
                        cout << endl;
                    } else {
                        cerr << "Polecenie nie znalezione" << endl;
                    }
                } else if (word == "Rotate") {
		    auto it = libMap.find("Rotate");
                    if (it != libMap.end()) {
                       // Создаём команду
                       shared_ptr<AbstractInterp4Command> command = it->second->CreateCommand();
		       command->ReadParams(IStrm4Cmds);
		       cout << endl;
                       cout << command->GetCmdName() << endl;
                       cout << endl;
                       command->PrintSyntax();
                       cout << endl;
                       command->PrintCmd();
                       cout << endl;
                    } else {
                       cerr << "Polecenie nie znalezione" << endl;
                    }
                } else if (word == "Pause") {
		    auto it = libMap.find("Pause");
                    if (it != libMap.end()) {
                       // Создаём команду
                       shared_ptr<AbstractInterp4Command> command = it->second->CreateCommand();
		       command->ReadParams(IStrm4Cmds);
		       cout << endl;
		       cout << command->GetCmdName() << endl;
		       cout << endl;
                       command->PrintSyntax();
		       cout << endl;
                       command->PrintCmd();
		       cout << endl;
                    } else {
                       cerr << "Polecenie nie znalezione" << endl;
                    }
                } else if (word == "Set") {
		    auto it = libMap.find("Set");
                    if (it != libMap.end()) {
                       // Создаём команду
                       shared_ptr<AbstractInterp4Command> command = it->second->CreateCommand();
                       command->ReadParams(IStrm4Cmds);
		       cout << endl;
		       cout << command->GetCmdName() << endl;
		       cout << endl;
                       command->PrintSyntax();
		       cout << endl;
                       command->PrintCmd();
		       cout << endl;
                    } else {
                       cerr << "Polecenie nie znalezione" << endl;
                    }
		} else {
                    cout << "Nieznane polecenie: " << word << endl;
                }*/
     }
 } else {
     cerr << "Blad preprocesora" << endl;
 }

 return 0;
 
}
