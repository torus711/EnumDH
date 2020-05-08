#include "./dh.h"
#include "./dhtree.h"

DHEnumerator::DHEnumerator( const int n ) : AbstructEnumerator( n ), N_( n )
{
	return;
}

std::vector< std::string > DHEnumerator::roots() const
{
	return { "S(L()L())", "P(L()L()L())" };
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
	std::string s = str;
	std::stack< char > stk;

	for ( size_t i = 0; i < str.length(); ++i )
	{
		if ( isalpha( str[i] ) )
		{
			stk.push( str[i] );
			if ( str[i] != 'L' )
			{
				continue;
			}
		}
		if ( str[i] == '(' )
		{
			continue;
		}
		if ( str[i] == ')' )
		{
			stk.pop();
			continue;
		}

		assert( str[i] == 'L' );

		const char parent_type = stk.top();
		if ( parent_type == 'P' )
		{
			// P(L(
			if ( str[ i - 2 ] == 'P' )
			{
				continue;
			}

			s = s.erase( i, 3 );

			i -= 2;
			// P(L())
			if ( s.substr( i, 6 ) == "P(L())" )
			{
				s = s.erase( i + 2, 3 );
				s[i] = 'L';
			}
			return DHTree( s ).representation();
		}
		else
		{
			// *(L())
			s = s.erase( i, 3 );
			--i;
			if ( s.substr( i, 2 ) == "()" )
			{
				--i;
				s[i] = 'L';
			}
			return DHTree( s ).representation();
		}
	}

	return "";
}

std::vector< std::string > DHEnumerator::children_candidates( const std::string &str ) const
{
	if ( count( std::begin( str ), std::end( str ), 'L' ) == N_ )
	{
		return {};
	}

	std::stack< char > stk;
	std::set< std::string > results;
	for ( size_t i = 0; i < str.length(); ++i )
	{
		if ( isalpha( str[i] ) )
		{
			stk.push( str[i] );
			if ( str[i] != 'L' )
			{
				continue;
			}
		}
		if ( str[i] == '(' )
		{
			continue;
		}
		if ( str[i] == ')' )
		{
			stk.pop();

			continue;
		}

		assert( str[i] == 'L' );

		const char parent_type = stk.top();
		{ // add sibling
			std::string s = str;
			s.insert( i, "L()" );
			results.insert( DHTree( s ).representation() );
		}
		if ( parent_type != 'S' )
		{ // convert to s
			std::string s = str;
			s.replace( i, 3, "S(L()L())" );
			results.insert( DHTree( s ).representation() );
		}
		if ( parent_type != 'W' || ( parent_type == 'P' && str[ i - 2 ] == 'P' ) )
		{ // convert to w
			std::string s = str;
			s.replace( i, 3, "W(L()L())" );
			results.insert( DHTree( s ).representation() );
		}
		{ // convert to p
			std::string s = str;
			s.replace( i, 3, "P(L()L())" );
			results.insert( DHTree( s ).representation() );
		}
	}

	return { std::begin( results ), std::end( results ) };
}

void DHEnumerator::output( std::ostream &out, const std::string &str ) const
{
	const int n = std::count( std::begin( str ), std::end( str ), 'L' );
	std::cerr << n << " : " << str << std::endl;
	out << n << " : " << str << std::endl;
}
