#include "./dh.h"
#include "./dhtree.h"

DHEnumerator::DHEnumerator( const int n ) : AbstructEnumerator( n ), N_( n )
{
	return;
}

std::vector< std::string > DHEnumerator::roots() const
{
	return { "L()", "S(L()L())", "P(L()L()L())" };
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
	if ( str == "" || str == "L()" || str == "S(L()L())" || str == "P(L()L()L())" )
	{
		return "";
	}

	DHTree dhtree( str );
	dhtree.prune_first_leaf();
	return dhtree.representation();
}

std::vector< std::string > DHEnumerator::children_candidates( const std::string &str ) const
{
	if ( count( std::begin( str ), std::end( str ), 'L' ) == N_ )
	{
		return {};
	}
	if ( str == "L()" )
	{
		return {};
	}

// 	std::cerr << "chidlren candidates" << std::endl;
// 	std::cerr << "str : " << str << std::endl;

	std::stack< char > stk;
	std::set< std::string > results;
	for ( size_t i = 0; i < str.length(); ++i )
	{
		if ( str[i] == '(' )
		{
			continue;
		}
		if ( str[i] == ')' )
		{
			stk.pop();
			continue;
		}

		assert( isalpha( str[i] ) );
		stk.push( str[i] );

		std::vector< std::string > ss;

		if ( i )
		{ // add sibling
			std::string s = str;
			s.insert( i, "L()" );
// 			std::cerr << "s : " << s << std::endl;
			ss.push_back( s );
		}
		if ( str[i] == 'L' )
		{
			for ( const char c : std::string( "SWP" ) )
			{
				const auto node = std::string( 1, c ) + "(L()L())";
				std::string s = str;
				s.replace( i, 3, node );
// 				std::cerr << "push : " << s << std::endl;
// 				std::cerr << str << " -> " << s << std::endl;
				ss.push_back( s );
			}
		}
		for ( const auto &s : ss )
		{
// 			std::cerr << "s : " << s << std::endl;
			const auto s2 = DHTree( s ).representation();
			if ( s2 != "" )
			{
// 				std::cerr << "insert : " << s2 << std::endl;
				results.insert( s2 );
			}
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
