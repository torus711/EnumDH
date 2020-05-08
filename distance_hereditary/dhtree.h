#include <bits/stdc++.h>

class DHTree
{
	struct Node
	{
		const char type_;
		const int depth_;
		std::vector< std::shared_ptr< Node > > children_;
		std::string representation_;

		Node( const char, const int );
		void normalize();
		void contraction();
		void reordering();
		bool operator<( const Node & ) const;
	};

	const int N_;
	const std::shared_ptr< Node > root_;

public:
	DHTree( const std::string & );
	DHTree& normalize();
	std::string representation() const;

private:
	DHTree contraction() const;
	DHTree reordering() const;
	template < typename Iterator >
	std::shared_ptr< Node > parse_node( Iterator &&, Iterator && ) const;
};
