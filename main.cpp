#include <bits/stdc++.h>

#include "./cmdline.h"

int main( int argc, char *argv[] )
{
	cmdline::parser optparser;
	optparser.add< int >( "size", 'n', "Uppberbound of # of vertices of graph to enumerate", true );
	optparser.add( "bipartite", 'b', "Enumerate bipartite distanced-hereditary graphs only" );
	if ( !optparser.parse( argc, argv ) )
	{
		std::cerr << optparser.error_full();
		std::cerr << optparser.usage();
		std::cerr << std::flush;
		return 1;
	}

	const int N = optparser.get< int >( "size" );
	const bool bipartite = optparser.exist( "bipartite" );
	if ( N <= 0 )
	{
		std::cerr << "The option -N ( or -size ) should be a positive integer." << std::endl;
		return 2;
	}

	Enumerator enumerator( N );
	enumerator.exec( std::cout );

	return 0;
}
