#include <QApplication>
#include "play.hpp"
#include "search.hpp"
#include "fetch.hpp"
#include <string>
#include <iostream>

int main( int argc, char *argv[] )
{
	QApplication app(argc, argv);


	if ( std::string( argv[1] ) == "--search" )
		search( argv[2] );

	if ( std::string( argv[1] ) == "--fetch" )
		fetch( argv[2] );

	if ( std::string( argv[1] ) == "--play" )
	{
		const int episode = stoi(std::string( argv[2] ));
		const int season = stoi( std::string( argv[3] ) );
		play(argv[4], episode, season );
	}

	if ( std::string( argv[1] ) == "--name" )
		std::cout << "Cuevana3.ch" << std::endl;

	return 0;
}
