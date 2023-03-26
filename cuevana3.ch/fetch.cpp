#include "fetch.hpp"
#include "networking.hpp"
#include <iostream>
#include <sstream>

struct informationClass
{
	std::string description = "";
	std::vector<std::string> episodes;
	std::vector<int> seasons;
	std::string options = "";
};

std::string getDescription( std::string &HTML )
{
	std::string query = "<div class=";
	query += {'"','D','e','s','c','r','i','p','t','i','o','n','"','>'};
	size_t pos = HTML.find( query );

	if ( pos == std::string::npos )
		return "";

	std::string text = HTML.substr( pos + query.length(), HTML.length() );
	text = text.substr( 0, text.find('<') );
	return text;
}

void getSeasons( std::string &HTML, informationClass &information )
{
	std::istringstream search(HTML);
	std::string line = "";

	while ( std::getline( search, line ) )
		if ( line.find("<option value=") != std::string::npos )
		{
			std::string aux = line.substr( line.find("value=") + 7, line.length() );
			information.seasons.push_back( stoi( aux.substr(0, aux.find('"')) ) );
		}
}

void getEpisodes( std::string &HTML, std::string &URL, informationClass &information )
{
	std::string name = URL.substr( URL.find_last_of('/'), URL.length() );
	std::istringstream search(HTML);
	std::string line = "";

	for ( auto item : information.seasons )
		information.episodes.push_back("");

	while( std::getline( search, line ) )
		if ( line.find("/episodio/") != std::string::npos && line.find(name) != std::string::npos )
		{
			std::string aux = line.substr( line.find_last_of('-')+1, line.length() );
			aux = aux.substr(0, aux.find('"') );
			
			for ( int i = 0; i < information.seasons.size(); i++ )
				if ( information.seasons.at(i) == stoi(aux) )
					information.episodes.at(i) += aux.substr( aux.find('x') +1, aux.length() )+ ", ";
		}
}

void fetchSerie( std::string &URL, informationClass &information )
{
	std::string html = download(URL.c_str()).toStdString();

	if ( html == "" )
	{
		std::cout << "ERROR: couldn't download html from cuevana.ch on fetch" << std::endl;
		return;
	}

	information.description = getDescription(html);

	getSeasons(html, information);

	getEpisodes(html, URL, information);
}
void fetchMovie( std::string &URL, informationClass &information )
{
	std::string html = download(URL.c_str()).toStdString();

	information.description = getDescription( html );
	information.episodes.push_back("1,");
}

void fetch( std::string URL )
{
	informationClass information;
	if ( URL.find("/serie/") != std::string::npos )
		fetchSerie(URL, information);
	else
		fetchMovie(URL, information);

	if ( information.description != "" )
		std::cout << "DESCRIPTION :" << information.description << std::endl;

	if ( information.episodes.size() > 0 )
	{
		if ( information.seasons.size() > 1 )
			for ( int i = 0; i < information.seasons.size(); i++ )
				std::cout << "SEASON_"<< information.seasons.at(i) << " :" << information.episodes.at(i) << std::endl;
		else
			std::cout << "EPISODES :" << information.episodes.at(0) << std::endl;
		std::cout << "DOWNLOAD :no" << std::endl;
	}
}
