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

	return 0;
}
