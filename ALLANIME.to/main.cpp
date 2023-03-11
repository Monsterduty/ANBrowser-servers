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
#include <string>
#include <iterator>
#include <sstream>
#include <cctype>

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
	string opcodes[] = {"&lt;", "&gt;", "&le;", "&ge;", "&amp;apos;", "&amp;", "&quot;", "&#x2014;", "&#39;", "&#039;" };
	string meaning[] = { "<", ">", "≤", "≥", "'", "&", {'"'}, "—", "'", "'" };

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
		char source[] = {'s','o','u','r','c','e','U','r','l','"',':','"'};
		if ( buffer.find(source) != string::npos )
			if ( buffer.find("clock?id=") != string::npos )
			{
				//cout << buffer << endl;
				//continue;
				size_t pos1 = buffer.find("clock?id=");
				string id = buffer.substr(pos1 + 9, buffer.length());
				id = id.substr(0, id.find('"'));
				
				pos1 = buffer.find("sourceName");
				string name = buffer.substr(pos1+13, buffer.length());
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

string getEpisodeFromM3u8( string id )
{
	return "";
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
		string idAndName = fetch.substr(fetch.find("anime/") +6, fetch.length());
		
		string urlTemplate = "https://allanime.to/watch/"+ idAndName +"/episode-" + episode +"-"+ mode; //$mode"
	
		string html = download( QString(urlTemplate.c_str()) ).toStdString();

		string allTheIds = GetIds(html);

		istringstream search(allTheIds);
		string line = "";
		html = "";
		string optionsAvailable = "";
		//just to get the available options.
		while ( std::getline(search, line) )
			optionsAvailable += line.substr(0, line.find(" ")) + ", ";

		search.clear();
		search.seekg(0);
		line = "";
		string optionsAvailableToManage[] = {"Luf-mp4"};
		string optionsToShow = "";
		for( string item : optionsAvailableToManage )
		if ( optionsAvailable.find(item) != string::npos )
			optionsToShow += item + ", ";

		while( std::getline(search,line) )
		{
			bool keep = false;
			for ( string item : optionsAvailableToManage )
				if (line.find(item) != string::npos) keep = true;

			if ( !keep )
				continue;

			string id = line.substr(line.find(":") +1, line.length());
			if ( id.find("&referer") != string::npos )
				id = id.substr(0, id.find("&referer"));

			string prefix = "https://blog.allanime.pro/apivtwo/clock.json\?id=" + id;

			html = download(prefix.c_str()).toStdString();

			if ( option == "" )
			{
				if ( html != "" ) break;
			}
			else
			{
				if ( line.find(option) != string::npos )
					if ( html != "" ) break;
			}
		}

		size_t linkPos = html.find("https://");
		if ( linkPos == string::npos )
		{
			cout << "error geting link" << endl;
			cout << "HTML: " << html << endl;
			exit(1);
		}

		string streamingData = html.substr(linkPos, html.length());
		streamingData = streamingData.substr(0, streamingData.find('"'));

		string link = "";
		string qualitiesAvailable = "";
		string episodeKey = "";
		string currentQuality = "";

		if ( streamingData.find(".m3u8") != string::npos )
		{
			link = download(streamingData.c_str()).toStdString();
			
			istringstream search(link);
			string line = "";

			while ( std::getline(search, line) )
			{
				size_t namePos = line.find("NAME=");
				size_t keyPos = line.find("ep.");

				if ( namePos != string::npos )
				{
					string aux = line.substr( namePos+6, line.length() );
					aux = aux.substr(0, aux.find('"'));
					qualitiesAvailable += aux + ", ";
					currentQuality = aux;
				}
				if ( keyPos != string::npos )
				{
					if ( quality == "" )
					{
						episodeKey = line;
					}
					else
					{
						if ( line.find(quality) != string::npos )
							episodeKey = line;
					}
				}
			}
			size_t change = streamingData.find("ep.");

			if ( change == string::npos )
			{
				cout << "ERROR: not ep.episodekey find on the link." << endl;
				cout << "LINK: " << streamingData << endl;
				exit(1);
			}

			if ( currentQuality == "" )
				currentQuality = "Default";

			if ( qualitiesAvailable == "" )
				qualitiesAvailable = "Default";

			streamingData = streamingData.substr(0, change);
			streamingData += episodeKey;
			cout << "link :" << streamingData << endl;
			cout << "currentQuality :" + currentQuality << endl;
			cout << "availableQualities :" << qualitiesAvailable << endl;
			cout << "options :" << optionsToShow << endl;
		}
		else
		if ( streamingData.find(".mp4") != string::npos )
		{
			cout << "link :" << streamingData << endl;
			cout << "currentQuality :" << "Default" << endl;
			cout << "availableQualities :" << "Default" << endl;
			cout << "options :" << "Default" << endl;
		}
		else
		{
			cout << "ERROR: unrecognized format" << endl;
			cout << "URL: " << streamingData << endl;
			exit(1);
		}

		//cout << streamingData << endl;
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
		string urlTemplate = "https://api.allanime.to/allanimeapi?variables=%7B%22search%22%3A%7B%22query%22%3A%22" + animeName + "%22%2C%22allowAdult%22%3Afalse%2C%22allowUnknown%22%3Afalse%7D%2C%22limit%22%3A26%2C%22page%22%3A1%2C%22translationType%22%3A%22sub%22%2C%22countryOrigin%22%3A%22ALL%22%7D&extensions=%7B%22persistedQuery%22%3A%7B%22version%22%3A1%2C%22sha256Hash%22%3A%22c4305f3918591071dfecd081da12243725364f6b7dd92072df09d915e390b1b7%22%7D%7D";
		string jsonData = download(urlTemplate.c_str()).toStdString();

		replaceByChar(jsonData, "{}", "\n");
		istringstream search(jsonData);
		string buffer = "";
		std::vector<string> final;

		while ( std::getline(search,buffer) )
		{
			if ( buffer.find("_id") != string::npos )
			{
				size_t idPos = buffer.find("id") + 5;
				size_t namePos = buffer.find("name") + 7;
				size_t miniaturePos = buffer.find("thumbnail") + 12;

				//manage link.
				string linkTemplate = "https://allanime.to/anime/";
				string aux = buffer.substr(idPos, buffer.length());
				aux = aux.substr(0, aux.find({'"',','}));
				linkTemplate += aux + "/";
				//final.push_back( "link :" + aux );

				//manage name.
				aux = buffer.substr(namePos, buffer.length());
				aux = aux.substr(0, aux.find({'"',','}));
				linkTemplate += generateAnimeLinkName(aux) + "/";
				final.push_back("link :"+linkTemplate);
				changeHtmlEntities(aux);
				final.push_back("name :" + aux);

				//manage miniature,
				aux = buffer.substr(miniaturePos, buffer.length());
				aux = aux.substr(0, aux.find({'"',','}));
				if ( aux.find("https") == string::npos )
					aux = "https://wp.youtube-anime.com/aln.youtube-anime.com/" + aux;
				final.push_back("miniature :" + aux);
				final.push_back("end (uwu)");
			}
		}
		// cout << jsonData << endl;
		for ( auto s : final )
		cout << s << endl;
	}
	else
	if ( string(argv[1]) == "--fetch" )
	{
		string data = download(argv[2]).toStdString();

		replaceByChar(data, "{}", "\n");
		istringstream streamLine(data);
		string line = "";
		//cout << data << endl;
		while ( std::getline(streamLine,line) )
		{
			string sub = "sub";
			sub += {0x5C, '"', ':', '['};
			if ( line.find(sub) != string::npos )
			{
				size_t startPos = line.find("sub:[") + 5;
				replaceByChar(line, {'"'}, "");

				line = line.substr(startPos, line.length());
				line = line.substr(0, line.find(']'));

				string episodeList = "";
				for ( char c : line )
					if ( c == ',' && episodeList != "" || isdigit(c) ) episodeList.push_back(c);

				cout << "episodeList :" << episodeList << "," << endl;
			}
			if ( line.find("description" +string({'"',':'}) ) < 4 )
			{
				size_t startPos = line.find(':') + 2;
				line = line.substr(startPos, line.length());
				line = line.substr(0, line.find('"'));
				changeHtmlEntities(line);
				cout << "description :" << line << endl;
			}
		}

		cout << "seasonList :undefined" << endl;
		cout << "download :no" << endl;
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
