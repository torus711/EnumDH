#include "./dh.h"
#include "./dhtree.h"

#define DUMP( x ) std::cerr << #x << " = " << ( x ) << std::endl;

bool are_clique( const std::vector< std::vector< int > > &G, const std::vector< int > &vertices )
{
	for ( const int u : vertices )
	{
		for ( const int v : vertices )
		{
			if ( u == v )
			{
				continue;
			}

			if ( std::find( std::begin( G[u] ), std::end( G[u] ), v ) == std::end( G[u] ) )
			{
				return false;
			}
		}
	}
	return true;
}

DHEnumerator::DHEnumerator( const int n, bool o ) :
	AbstructEnumerator( n ), N_( n ), graph_output( o )
{
	return;
}

std::vector< std::string > DHEnumerator::roots() const
{
	return { "L()" };
}

bool DHEnumerator::recognition( const std::string & ) const
{
	return true;
}

bool DHEnumerator::isomorphic( const std::string &v1, const std::string &v2 ) const
{
	return v1 == v2;
}

std::string DHEnumerator::parent( const std::string &str ) const
{
	if ( str == "" || str == "L()" )
	{
		return "";
	}

	DUMP( use_pendant );
	DHTree dhtree( str );
	dhtree.prune_first_leaf();
	const auto G = dhtree.get_graph();
	return DHTree( G ).representation();
}

std::vector< std::string > DHEnumerator::children_candidates( const std::string &str ) const
{
	if ( count( std::begin( str ), std::end( str ), 'L' ) == N_ )
	{
		return {};
	}
	std::set< std::string > results;
	const auto G0 = DHTree( str ).get_graph();

	for ( size_t u = 0; u < G0.size(); ++u )
	{
		if ( use_pendant )
		{ // make pendant s.t. u is a neck
			std::cerr << "pendant" << std::endl;
			auto G = G0;
			const int v = G.size();
			G.emplace_back();
			G[u].push_back( v );
			G[v].push_back( u );
			results.insert( DHTree( G ).representation() );
		}
		for ( const bool is_strong_twin : { true, false } )
		{ 
			if ( G0.size() == 1 && !is_strong_twin )
			{
				continue;
			}
// 			if ( ptolemaic && !is_strong_twin && !are_clique( G0, G0[u] ) )
// 			{
// 				continue;
// 			}

			if ( ( !is_strong_twin && !use_wtwin ) || ( is_strong_twin && !use_stwin ) )
			{
				continue;
			}

			std::cerr << ( is_strong_twin ? "strong twin" : "weak twin" ) << std::endl;

			auto G = G0;
			const int v = G.size();
			G.push_back( G[u] );
			for ( const int w : G[u] )
			{
				G[w].push_back( v );
			}
			if ( is_strong_twin )
			{
				G[u].push_back( v );
				G[v].push_back( u );
			}
			results.insert( DHTree( G ).representation() );

			output( std::cerr, DHTree( G ).representation() );
			std::cerr << std::endl;
		}
	}

	return { std::begin( results ), std::end( results ) };
}

void DHEnumerator::output( std::ostream &out, const std::string &str ) const
{
	if ( !graph_output )
	{
		const int n = std::count( std::begin( str ), std::end( str ), 'L' );
// 		std::cout << n << " : " << str << std::endl;
		out << n << " : " << str << std::endl;
		return;
	}

	const auto G = DHTree( str ).get_graph();
	std::vector< std::pair< int, int > > es;
	const int N = G.size();
	for ( int u = 0; u < N; ++u )
	{
		for ( const int v : G[u] )
		{
			if ( u < v )
			{
				es.emplace_back( u, v );
			}
		}
	}

	const int M = es.size();
	out << N << ' ' << M << '\n';
	for ( const auto &e : es )
	{
		out << e.first << ' ' << e.second << '\n';
	}
	out << std::flush;
}
