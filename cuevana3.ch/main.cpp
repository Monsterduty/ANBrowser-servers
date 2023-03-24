#include <QApplication>
#include "play.hpp"
#include "search.hpp"
#include <string>
#include <iostream>

int main( int argc, char *argv[] )
{
	QApplication app(argc, argv);


	if ( std::string( argv[1] ) == "--search" )
		search( argv[2] );
	
	if ( std::string( argv[1] ) == "--play" )
		play(argv[2]);

	if ( std::string( argv[1] ) == "--name" )
		std::cout << "Cuevana3.ch" << std::endl;

	return 0;
}
