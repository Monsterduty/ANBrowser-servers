#include <iostream>
#include "networking.hpp"
#include "search.hpp"
#include <string>
#include <sstream>

void getLinks( std::string html, std::vector<std::string> &container )
{
	std::istringstream search( html );
	std::string line = "";

	while( std::getline(search, line) )
		if ( line.find("href=") != std::string::npos )
		{
			size_t pos = line.find('"')+ 1;
			std::string aux = line.substr(pos, line.length());
			aux = aux.substr( 0, aux.find('"') );
			container.push_back(aux);
		}
	if ( container.size() == 0 )
		std::cout << "ERROR: couldn't find any link on search::getLinks" << std::endl;
}

void getMiniatures( std::string html, std::vector<std::string> &container )
{
	std::istringstream search( html );
	std::string line = "";

	while( std::getline( search, line ) )
		if ( line.find("src=") != std::string::npos )
		{
			size_t pos = line.find( '"' )+1;
			std::string aux = line.substr(pos, line.length());
			aux = aux.substr( 0, aux.find('"') );
			container.push_back(aux);
		}
	if ( container.size() == 0 )
		std::cout << "ERROR: couldn't find any miniature on search::getMiniatures" << std::endl;
}

void getTitles( std::string html, std::vector<std::string> &container )
{
	std::istringstream search( html );
	std::string line = "";

	while( std::getline( search, line ) )
		if ( line.find("Title") != std::string::npos )
		{
			size_t pos = line.find(">")+ 1;
			std::string aux = line.substr( pos, line.length() );
			aux = aux.substr( 0, aux.find("<") );
			container.push_back(aux);
		}
	if ( container.size() == 0 )
		std::cout << "ERROR: couldn't find any Title on search::getTitle" << std::endl;
}

void search( const char *query )
{
	std::string base = "https://www6.cuevana3.ch/search.html?keyword=";
	base += query;

	std::vector<std::pair<QByteArray, QByteArray>> headers;

	std::pair header( "X-Requested-With", "XMLHttpRequest" );

	headers.push_back(header);

	std::string html = download(base.c_str(), headers).toStdString();

	std::vector<std::string> links, miniatures, titles;

	getLinks(html, links);
	getMiniatures( html, miniatures );
	getTitles( html, titles );

	std::string prefix = "https://www6.cuevana3.ch";

	for ( int i = 0; i < links.size(); i++ )
	{
		std::cout << "name :" << titles.at(i) << std::endl;
		std::cout << "link :" << prefix + links.at(i) << std::endl;
		std::cout << "miniature :https:" << miniatures.at(i) << std::endl;
		std::cout << "end (uwu)" << std::endl;
	}
}
