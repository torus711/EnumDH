#include <bits/stdc++.h>

class DHTree
{
	struct Node
	{
		char type_;
		int size_;
		const int depth_;
		bool valid_ = true;
		std::vector< std::shared_ptr< Node > > children_;
		std::string representation_;

		Node( const char, const int, const int );
		void normalize();
		void contraction();
		void reordering();
		void verification();
		bool prune();
		bool operator<( const Node & ) const;
		void construct_graph( std::vector< std::vector< int > > &, int ) const;

	};
	const int N_;
	const std::shared_ptr< Node > root_;

public:
	DHTree( const std::string & );
	DHTree( const std::vector< std::vector< int > > & );

	DHTree& normalize();
	void verification();
	void root_verification();
	void prune_first_leaf();
	std::string representation() const;

	std::shared_ptr< Node > construct_tree( std::vector< std::vector< int > > G );
	std::vector< std::vector< int > > get_graph() const;

private:
	DHTree contraction() const;
	DHTree reordering() const;
	template < typename Iterator >
	std::shared_ptr< Node > parse_node( Iterator &&, Iterator && ) const;
};
