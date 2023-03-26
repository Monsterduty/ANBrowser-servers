#include "play.hpp"
#include "networking.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

struct playDataStruct
{
	std::string link = "";
	std::string currentQuality = "Default";
	std::vector<std::pair<QByteArray, QByteArray>> headers = {};
	std::vector<std::string> qualities = {};
	std::vector<std::string> optionsAvailable = {};
	//std::vector<std::pair<int, std::string> qualityAndLinkTogether = {};
};

void filterStreamsUrls( std::string &html, std::vector<std::string> &container )
{
	std::istringstream search(html);
	std::string line = "";

	while( std::getline( search, line ) )
		if ( line.find("<li data-video=") != std::string::npos )
		{
			std::string result = "";
			result += line.substr( line.find("data-video=")+12, line.length() );
			result = result.substr( 0, result.find('"') );

			if ( result.find("//") == 0 )
				result = "https:" + result;

			container.push_back(result);
		}
	if ( container.size() == 0 )
	{
		std::cout << "ERROR: couldn't find any streamId on the cuevana.ch page" << std::endl;
		exit(1);
	}
}

namespace streamsss
{
	void manageUrl( const char *URL )
	{
		std::string html = download( URL ).toStdString();

		if ( html == "" )
		{
			std::cout << "ERROR: couldn't download html from streamsss" << std::endl;
			exit(1);
		}

		std::cout << html << std::endl;

		exit(0);
	}
}

namespace dood_wf
{
	std::string getMd5Path( std::string html )
	{
		size_t pos = html.find("/pass_md5/");

		if ( pos == std::string::npos )
		{
			std::cout << "ERROR: couldn't find pass_md5 on the html from dood_wf::getMd5Path" << std::endl;
			exit(1);
		}

		std::string md5Path = html.substr(pos, html.length());
		md5Path = md5Path.substr( 0, md5Path.find("'") );
		return md5Path;
	}

	void manangeUrl( const char *URL, playDataStruct &playData )
	{
		//this server requires some custom http header fields
		std::vector<std::pair<QByteArray, QByteArray>> headers = {};
		std::pair<QByteArray, QByteArray> header("Referer", "https://dood.wf/");//header( "Referer", URL );
		headers.push_back(header);

		std::string html = download(URL, headers).toStdString();

		if ( html == "" )
		{
			std::cout << "ERROR: couldn't download html from server to play" << std::endl;
			exit(1);
		}

		//here we need to get a md5 password from the server to make a http GET request to that md5.
		std::string pass = dood_wf::getMd5Path(html);

		//taking the pass_md5 password we append to it the server domain, in this case "https://dood.wf"
		//making a request to that url will send us the first part of the video url.
		std::string getVideoLink = "https://dood.wf" + pass;

		//this is the first part of the video link that provide us the pass_mp5 request.
		std::string linkFromPass_md5 = download( getVideoLink.c_str(), headers).toStdString();
		
		if ( linkFromPass_md5 == "" )
		{
			std::cout << "ERROR: couldn't download linkFromPass_md5" << std::endl;
			exit(1);
		}

		//to complete complete video url we going to replicate a javascript algorithm located in the web page source code.
		std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; 
	
		//the pass_mp5 string contains at the end a token nedded to perform request to the server
		//we will use it when completing the received url.
		std::string token = pass.substr( pass.find_last_of("/")+1, pass.length() );	

		//for some reason the link need our epoch reference so we provite it too.
		auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

		//in c++ we need to set some parameters to the random function, if not, we will obtain the same number for ever.
		std::srand( epoch );
		//range of random.
		int min = 0, max = characters.length() -1;

		//select a ramdom character from the string provided and append it to the url.
		for ( int i = 0; i < 10; i++ )
			linkFromPass_md5 += characters[ ( std::rand() % ( max - min + 1 ) + min ) ];

		//append to the link the token from the pass_md5 and the epoch value.
		linkFromPass_md5 += "?token=" + token + "&expiry=" + std::to_string( epoch );

		playData.link = linkFromPass_md5;
		playData.headers = headers;
	}
}

namespace hydrax_net
{
	void manageUrl( const char *URL, playDataStruct &playData )
	{
		std::string html = download(URL).toStdString();

		std::cout << "HTML: " << html << std::endl;
	}
}

namespace pelisplay_cc
{
	std::string getMainM3u8( std::string &HTML )
	{
		size_t pos = HTML.find( "sources:[{file:" );
		if ( pos == std::string::npos )
		{
			std::cout << "ERROR: couldn't find 'source:[{file:' on pelisplay_cc::getMainM3u8" << std::endl;
			return "";
		}

		std::string file = HTML.substr( pos + 17,HTML.length() );
		file = file.substr( 0, file.find("'") );
		return file;
	}

	std::vector<std::pair<int, std::string>> getQualities( std::string &mainM3u8 )
	{
		std::string data = download( mainM3u8.c_str() ).toStdString();

		if ( data == "" )
		{
			std::cout << "ERROR: couldn't get data from mainM3u8 file on pelisplay_cc::selectQuality" << std::endl;
			return {};
		}

		std::istringstream search(data);
		std::string line = "";
		std::vector<std::pair<int, std::string>> collectionQualities = { };
		std::pair<int, std::string> cache(0,"");

		while( std::getline( search, line ) )
		{
			if ( line.find( "NAME=" ) != std::string::npos )
			{
				size_t pos = line.find( "NAME=" ) + 6;
				std::string aux =  line.substr( pos, line.length() );
				aux = aux.substr( 0, aux.find('"') );
				cache.first = stoi(aux);
			}

			if ( line.find( "EP." ) != std::string::npos )
			{
				size_t end = line.find( ".m3u8" );
				std::string aux = line.substr( 0, end + 5 );
				cache.second = aux;
			}
			if ( cache.first != 0 && cache.second != "" )
			{
				collectionQualities.push_back(cache);
				cache = std::pair( 0, "" );
			}
		}

		return collectionQualities;
	}

	void makeInfo( std::string &mainM3u8, std::vector<std::pair<int, std::string>> &qualities, playDataStruct &playData, std::string quality = "" )
	{
		int current = 0;

		for( auto item : qualities )
			if ( quality == "" )
			{
				if ( item.first > current )
				{
					current = item.first;
					mainM3u8 = mainM3u8.substr( 0, mainM3u8.find("EP.") ) + item.second;
				}
			}
			else
			{
				if ( item.first == stoi(quality) )
				{
					current  = item.first;
					mainM3u8 = mainM3u8.substr( 0, mainM3u8.find("EP.") ) + item.second;
					break;
				}
			}

		playData.link = mainM3u8;
		if ( current != 0 )
			playData.currentQuality = std::to_string(current) + "p";

		if ( qualities.size() > 1 )
			for ( auto item : qualities )
				playData.qualities.push_back( std::to_string(item.first) + "p" );
	}

	void manageUrl( const char *URL, playDataStruct &playData )
	{
		std::string html = download(URL).toStdString();

		std::string mainM3u8 = getMainM3u8(html);

		auto qualities = getQualities( mainM3u8 );

		if ( qualities.size() == 0 )
		{
			std::cout << "ERROR: couldn't get any quality from the mainM3u8 file on pelisplay_cc::manageUrl" << std::endl;
			return;
		}

		makeInfo(mainM3u8, qualities, playData);
	}
}

void play(const char *query, const int episode, const int season)
{
	std::vector<std::string> streamUrls = {};
	std::string html = "";
	std::string link = std::string( query );

	//html from cuevana.ch
	if ( link.find("/serie/") == std::string::npos )
		html = download(query).toStdString();
	else
	{
		link.replace(link.find("/serie")+1, 5, "episodio");
		link += "-" + std::to_string(season) + "x" + std::to_string(episode);
		html = download(link.c_str()).toStdString();
	}

	if ( html == "" )
	{
		std::cout << "ERROR: couldn't donwload html from cuevana.ch on play" << std::endl;
		exit(1);
	}

	filterStreamsUrls(html, streamUrls);

	if ( html == "" )
	{
		std::cout << "ERROR: couldn't get any streamUrl from cuevana.ch on play" << std::endl;
		exit(1);
	}

	playDataStruct playData;
	std::string ableToManage[] = { "dood.wf", "pelisplay.cc" };
	std::string optionsToShow = "";

	for ( auto item : streamUrls )
		for ( auto manage : ableToManage )
			if ( item.find( manage ) != std::string::npos && optionsToShow.find( manage ) == std::string::npos )
				optionsToShow += manage += ", ";


	for ( auto item : streamUrls )
	{
		//std::cout << item << std::endl;
		//if ( item.find("streamsss.net") != std::string::npos ) streamsss::manageUrl( item.c_str() );
		if ( item.find("dood.wf") != std::string::npos ) dood_wf::manangeUrl(item.c_str(), playData);
		//if ( item.find("hydrax.net") != std::string::npos ) hydrax_net::manageUrl(item.c_str(), playData);
		if ( item.find("pelisplay.cc") != std::string::npos ) pelisplay_cc::manageUrl(item.c_str(), playData);
		if ( playData.link != "" )
			break;
	}

	if ( playData.link == "" )
	{
		std::cout << "ERROR: there's no handler for the given options:" << std::endl;
		for ( auto item : streamUrls )
			std::cout << item << std::endl;
		return;
	}

	std::cout << "LINK :" << playData.link << std::endl;
	if ( playData.headers.size() > 0 )
		for ( auto item : playData.headers )
			std::cout << "HTTP_HEADER :[" << item.first.toStdString() << "][" << item.second.toStdString() << "]" << std::endl;
	std::cout << "CURRENT_QUALITY :" << playData.currentQuality << std::endl;
	if ( playData.qualities.size() > 1 )
	{
		std::cout << "availableQualities :";
		for ( auto item : playData.qualities )
			std::cout << item + ", ";
		std::cout << std::endl;
	}
	else
		std::cout << "AVAILABLE_QUALITIES :Default" << std::endl;
	std::cout << "OPTIONS :" << optionsToShow << std::endl;
}
