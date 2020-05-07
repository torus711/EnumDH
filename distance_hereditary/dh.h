#include <bits/stdc++.h>

#include "./../common/abstruct_enumerator.h"

class DHEnumerator : public AbstructEnumerator< std::string >
{
public:
	DHEnumerator( const int n );

private:
	virtual std::string root( const int N ) const override;

	virtual std::string remove_edge( const std::string &intervals, const int u, const int v ) const override;
	virtual std::string parent( const std::string & ) const override;

	virtual bool recognition( const std::string &intervals ) const override;
	virtual bool isomorphic( const std::string &g1, const std::string &g2 ) const override;

	virtual void output( std::ostream &out, const std::string &intervals ) const override;
};
