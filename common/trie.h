#pragma once

#include <bits/stdc++.h>

class Trie
{
private:
	struct Node;

	const int W;
	const std::shared_ptr< Node > root;

public:
	Trie( const int w );

	void add( const std::vector< int > &v );
	bool contains( const std::vector< int > &v );

private:
	std::shared_ptr< Node > eval_node( std::shared_ptr< Node > &p );

};

struct Trie::Node
{
	int count = 0;
	std::vector< std::shared_ptr< Node > > children;

	Node( const int w );
};
