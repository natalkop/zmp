#include <xercesc/util/PlatformUtils.hpp>
#include "xmlinterp.hh"
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include "Vector3D.hh"



using namespace std;

void printCube2(const Cube& cube) {
    std::cout << "Cube Name: " << cube.Name << std::endl;
    std::cout << "  Shift: [" << cube.Shift << "]" << std::endl;
    std::cout << "  Scale: [" << cube.Scale << "]" << std::endl;
    std::cout << "  Rotation (deg): [" << cube.RotXYZ_deg << "]" << std::endl;
    std::cout << "  Translation (m): [" << cube.Trans_m << "]" << std::endl;
    std::cout << "  RGB: [" << cube.RGB << "]" << std::endl;
}

/*!
 * Konstruktor klasy. Tutaj należy zainicjalizować wszystkie
 * dodatkowe pola.
 */
XMLInterp4Config::XMLInterp4Config(Configuration &rConfig): Config(rConfig)
{
}


/*!
 * Metoda wywoływana jest bezpośrednio przed rozpoczęciem
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::startDocument()
{
  cout << "*** Rozpoczecie przetwarzania dokumentu XML." << endl;
}


/*!
 * Metoda wywoływana jest bezpośrednio po zakończeniu
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::endDocument()
{
  cout << "=== Koniec przetwarzania dokumentu XML." << endl;
}





/*!
 * Analizuje atrybuty elementu XML \p "Lib" i odpowiednio je interpretuje.
 * \param[in] rAttrs - atrybuty elementu XML \p "Lib".
 */
void XMLInterp4Config::ProcessLibAttrs(const xercesc::Attributes  &rAttrs)
{
 if (rAttrs.getLength() != 1) {
      cerr << "Zla ilosc atrybutow dla \"Lib\"" << endl;
      exit(1);
 }

 char* sParamName = xercesc::XMLString::transcode(rAttrs.getQName(0));

 if (strcmp(sParamName,"Name")) {
      cerr << "Zla nazwa atrybutu dla Lib" << endl;
      exit(1);
 } 

 XMLSize_t  Size = 0;
 char* sLibName = xercesc::XMLString::transcode(rAttrs.getValue(Size));

 cout << "  Nazwa biblioteki: " << sLibName << endl;

 // Tu trzeba wpisać własny kod ...
 Config.setLib(sLibName);
 
 /*const vector<string>& libs = Config.getLibs();
    for (size_t i = 0; i < libs.size(); ++i) {
        std::cout << libs[i] << " ";
    }
    cout << endl;*/
 
 xercesc::XMLString::release(&sParamName);
 xercesc::XMLString::release(&sLibName);
}


/*!
 * Analizuje atrybuty. Sprawdza czy ich nazwy są poprawne. Jeśli tak,
 * to pobiera wartości atrybutów (w postaci napisów) i przekazuje ...
 * \param[in] rAttrs - atrybuty elementu XML \p "Cube".
 */
void XMLInterp4Config::ProcessCubeAttrs(const xercesc::Attributes  &rAttrs)
{
 if (rAttrs.getLength() < 1) {
      cerr << "Zla ilosc atrybutow dla \"Cube\"" << endl;
      exit(1);
 }

 /*
  *  Tutaj pobierane sa nazwy pierwszego i drugiego atrybuty.
  *  Sprawdzamy, czy na pewno jest to Name i Value.
  */

 char* sName_Name = xercesc::XMLString::transcode(rAttrs.getQName(0));
 char* sName_Shift = xercesc::XMLString::transcode(rAttrs.getQName(1));
 char* sName_Scale = xercesc::XMLString::transcode(rAttrs.getQName(2));
 char* sName_RotXYZ_deg = xercesc::XMLString::transcode(rAttrs.getQName(3));
 char* sName_Trans_m = xercesc::XMLString::transcode(rAttrs.getQName(4));
 char* sName_RGB = xercesc::XMLString::transcode(rAttrs.getQName(5));

 XMLSize_t  Index = 0;
 char* sValue_Name    = xercesc::XMLString::transcode(rAttrs.getValue(Index));
 char* sValue_Shift    = xercesc::XMLString::transcode(rAttrs.getValue(1));
 char* sValue_Scale = xercesc::XMLString::transcode(rAttrs.getValue(2));
 char* sValue_RotXYZ_deg = xercesc::XMLString::transcode(rAttrs.getValue(3));
 char* sValue_Trans_m = xercesc::XMLString::transcode(rAttrs.getValue(4));
 char* sValue_RGB     = xercesc::XMLString::transcode(rAttrs.getValue(5));


 //-----------------------------------------------------------------------------
 // Wyświetlenie nazw atrybutów i ich "wartości"
 //
 cout << " Atrybuty:" << endl
      << "     " << sName_Name << " = \"" << sValue_Name << "\"" << endl
      << "     " << sName_Shift << " = \"" << sValue_Shift << "\"" << endl
      << "     " << sName_Scale << " = \"" << sValue_Scale << "\"" << endl
      << "     " << sName_RotXYZ_deg << " = \"" << sValue_RotXYZ_deg << "\"" << endl
      << "     " << sName_Trans_m << " = \"" << sValue_Trans_m << "\"" << endl
      << "     " << sName_RGB << " = \"" << sValue_RGB << "\"" << endl   
      << endl; 
 //-----------------------------------------------------------------------------
 // Przykład czytania wartości parametrów
 // Ten przykład jest zrobiony "na piechotę" wykorzystując osobne zmienne.
 // Skala powinna być wektorem. Czytanie powinno być rezlizowane z wykorzysaniem
 // wektorów, np.
 //
 //
 // istringstream IStrm;
 // IStrm.str(sValue_Scale);
 // Vector3D  Scale;
 //
 // IStrm >> Scale;
 //
 istringstream   IStrm;
 Cube cube;
 
 cube.Name = sValue_Name;
 
 IStrm.str(sValue_Shift);
 Vector3D Shift;

 IStrm >> Shift;
 if (IStrm.fail()) {
     cerr << " Blad!!!" << endl;
 } else {
     cout << " Czytanie wartosci OK!!!" << endl;
     cout << Shift << endl;
     cube.Shift=Shift;
 }
 
 IStrm.clear();  // Resetuje stan strumienia (np. usuwa flagę błędu)
 IStrm.str("");  // Usuwa dotychczasową zawartość bufora
 
 IStrm.str(sValue_Scale);
 Vector3D Scale;

 IStrm >> Scale;
 if (IStrm.fail()) {
     cerr << " Blad!!!" << endl;
 } else {
     cout << " Czytanie wartosci OK!!!" << endl;
     cout << Scale << endl;
     cube.Scale=Scale;
 }
 
 IStrm.clear();
 IStrm.str("");
 
 IStrm.str(sValue_RotXYZ_deg);
 Vector3D RotXYZ_deg;

 IStrm >> RotXYZ_deg;
 if (IStrm.fail()) {
     cerr << " Blad!!!" << endl;
 } else {
     cout << " Czytanie wartosci OK!!!" << endl;
     cout << RotXYZ_deg << endl;
     cube.RotXYZ_deg=RotXYZ_deg;
 }
 
 IStrm.clear();
 IStrm.str("");
 
 IStrm.str(sValue_Trans_m);
 Vector3D Trans_m;

 IStrm >> Trans_m;
 if (IStrm.fail()) {
     cerr << " Blad!!!" << endl;
 } else {
     cout << " Czytanie wartosci OK!!!" << endl;
     cout << Trans_m << endl;
     cube.Trans_m=Trans_m;
 }
 
 IStrm.clear();
 IStrm.str("");
 
 IStrm.str(sValue_RGB);
 Vector3D RGB;

 IStrm >> RGB;
 if (IStrm.fail()) {
     cerr << " Blad!!!" << endl;
 } else {
     cout << " Czytanie wartosci OK!!!" << endl;
     cout << RGB << endl;
     cube.RGB=RGB;
 }
 
 IStrm.clear();
 IStrm.str("");

 // Tu trzeba wstawić odpowiednio własny kod ...
 Config.setCube(cube);
 
 std::cout << "Zawartość Cubes:" << std::endl;
    for (const auto& cube : Config.getCubes()) {
        printCube2(cube);
        std::cout << std::endl; // Oddzielanie kolejnych Cube
    }

 xercesc::XMLString::release(&sName_Name);
 xercesc::XMLString::release(&sName_Shift);
 xercesc::XMLString::release(&sName_Scale);
 xercesc::XMLString::release(&sName_RotXYZ_deg);
 xercesc::XMLString::release(&sName_Trans_m);
 xercesc::XMLString::release(&sName_RGB);
 xercesc::XMLString::release(&sValue_Name);
 xercesc::XMLString::release(&sValue_Shift);
 xercesc::XMLString::release(&sValue_Scale);
 xercesc::XMLString::release(&sValue_RotXYZ_deg);
 xercesc::XMLString::release(&sValue_Trans_m);
 xercesc::XMLString::release(&sValue_RGB);
}







/*!
 * Wykonuje operacje związane z wystąpieniem danego elementu XML.
 * W przypadku elementu \p "Action" będzie to utworzenie obiektu
 * reprezentującego odpowiednią działanie robota.
 * W przypadku elementu \p "Parameter" będzie to zapisanie parametrów
 * związanych z danym działaniem.
 * \param[in] rElemName - nazwa elementu XML.
 * \param[in] rAttrs - atrybuty napotkanego elementu XML.
 */
void XMLInterp4Config::WhenStartElement( const std::string           &rElemName,
		                         const xercesc::Attributes   &rAttrs
                                       )
{
  if (rElemName == "Lib") {
    ProcessLibAttrs(rAttrs);   return;   
  }

  if (rElemName == "Cube") {
    ProcessCubeAttrs(rAttrs);  return;
  }
}




/*!
 * Metoda jest wywoływana po napotkaniu nowego elementu XML, np.
 * gdy zostanie napotkany element:
   \verbatim
     <Action Name="Rotate">
   \endverbatim
 *  to poprzez parametr \e xscElemName jest dostęp do nazwy elementu
 *  tzn. \p Action, 
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Action".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *  \param[in] rAttrs -  lista atrybutów danego symbolu XML.
 *                 W przykładzie pokazanym powyżej listę atrybutów
 *                 będą stanowiły napisy:
 */
/*
 * Te metode nalezy odpowiednio zdekomponowac!!!
 */
void XMLInterp4Config::startElement(  const   XMLCh* const            pURI,
                                      const   XMLCh* const            pLocalName,
                                      const   XMLCh* const            pQName,
				      const   xercesc::Attributes&    rAttrs
                                    )
{
  char* sElemName = xercesc::XMLString::transcode(pLocalName);
  cout << "+++ Poczatek elementu: "<< sElemName << endl;

  WhenStartElement(sElemName, rAttrs);

  xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda zostaje wywołana w momencie zakończenia danego elementu
 * XML, np. jeżeli w pliku jest wpis:
   \verbatim
     <Lib Name="Rotate">
     </Lib>
   \endverbatim
 * to metoda ta zostanie wywołana po napotkaniu znacznika:
 * \verbatim</Lib>\endverbatim
 * Jeżeli element XML ma formę skróconą, tzn.
   \verbatim
     <Parametr Name="Rotate"/>
   \endverbatim
 * to wówczas wywołana metoda wywołana zostanie w momencie
 * napotkania sekwencji "/>"
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Lib".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *                      
 */
void XMLInterp4Config::endElement(  const   XMLCh* const    pURI,
                                    const   XMLCh* const    pLocalName,
                                    const   XMLCh* const    pQName
                                 )
{
   char* sURI =  xercesc::XMLString::transcode(pURI);
   char* sElemName = xercesc::XMLString::transcode(pLocalName);
   char* sQName =  xercesc::XMLString::transcode(pQName);
   cout << "   URI: " << sURI << endl;
   cout << " QName: " << sQName << endl;
   cout << "----- Koniec elementu: "<< sElemName << endl;

   xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda wywoływana jest, gdy napotkany zostanie błąd fatalny,
 * np.
  \verbatim
    <Action Name="Rotate">
    </Parametr>
  \endverbatim
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::fatalError(const xercesc::SAXParseException&  rException)
{
   char* sMessage = xercesc::XMLString::transcode(rException.getMessage());
   char* sSystemId = xercesc::XMLString::transcode(rException.getSystemId());

   cerr << "Blad fatalny! " << endl
        << "    Plik:  " << sSystemId << endl
        << "   Linia: " << rException.getLineNumber() << endl
        << " Kolumna: " << rException.getColumnNumber() << endl
        << " Informacja: " << sMessage 
        << endl;

   xercesc::XMLString::release(&sMessage);
   xercesc::XMLString::release(&sSystemId);
}

/*!
 * Metoda jest wywoływana, gdy napotkany zostanie błąd, który nie
 * jest traktowany jako fatalny. W ten sposób traktowane są błędy
 * występujące w opisie gramatyki dokumentu.
 * \param[in] rException - zawiera informacje dotyczące błędu. Informacje
 *                     te to opis błędu oraz numer linii, w której
 *                     wystąpił błąd.
 */
void XMLInterp4Config::error(const xercesc::SAXParseException&  rException)
{
  cerr << "Blad ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}


/*!
 * Metoda wywołana jest w przypadku błędów, które nie są fatalne
 * i mogą być traktowane jako ostrzeżenia.
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::warning(const xercesc::SAXParseException&  rException)  
{
  cerr << "Ostrzezenie ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}
