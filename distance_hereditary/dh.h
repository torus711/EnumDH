#include <bits/stdc++.h>

#include "./../common/abstruct_enumerator.h"

class DHEnumerator : public AbstructEnumerator< std::string >
{
	const bool connected;

public:
	DHEnumerator( const int n, const bool c );

private:
	virtual VI root( const int N ) const override;

	virtual VI remove_edge( const VI &intervals, const int u, const int v ) const override;
	virtual VI parent( const VI & ) const override;

	virtual bool recognition( const VI &intervals ) const override;
	virtual bool isomorphic( const VI &g1, const VI &g2 ) const override;

	virtual void output( std::ostream &out, const VI &intervals ) const override;
};
