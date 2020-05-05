#include <bits/stdc++.h>

class Enumerator
{
private:
	const int N_;
public:
	Enumerator( const int n ) : N_( n );
	void exec( std::ostream &out );

private:
	std::string root() const;
	std::string parent( const const string & ) const;
	std::vector< std::string > > children( const std::string & ) const;
};
