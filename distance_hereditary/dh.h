#include <bits/stdc++.h>

#include "./../common/abstruct_enumerator.h"

class DHEnumerator : public AbstructEnumerator< std::string >
{
	const int N_;
public:
	DHEnumerator( const int n );

private:
	virtual std::vector< std::string > roots() const override;
	virtual bool recognition( const std::string & ) const override;
	virtual bool isomorphic( const std::string &, const std::string & ) const override;

	virtual std::string parent( const std::string & ) const override;
	virtual std::vector< std::string > children_candidates( const std::string & ) const override;
	virtual void output( std::ostream &out, const std::string &intervals ) const override;
};
