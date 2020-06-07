#include "./dh.h"
#include "./dhtree.h"

DHEnumerator::DHEnumerator( const int n ) : AbstructEnumerator( n ), N_( n )
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

	std::cerr << "chidlren candidates" << std::endl;
	std::cerr << "str : " << str << std::endl;

	std::set< std::string > results;
	const auto G0 = DHTree( str ).get_graph();

	std::cerr << "G0" << std::endl;
	std::cerr << "N = " << G0.size() << std::endl;
	for ( size_t u = 0; u < G0.size(); ++u )
	{
		std::cerr << u << " : ";
		for ( const int v : G0[u] )
		{
			std::cerr << v << ' ';
		}
		std::cerr << std::endl;
	}

	for ( size_t u = 0; u < G0.size(); ++u )
	{
		{ // make pendant s.t. u is a neck
			auto G = G0;
			const int v = G.size();
			G.emplace_back();
			G[u].push_back( v );
			G[v].push_back( u );
			std::cerr << DHTree( G ).representation() << std::endl;
			results.insert( DHTree( G ).representation() );
		}
		for ( const bool is_strong_twin : { true, false } )
		{ 
			if ( G0.size() == 1 && !is_strong_twin )
			{
				continue;
			}

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
			std::cerr << DHTree( G ).representation() << std::endl;
			results.insert( DHTree( G ).representation() );
		}
	}

// 	std::cerr << "return from chidlren candidates" << std::endl;
	return { std::begin( results ), std::end( results ) };
}

void DHEnumerator::output( std::ostream &out, const std::string &str ) const
{
	const int n = std::count( std::begin( str ), std::end( str ), 'L' );
	std::cout << n << " : " << str << std::endl;
	out << n << " : " << str << std::endl;
}
