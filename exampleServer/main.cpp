#include <iostream>
#include <cstring>

int main( int argc, char *argv[] )
{

	if ( !strcmp( argv[1], "--search" ) )
	{
		std::cout << "enter to search" << std::endl;
		int times = 1;

		if ( argv[2] != nullptr )
			if ( strcmp( argv[2], "" ) )
			times = stoi( std::string(argv[2]) );

		int loop = 1;

		if ( times > 1 ) loop = times;

		for ( int i = 0; i < loop; i++ )
		{
			std::cout << "NAME :testName" << std::endl;
			std::cout << "LINK :https://www/testLink.com" << std::endl;
			std::cout << "MINIATURE :https://upload.wikimedia.org/wikipedia/commons/thumb/c/c5/Big_buck_bunny_poster_big.jpg/1200px-Big_buck_bunny_poster_big.jpg" << std::endl;
			std::cout << "END (uwu)" << std::endl;
		}
	}

	if ( !strcmp( argv[1], "--fetch" ) )
	{
		std::cout << "DESCRIPTION :you must fill this field with the information of the serie/movie bein requested" << std::endl;
		std::cout << "EPISODES :1, 2, 3, 4, 5, 6, 7, 8" << std::endl;
		std::cout << "DOWNLOAD :no" << std::endl;
	}

	if ( !strcmp( argv[1], "--play" ) )
	{
		std::cout << "LINK :https://www.testLink.com" << std::endl;
		std::cout << "HTTP_HEADER :[Referer][https://www.testReferer.com]" << std::endl;
		std::cout << "CURRENT_QUALITY :720p" << std::endl;
		std::cout << "AVAILABLE_QUALITIES :360p, 480p, 720p, 1080p, 4k" << std::endl;
		std::cout << "OPTIONS :fembedServer, hydrax, masterHQ, LuffMP4, jkdesu" << std::endl;
	}

	if ( !strcmp( argv[1], "--name" ) )
		std::cout << "Test Server" << std::endl;

	return 0;
}
