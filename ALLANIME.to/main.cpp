//coded by Monsterduty.
#include <QApplication>
#include <QAbstractSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QByteArray>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>
#include <nlohmann/json.hpp>

using std::string;
using std::istringstream;
using std::cout;
using std::endl;
using std::istringstream;

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

void changeHtmlEntities( string &text )
{
	string opcodes[] = {"&lt;","u003C", "&gt;", "u003C", "&le;", "&ge;", "&amp;apos;", "&amp;", "&quot;", "&#x2014;", "&#39;", "&#039;", "&apos;", "<br>", "<i>", "</i>" };
	string meaning[] = { "<", "<", ">", ">", "≤", "≥", "'", "&", {'"'}, "—", "'", "'", "'", "", "", "" };

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
	if ( text.find('<') != string::npos )
	{
		size_t pos = text.find('<');
		text = text.substr(0, pos);
	}
}

void stractNameAndId(string &line)
{
	std::istringstream search(line);
	string buffer;
	string ret = "";
	while ( std::getline(search,buffer) )
	{
		const char *source = ".sourceUrl=/";//{'s','o','u','r','c','e','U','r','l','"',':','"'};
		if ( buffer.find(source) != string::npos )
		continue;
			if ( buffer.find("clock?id=") != string::npos )
			{
				//cout << buffer << endl;
				//continue;
				size_t pos1 = buffer.find("clock?id=");
				string id = buffer.substr(pos1 + 9, buffer.length());
				id = id.substr(0, id.find('"'));
				
				pos1 = buffer.find("sourceName");
				string name = buffer.substr(pos1+12, buffer.length());
				name = name.substr(0, name.find('"'));
				ret += name + " :" + id + "\n";
			}
		line = ret;
	}
}

string GetIds( string &page )
{
	istringstream search(page);
	string buf = "";
	string ret = "";

	//problem with endlines.
	while ( std::getline( search, buf ) )
	{
		replaceByChar(buf, "{}", "\n");
		const char a[] = {0x5C, 'u', '0', '0', '2', 'F'};
		replaceByString(buf, a, "/");
		string backSlash = "";
		backSlash.push_back(a[0]);
		replaceByString(buf, backSlash, "" );
		if ( buf.find("sourceUrl") != string::npos )
			stractNameAndId(buf);
		else
			continue;
		
		ret += buf;
	}
	return ret;
}

//==================================================>

//==================================================>
//SEARCH ANIME UTILITIES.
//==================================================>

string generateAnimeLinkName(string name)
{
	string ret = "";
	string symbols = ":!–";
	for ( char c : name )
	{
		if ( c == ' ' || c == 0x27 ) ret += "-";
		else
		if ( symbols.find(c) == string::npos ) ret.push_back(tolower(c));
	}
	
	if ( ret.find("--") != string::npos )
	{
		size_t pos = ret.find("--");
		while( pos != string::npos )
		{
			string aux = ret.substr(0, pos+1);
			ret = aux + ret.substr(pos+2, ret.length());
			pos = ret.find("--");
		}
	}

	return ret;
}

//==================================================>

void getEpisodeM3u8FromWww005_anifastcdn_info( string link, std::vector<std::pair<string, string>> &nameAndLink )
{
	string m3u8Data = download(link.c_str()).toStdString();

	istringstream search(m3u8Data);
	string line = "";
	std::vector<string> names;
	std::vector<string> Links;
	while( std::getline( search, line ) )
	{
		string name = "NAME=";
		if ( line.find(name) != string::npos )
		{
			size_t pos = line.find(name) + name.size() + 1;
			string aux = line.substr(pos, line.length());
			aux = aux.substr(0, aux.find('"'));
			names.push_back(aux);
			continue;
		}
		if ( line.find_last_of("ep.") != string::npos )
		{
			//since in this option the links are based on the parent link
			//we need to cut the parent link and append the line with the format
			//and quality desired.
			size_t pos = link.find("ep.");
			string aux = link.substr(0, pos);
			aux += line;
			Links.push_back(aux);
			continue;
		}
	}

	if ( names.size() != Links.size() )
	{
		cout <<"WARNING: lost data on option from www005.anifastcdn.info" << endl;
		names.clear();
		Links.clear();
		return;
	}

	for ( int i = 0; i < names.size(); i++ )
	{
		std::pair<string, string> data( names.at(i), Links.at(i) );
		nameAndLink.push_back(data);
	}
}

void Luf_mp4Option( nlohmann::json &itemFromjsonData, std::vector<std::pair<string, string>> &nameAndLink )
{
	string id = itemFromjsonData["sourceUrl"].get<string>();//data.substr(data.find(":") +1, data.length());
	if ( id.find("&referer") != string::npos )
		id = id.substr(0, id.find("&referer"));

	id = id.substr( id.find("id=")+ 3, id.length() );

	string prefix = "https://blog.allanime.pro/apivtwo/clock.json\?id=" + id;//id;

	//string prefix = "https://blog.allanime.pro" + id;

	//cout << prefix << endl;
	//return;

	string jsonData = download(prefix.c_str()).toStdString();

	if ( jsonData == "" )
	{
		cout << "ERROR: Couldn't get the json data from Luf_mp4" << endl;
		exit(1);
	}

	std::vector<string> links;

	replaceByChar(jsonData, "{}", "\n");

	std::istringstream search(jsonData);
	string line = "";

	while( std::getline(search, line) )
	{
		if ( line.find("https://") != string::npos )
		{
			size_t pos = line.find("https://");
			string aux = line.substr( pos, line.length());
			aux = aux.substr( 0, aux.find({'"',','}));
			links.push_back(aux);
		}
	}

	if ( links.size() == 0 )
	{
		cout << "ERROR: Couldn't stract link from JSON data Luf-mp4" << endl;
		exit(1);
	}

	for ( int i = links.size() -1; i >= 0; i-- )
	{
		//"www005.anifastcdn.info"
		//"www025.anifastcdn.info"
		if ( links.at(i).find(".anifastcdn.info") != string::npos )
			getEpisodeM3u8FromWww005_anifastcdn_info( links.at(i), nameAndLink);

		if ( nameAndLink.size() > 0 )
			break;
	}

	if ( nameAndLink.size() == 0 )
	{
		cout << "ERROR: There's no option handler for the links requested" << endl;
		cout << "list of links:" << endl;
		for( auto link : links )
			cout << link << endl;
		exit(1);
	}
}

void playInfo( std::vector<std::pair<string, string>> &nameAndLink, string &optionsToShow, string &quality )
{
	if ( nameAndLink.size() == 1 )
	{
		cout << "link :" << nameAndLink.at(0).second << endl;
		cout << "currentQuality :" << nameAndLink.at(0).first << endl;
		cout << "availableQualities :" << nameAndLink.at(0).first << endl;
		cout << "options :" << optionsToShow << endl;
		return;
	}

	int biggest = 0;
	if ( quality == "" )
	{
		for ( auto item : nameAndLink )
			if ( stoi(item.first) > biggest )
				biggest = stoi(item.first);

		for ( auto item : nameAndLink )
			if ( stoi(item.first) == biggest )
			{
				cout << "LINK :" << item.second << endl;
				cout << "CURRENT_QUALITY :" << item.first << endl;
				cout << "AVAILABLE_QUALITIES :";
				for ( auto qualities : nameAndLink )
					cout << qualities.first + ", ";
				cout << endl;
				cout << "OPTIONS :" << optionsToShow << endl;
				return;
			}
	}
	else
	{
		for ( auto item : nameAndLink )
			if ( item.first == quality )
			{
				cout << "LINK :" << item.second << endl;
				cout << "CURRENT_QUALITY :" << item.first << endl;
				cout << "AVAILABLE_QUALITIES :";
				for ( auto qualities : nameAndLink )
					cout << qualities.first + ", ";
				cout << endl;
				cout << "OPTIONS :" << optionsToShow << endl;
				return;
			}
	}	
}

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
		string season = argv[2]; //not nedded on this server.
		string episode = argv[3];
		string fetch = argv[4];
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
					option = argv[i];
				}
			}
		}
		string mode = "sub";
		//https://allanime.site/anime/gHQe2eBBh57QdC9hZ/otonari-no-tenshi-sama-ni-itsunomanika-dame-ningen-ni-sareteita-ken
		//https://api.allanime.to/allanimeapi?variables={%22showId%22:%22cstcbG4EquLyDnAwN%22,%22translationType%22:%22sub%22,%22episodeString%22:%221%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%220ac09728ee9d556967c1a60bbcf55a9f58b4112006d09a258356aeafe1c33889%22}}
		string idAndName = fetch.substr(fetch.find("anime/") +6, fetch.length());
		
		//string urlTemplate = "https://allanime.to/watch/"+ fetch +"/episode-" + episode +"-"+ mode; //$mode"

		//cout << "id: " << fetch << " mode: " << mode << " episode: " << episode << endl;

		string urlTemplate = "https://api.allanime.to/allanimeapi?variables={%22showId%22:%22" + fetch + "%22,%22translationType%22:%22" + mode + "%22,%22episodeString%22:%22" + episode + "%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%220ac09728ee9d556967c1a60bbcf55a9f58b4112006d09a258356aeafe1c33889%22}}";

		string serverJsonData = download( QString(urlTemplate.c_str()) ).toStdString();

		if ( serverJsonData == "" )
		{
			cout << "ERROR: couldn't download the json data from the server on search method." << endl;
			cout << "LINK: " << urlTemplate << endl;
			return 1;
		}

		nlohmann::json jsonData;
		jsonData = nlohmann::json::parse( serverJsonData );

		string optionsAvailableToManage[] = {"Luf-mp4"};
		string optionsToShow = "";

		for( auto item : jsonData["data"]["episode"]["sourceUrls"] )
			for ( string &name : optionsAvailableToManage )
				if ( item["sourceName"].get<string>() == name )
					optionsToShow += name + ", ";

		std::vector<std::pair<string, string>> nameAndLink;

		for( auto item : jsonData["data"]["episode"]["sourceUrls"] )
		{
			if ( option != "" )
			{
				if(option.find("Luf-mp4") != string::npos) Luf_mp4Option( item, nameAndLink);
			}
			else
			{
				if ( item["sourceName"].get<string>() == "Luf-mp4" ) Luf_mp4Option( item, nameAndLink);
			}

			if ( nameAndLink.size() != 0 ) break;
		}

		if ( nameAndLink.size() == 0 )
		{
			cout << "ERROR: Couldn't find anime data" << endl;
			cout << "Ids available:" << endl;
			return 1;
		}

		playInfo(nameAndLink, optionsToShow, quality);
	}
	else
	if ( string(argv[1]) == "--search" )
	{
		if ( argc < 3 )
		{
			cout << "ERROR: --search require a pattern to search" << endl;
			exit(1);
		}

		string animeName = argv[2];
		string mode = "sub";
		size_t spacePos = animeName.find(" ");

		//change space characters with the string %20.
		while ( spacePos != string::npos )
		{
			animeName = animeName.substr(0, spacePos) + "%20" + animeName.substr(spacePos +1, animeName.length());
			spacePos = animeName.find(" ");
		}

		// string urlTemplate = "https://api.allanime.to/allanimeapi?variables=%7B%22search%22%3A%7B%22allowAdult%22%3Atrue%2C%22allowUnknown%22%3Atrue%2C%22query%22%3A%22" + animeName + "%22%7D%2C%22limit%22%3A40%2C%22page%22%3A1%2C%22translationType%22%3A%22" + mode + "%22%2C%22countryOrigin%22%3A%22ALL%22%7D&extensions=%7B%22persistedQuery%22%3A%7B%22version%22%3A1%2C%22sha256Hash%22%3A%229c7a8bc1e095a34f2972699e8105f7aaf9082c6e1ccd56eab99c2f1a971152c6%22%7D%7D";
		//string urlTemplate = "https://api.allanime.to/allanimeapi?variables=%7B%22search%22%3A%7B%22query%22%3A%22" + animeName + "%22%2C%22allowAdult%22%3Afalse%2C%22allowUnknown%22%3Afalse%7D%2C%22limit%22%3A26%2C%22page%22%3A1%2C%22translationType%22%3A%22sub%22%2C%22countryOrigin%22%3A%22ALL%22%7D&extensions=%7B%22persistedQuery%22%3A%7B%22version%22%3A1%2C%22sha256Hash%22%3A%222f9e95bd0291bafd5351f9ba0ee042086fade20eb4cace7fa69f502c45df93e1%22%7D%7D";
		
		string  urlTemplate = "https://api.allanime.to/allanimeapi?variables={%22search%22:{%22query%22:%22" + animeName + "%22,%22allowAdult%22:true,%22allowUnknown%22:false},%22limit%22:50,%22page%22:1,%22translationType%22:%22sub%22,%22countryOrigin%22:%22ALL%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%22b645a686b1988327795e1203867ed24f27c6338b41e5e3412fc1478a8ab6774e%22}}";

		string jsonData = download(urlTemplate.c_str()).toStdString();

		if ( jsonData == "" )
		{
			cout << "ERROR: Failed to download jsonData" << endl;
			cout << "LINK: " << urlTemplate << endl;
			return 1;
		}

		nlohmann::json info;
		info = nlohmann::json::parse( jsonData );

		if ( info["data"]["shows"]["edges"].size() < 1 )
		{
			cout << "ERROR: there's not data on the json edges" << endl;
			cout << info << endl;
			return 1;
		}

		for ( auto item : info["data"]["shows"]["edges"] )
		{
			string miniature = item["thumbnail"].get<string>();

			if ( miniature.find("https") == string::npos )
				miniature = "https://wp.youtube-anime.com/aln.youtube-anime.com/" + miniature;

			cout << "NAME :" << item["name"].get<string>() << endl;
			cout << "LINK :" << item["_id"].get<string>() << endl;
			cout << "MINIATURE :" << miniature << endl;
			cout << "END (uwu)" << endl;
		}

		return 0;
	}
	else
	if ( string(argv[1]) == "--fetch" )
	{
		//https://api.allanime.to/allanimeapi?variables={%22_id%22:%22cstcbG4EquLyDnAwN%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%22d6069285a58a25defe4a217b82140c6da891605c20e510d4683ae73190831ab0%22}}
		string data =  "";
		string link( argv[2] );

		//link = "https://api.allanime.to/allanimeapi?variables={%22_id%22:%22" + link + "%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%22b645a686b1988327795e1203867ed24f27c6338b41e5e3412fc1478a8ab6774e%22}}";

		link = "https://api.allanime.to/allanimeapi?variables={%22_id%22:%22" + link + "%22}&extensions={%22persistedQuery%22:{%22version%22:1,%22sha256Hash%22:%22d6069285a58a25defe4a217b82140c6da891605c20e510d4683ae73190831ab0%22}}";

		data = download( link.c_str() ).toStdString();

		if ( data == "" )
		{
			std::cout << "ERROR: couldn't download JSON data from allanime api on fetch" << std::endl;
			return 1;
		}

		nlohmann::json info;
		info = nlohmann::json::parse(data);

		if ( !info.contains("data") )
		{
			std::cout << "ERROR : JSON data doesn't contains a [data] field!" << std::endl;
			return 1;
		}

		if ( !info["data"].contains("show") )
		{
			std::cout << "ERROR: JSON data doesn't contains a [data][show] field!" << std::endl;
			std::cout << "JSON: " << info << std::endl;
			return 1;
		}

		if ( !info["data"]["show"].contains("availableEpisodesDetail") )
		{
			std::cout << "ERROR: JSON data doesn't contains a [data][show][availableEpisodesDetail] field!" << std::endl;
			return 1;
		}

		if ( !info["data"]["show"]["availableEpisodesDetail"].contains("sub") )
		{
			std::cout << "ERROR: JSON data doesn't contains a [data][show][availableEpisodesDetail][sub] field!" << std::endl;
			return 1;
		}

		if ( info["data"]["show"]["availableEpisodesDetail"]["sub"].size() == 0 )
		{
			std::cout << "ERROR: JSON data doesn't contains any episode to show" << std::endl;
			return 1;
		}

		string episodeList = "";
		for ( auto item : info["data"]["show"]["availableEpisodesDetail"]["sub"] )
			episodeList += item.get<std::string>() + ",";
		string description = "";

		if ( info["data"]["show"]["description"].size() > 0 )
		{
			description = info["data"]["show"]["description"].get<string>();
			changeHtmlEntities( description );
		}
		cout << "EPISODES :" << episodeList << "," << endl;
		if ( description != "" )
			cout << "DESCRIPTION :" << description << endl;
		//cout << "seasonList :undefined" << endl;
		cout << "DOWNLOAD :no" << endl;
	}
	else
	if ( string(argv[1]) == "--name" )
	{
		cout << "Allanime.to" << endl;
	}
	else
	if ( string(argv[1]) == "--logo" )
	{
		cout << "https://allanime.to/pics/allanime.png" << endl;
	}
	else
	if ( string(argv[1]) == "--version" )
	{
		cout << "1.0" << endl;
	}
	else
	if ( string(argv[1]) == "--language" )
	{
		cout << "english" << endl;
	}
	else
	{
		cout << "unrecognized option: [" << argv[1] << "]" << endl;
	}

	return 0;
}
