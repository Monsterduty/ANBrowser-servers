//coded my Monsterduty.
#include "qnetworkreply.h"
#include "qnetworkrequest.h"
#include <QApplication>
#include <QAbstractSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QByteArray>
#include <asm-generic/errno-base.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>
#include <utility>

using std::string;
using std::istringstream;
using std::cout;
using std::endl;

QString download( QString link )
{
  QUrl URL(link);
  QNetworkRequest request;// = new QNetworkRequest();
  QNetworkReply *reply = nullptr;
  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QEventLoop loop;

  request.setUrl( URL );
  reply = manager->get( request );

  QAbstractSocket::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  QByteArray bts = reply->readAll();
  return QString( bts );
}

QString sendPOST( QString link, QByteArray data, std::vector<std::pair<QByteArray, QByteArray>> headers = {} )
{
  QUrl URL(link);
  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QNetworkRequest request;
  QNetworkReply *reply = nullptr;
  QEventLoop loop;
  request.setUrl(URL);

  if ( headers.size() > 0 )
    for ( auto item : headers )
      request.setRawHeader(item.first, item.second);

  reply = manager->post(request, data);

  QAbstractSocket::connect( reply, &QNetworkReply::finished, &loop, &QEventLoop::quit );
  loop.exec();

  QByteArray bts = reply->readAll();
  return QString(bts);
}

auto sendGET( QString link, std::vector<std::pair<QByteArray, QByteArray>> headers )
{
  QUrl URL(link);
  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QNetworkRequest request;
  QNetworkReply *reply = nullptr;
  QEventLoop loop;
  request.setUrl(URL);

  for ( auto arg : headers )
    request.setRawHeader( arg.first, arg.second);

  reply = manager->get(request);

  QAbstractSocket::connect( reply, &QNetworkReply::finished, &loop, &QEventLoop::quit );
  loop.exec();

  QByteArray bts = reply->readAll();
  return QString(bts);
}

auto getPOST_Reply( QString link, QByteArray data, std::vector<std::pair<QByteArray, QByteArray>> headers = {} )
{
  QUrl URL(link);
  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QNetworkRequest request;
  QNetworkReply *reply = nullptr;
  QEventLoop loop;
  request.setUrl(URL);

  if ( headers.size() > 0 )
    for ( auto item : headers )
      request.setRawHeader(item.first, item.second);

  reply = manager->post(request, data);

  QAbstractSocket::connect( reply, &QNetworkReply::finished, &loop, &QEventLoop::quit );
  loop.exec();

  //i can't find the location field in the response but it is pressent in the header! :(
  qDebug() << reply->header(QNetworkRequest::LocationHeader);
  // qDebug() << reply->readAll();
  // QByteArray bts = reply->readAll();

  return reply;
}

//===============================================>
//stract Ids related
//===============================================>

void replaceByChar( string &line, string replaceThat, string withThis)
{
  string aux = "";
  for ( char c : replaceThat )
  { 
    while ( line.find(c) != string::npos )
    { 
      aux = line.substr(0, line.find(c));
      aux += withThis;
      aux += line.substr(line.find(c) + 1, line.length() -1 );
      line = aux;
    }
  }
}

void replaceByString( string &line,string word, string byThis )
{
  //cout << line.find(word) << endl;
  if ( line.find(word) == string::npos ) return;

  while ( line.find(word) != string::npos )
  {
    string aux = line.substr(0, line.find(word));
    aux += byThis;
    aux += line.substr(line.find(word) + word.length(), + line.length());
    line = aux;
  }
}

//==================================================>

//==================================================>
//SEARCH ANIME UTILITIES.
//==================================================>

void changeHtmlEntities( string &text )
{
  cout << text << std::endl;
  string opcodes[] = {"&lt;", "&gt;", "&le;", "&ge;", "&amp;apos;", "&amp;", "&quot;", "&#x2014;", "&#39;", "&#039;" };
  string meaning[] = { "<", ">", "≤", "≥", "'", "&", {'"'}, "—", "'", "'" };

  cout << "enter first loop";
  for( int i = 0; i < std::size(opcodes); i++ )
  {
    while ( text.find(opcodes[i]) != string::npos )
    {
      size_t pos = text.find(opcodes[i]);
      string aux = text.substr(0, pos);
      aux += meaning[i];
      aux += text.substr(pos+opcodes[i].length(), text.length());
      text = aux;
    }
  }
  cout << "exit loop" << endl;
  if ( text.find('<') != string::npos )
  {
    size_t pos = text.find('<');
    text = text.substr(0, pos);
  }
  cout << "result: " << text << endl;
}

std::vector<std::pair<string, string>> getAnimeNamesAndLink( string &html )
{
  istringstream search(html);
    string line = "";
    std::vector<std::pair<string, string>> links;
    while( std::getline(search, line) )
    {
      if ( line.find("<a title=") != string::npos && line.find("site-logo") == string::npos )
      {
        size_t pos = line.find("title=") + 7;
        string name = line.substr(pos, line.length());
        name = name.substr(0,name.find('"'));

        pos = line.find("href=") + 6;
        string url = line.substr(pos, line.length());
        url = url.substr(0, url.find('"'));

        std::pair<string, string> info(name, url);

        if ( links.size() > 0 )
        {
          if ( info != links.back() ) links.push_back(info);
        }
          else
            links.push_back(info);
      }
    }
  return links;
}

std::vector<string> getAnimeMiniature(string &html, std::vector<std::pair<string, string>> &references)
{
  istringstream search(html);
  string line = "";
  std::vector<string> links;
  while ( std::getline(search, line) )
  {
    if ( line.find("<img src=") != string::npos )
      for ( auto el : references )
        if ( line.find(el.first) != string::npos )
        {
          size_t pos = line.find("src=") + 5;
          string aux = line.substr(pos, line.length());
          aux = aux.substr(0, aux.find('"'));
          if ( links.size() > 0 )
          {
            if ( links.back() != aux ) links.push_back(aux);
          }
            else
              links.push_back(aux);
        }
  }
  return links;
}

//==================================================>

//==================================================>
//PLAY ANIME.
//==================================================>
std::vector<string> stractOptionsHtml( string html )
{
    std::istringstream search(html);
    string line = "";
    std::vector<string> options;
    while ( std::getline(search,line) )
      if ( line.find("video[") != string::npos && line.find("video_data") == string::npos )
      {
        //for this version i gonna grab the fembed server.
        //if ( line.find("jk.php") == string::npos ) continue;
        size_t pos = line.find("src=") + 5;

        line = line.substr(pos,line.length());
        line = line.substr(0, line.find('"'));
        options.push_back(line);
      }
  return options;
}

void optionJkFembed( string option, std::vector<std::pair<string, string>> &information )
{
   //here is where i need to make a POST request.
    string link = "https://vanfem.com/api/source/" + option.substr(option.find("=")+1, option.length());

    QByteArray postBody;
    postBody.append("r=");
    postBody.append("&");
    postBody.append("d=vanfem.com");
    
    string data = sendPOST( link.c_str(), postBody ).toStdString();
    
    if ( data == "" )
    {
      cout << "ERROR :POST message failed on jkfembed option" << endl;
      exit(1);
    }

    replaceByChar(data, "{}", "\n");
    replaceByChar(data, {0x5C}, "");

    istringstream search(data);
    string line = "";
    std::vector<std::pair<string, string>> videos;

    while ( std::getline(search, line) )
      if ( line.find("file") != string::npos && line.find("label") != string::npos )
      {
        size_t pos = line.find("file") + 7;

        string link = line.substr(pos, line.length());
        link = link.substr(0,link.find('"'));
        

        pos = line.find("label") + 8;
        string name = line.substr(pos, line.length());
        name = name.substr(0, name.find('"'));

        std::pair<string, string> info(name, link);

        information.push_back(info);
      }
}
void optionJkPhp( string option, std::vector<std::pair<string, string>> &information )
{
  string base = "https://jkanime.bz";
  string link = base + option;

  string html = download(link.c_str()).toStdString();
  

  istringstream search(html);
  string line = "";
  string videoFile = "";
  while( std::getline(search,line) )
    if ( line.find("source src=") != string::npos )
    {
      size_t pos = line.find("source src=") + 12;
      videoFile = line.substr(pos,line.length());
      videoFile = videoFile.substr(0, videoFile.find('"'));
    }
  if ( videoFile == "" ) return;
  std::pair<string, string> values("Default", videoFile);
  information.push_back(values);
}

void optionJkDesu( string option, std::vector<std::pair<string, string>> &information )
{
  string base = "https://jkanime.bz";
  string link = base + option;

  string html = download(link.c_str()).toStdString();
  
  istringstream search(html);
  string line = "";

  while (std::getline(search,line))
    if ( line.find("swarmId") != string::npos )
    {
      size_t pos = line.find("swarmId: '")+10;
      string aux = line.substr(pos,line.length());
      aux = aux.substr(0,aux.find("'"));
      std::pair info("Default", aux);
      information.push_back(info);
      return;
    }
}

void optionUm2Php( string option, std::vector<std::pair<string, string>> &information )
{
  string base = "https://jkanime.bz";
  string link = base + option;
  // cout << link << endl;
  // string html = sendPOST(link.c_str(), "v=").toStdString();
  std::pair<QByteArray,QByteArray> header( "Referer", "https://jkanime.bz" );
  std::vector<std::pair<QByteArray,QByteArray>> headers;
  headers.push_back(header);
  string html = sendGET(link.c_str(), headers ).toStdString();

  istringstream search(html);
  string line = "";
  string postData = "";
  while ( std::getline(search, line) )
  {
    if ( line.find("<form action=") != string::npos )
    {
      size_t pos = line.find("=") + 2;
      string aux = line.substr(pos,line.length());
      aux = aux.substr(0,aux.find('"'));
      link = "http://jkanime.bz" + aux;
      // break;
    }

    if ( line.find("<input type") != string::npos )
    {
      size_t pos = line.find("name") + 6;
      string name = line.substr(pos,line.length());
      name = name.substr(0, name.find('"'));

      pos = line.find("value") + 7;
      string data = line.substr(pos,line.length());
      data = data.substr(0,data.find('"'));
      postData = name + "=" + data;
      break;
    }
  }
  
  headers.clear();

  header.first = "Content-Type";
  header.second = "application/x-www-form-urlencoded";
  headers.push_back(header);

  header.first = "Referer";
  header.second = "https://jkanime.bz";
  headers.push_back(header);

  // html = sendPOST(link.c_str(), postData.c_str(), headers).toStdString();
  
  QNetworkReply *reply = getPOST_Reply(link.c_str(), postData.c_str(), headers);


  //i can't find the location field in the response but it is pressent in the header! :(
  qDebug() << reply->rawHeader("Location");

  exit(0);
  cout << html << endl;
  // header.first = "data";
  // header.second = "WlZUcUJEdy9DU0MwOHdMSVI2T0Nlems3UjdadzdBTUZnR1JheiswMXd0cHBqQzVGNjRSLzVZN3lsWkEwUzh2dw";
  // headers.push_back(header);

  exit(0);

  html = sendPOST( "http://jkanime.bz/gsplay/api.php", "v=#UE5aWGllSjk5Q2JhS0ZQOHVPdzVnUW9hR0MwZlpqYml1S0crTlF2WXVCaGNnRDA1angwNkcwVHZLY2pFZFErVHFBSVJiVnE2Zm51VFpLMWFKZjlpbHNYUTU2aGN3Z2F3NUg1bUwzN1A0WXdOZTdMYUk2aW9HeTV1VDFGaks3TWx0OGpTcVY2clFLOHlOekd3emo3RllPT3F3MjdLeXR4Ny9FNVlFQzV4V2NGRXZ5SFMrendJbEVGUGYwK3FrZEdVNDBURjV3ZW53SDdVWE9XUlZrbSs0MzBQQ0FxK0lydTdTTW9aNDl0OUpBdz0", headers).toStdString();
  cout << html << endl;
}

void playInformation( std::vector<std::pair<string, string>> &information, std::vector<string> &options )
{
  if ( information.size() == 1 )
  {
    cout << "LINK :" << information.at(0).second << endl;
    cout << "CURRENT_QUALITY :" << information.at(0).first << endl;
    cout << "AVAILABLE_QUALITIES :" << information.at(0).first << endl;
    cout << "OPTIONS :";
    for ( string element : options )
      cout << element << ", ";
    cout << endl;
  }
  else
  {
    int bestQuality = 0;
    for ( auto element : information )
      if ( stoi( element.first ) > bestQuality ) bestQuality = stoi( element.first );

    for ( auto element : information )
      if ( stoi( element.first ) == bestQuality )
      {
        cout << "link :" << element.second << endl;
        cout << "currentQuality :" << element.first << endl;
        cout << "availableQualities :";
        for ( auto qualities : information )
          cout << qualities.first + ", ";
        cout << endl;
        cout << "options :";
        for ( string element : options )
          cout << element << ", ";
        cout << endl;
      }
  }
}
//==================================================>

//==================================================>
//FETCH ANIME INFORMATION.
//==================================================>
string getDescription( string &html )
{
  string data = "";
  istringstream search(html);
  string line = "";
  while( std::getline(search,line) )
    if ( line.find("</strong>") != string::npos )
    {
      size_t pos = line.find("</strong>") + 9;
      data = line.substr(pos,line.length());
      data = data.substr(0, data.find('<'));
    }
  changeHtmlEntities(data);
  return data;
}

string getCaps( string &html )
{
  istringstream search(html);
  string line = "";
  string serverLink = "";
  while( std::getline(search,line) )
  {
    if ( line.find("pagination_episodes") != string::npos )
      if ( line.find("//") == string::npos )
      {
        size_t pos = line.find("'") + 1;
        serverLink = line.substr(pos, line.length());
        serverLink = serverLink.substr(0, serverLink.find(','));
        break;
      }
  }
  replaceByChar(serverLink, "'+ ","");
  string baseUrl = "https://jkanime.bz";
  serverLink = baseUrl + serverLink;
  string jsonData = download(serverLink.c_str()).toStdString();
  
  if (jsonData == "")
  {
    cout << "ERROR: could not download the jsonData" << endl;
    exit(1);
  }

  replaceByChar(jsonData,"{}","\n");
  
  search = istringstream(jsonData);
  string episodes = "";
  while( std::getline(search,line) )
  {
    if ( line.find("number") != string::npos );
    {
      size_t pos = line.find("number") + 9;
      if ( pos < 10 ) continue;
      string aux = line.substr(pos,line.length());
      aux = aux.substr(0,aux.find('"'));
      episodes += aux + ",";
    }
  }
  return episodes;
}
//==================================================>

int main(int argc, char *argv[])
{
  if ( argc < 2 )
  {
    cout << "nothing to do" << endl;
    exit(1);
  }
  QApplication app(argc, argv);

  if ( string(argv[1]) == "--play" )
  {
    if ( argc < 5 )
    {
      cout << "not enough information. [--play 1 5 https://anime.anime]" << endl;
      exit(1);
    }
    string base = "https://jkanime.bz";
    string availableOptionsToHandle[] = {"jkfembed", "jk.php", "um.php"};
    string correspondingServer[] = { "jkfembed", "jk.php", "um.php" };
    string episode = argv[3];
    string season; //not used on this server.
    string quality = "";
    string option = "";
    if ( argc > 5 )
    {
      for ( int i = 5; i < argc; i++ )
      {
        if ( string(argv[i]) == "--quality" )
        {
          i++;
          quality = string(argv[i]);
        }
        if ( string(argv[i]) == "--option" )
        {
          i++;
          for ( string op : availableOptionsToHandle )
            if ( op.find(argv[i]) != string::npos ) option = op;

            if ( option == "" )
            {
              cout << "ERROR: option [" << argv[i] << "] doesn't exist." << endl;
              exit(1);
            }
        }
      }
    }
    //"https://jkanime.bz/isekai-meikyuu-de-harem-wo-specials/"
    string link = string(argv[4]) + episode + "/"; //argv[4];

    if ( link.find("jkanime.net") != string::npos )
    {
      size_t pos = link.find("jkanime.net") + 11;
      link = link.substr(pos, link.length());
      link = base + link;
    }

    string html = download(link.c_str()).toStdString();

    if ( html == "" )
    {
      cout << "ERROR :Imposible to download anime web page." << endl;
      exit(1);
    }

    std::vector linksAvaliable = stractOptionsHtml(html);
    
    //grabing the name of each option available.
    std::vector<string> options;
    for ( string element : linksAvaliable )
      for ( string op : availableOptionsToHandle )
        if ( element.find(op) != string::npos ) options.push_back(op);

    //fill the information vector with anime links and qualities.
    std::vector<std::pair<string, string>> information;
    for ( string element : linksAvaliable )
    {
      if ( option != "" )
      {
        if ( element.find(option) != string::npos )
        { 
          // if ( element.find("um2.php") != string::npos ) optionUm2Php( element, information );
          if ( element.find("jkfembed.php") != string::npos ) optionJkFembed( element, information );
          if ( element.find("jk.php") != string::npos ) optionJkPhp( element, information );
          if ( element.find("um.php") != string::npos ) optionJkDesu( element, information ); 
        }
      }
      else
      {
        if ( element.find("jkfembed.php") != string::npos ) optionJkFembed( element, information );
        if ( element.find("jk.php") != string::npos ) optionJkPhp( element, information );
        if ( element.find("um.php") != string::npos ) optionJkDesu( element, information );
      }
      if ( information.size() > 0 ) break;
    }

    if ( information.size() < 1 )
    {
      cout << "error getting animes." << endl;
      exit(1);
    }

    playInformation( information, options );
  }
  else
  if ( string(argv[1]) == "--search" )
  {
    if ( argc < 3 )
    {
      cout << "ERROR: --search need a query for the server." << endl;
      exit(1);
    }
    string query = argv[2];
    string link = "https://jkanime.bz/buscar/" + query + "/1/";
    
    string html = download( link.c_str() ).toStdString();
    
    std::vector<std::pair<string, string>> names = getAnimeNamesAndLink(html);
    std::vector<string> miniatures = getAnimeMiniature(html, names);

    for ( int i = 0; i < names.size(); i++ )
    {
      changeHtmlEntities(names.at(i).first);
      cout << "NAME :" << names.at(i).first << endl;
      cout << "MINIATURE :" << miniatures.at(i) << endl;
      cout << "LINK :" << names.at(i).second << endl;
      cout << "END (uwu)" << endl;
    }
  }
  else
  if ( string(argv[1]) == "--fetch" )
  {
    if ( argc < 3 )
    {
      cout << "ERROR: [--fetch] require a link to fetch." << endl;
      exit(1);
    }
    string link = argv[2];
    //string link = "https://jkanime.bz/slime-taoshite-300-nen-shiranai-uchi-ni-level-max-ni-nattemashita/";
    //string link = "https://jkanime.bz/tensei-shitara-slime-datta-ken-2nd-season/";
    //string link = "https://jkanime.bz/tensei-shitara-slime-datta-ken-2nd-season-part-2/";

    string html = download(link.c_str()).toStdString();
    
    string description = getDescription(html);
    string caps = getCaps(html);
    
    cout << "DESCRIPTION :" << description << endl;
    cout << "EPISODES :" << caps << endl;
    cout << "DOWNLOAD :no" << endl;
  }
  else
  if ( string(argv[1]) == "--name" )
  {
    cout << "Jkanime.net" << endl;
  }
  else
  if ( string(argv[1]) == "--logo" )
  {
    cout << "https://cdn.jkdesu.com/assets2/css/img/jkanimenet.png" << endl;
  }
  else
  if ( string(argv[1]) == "--version" )
  {
    cout << "1.0" << endl;
  }
  else
  if ( string(argv[1]) == "--language" )
  {
    cout << "Espanol" << endl;
  }
  else
  {
    cout << "unrecognized option: [" << argv[1] << "]" << endl;
  }

  return 0;
}
