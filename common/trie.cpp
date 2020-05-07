#include "./trie.h"

Trie::Trie( const int w ) : W( w ), root( std::make_shared< Node >( W ) )
{
	return;
}

void Trie::add( const std::vector< int > &v )
{
	std::shared_ptr< Node > p = root;
	
	for ( const int c : v )
	{
		p = eval_node( p->children[c] );
	}

	++p->count;
	assert( p->count );

	return;
}

bool Trie::contains( const std::vector< int > &v )
{
	std::shared_ptr< Node > p = root;
	
	for ( const int c : v )
	{
		p = eval_node( p->children[c] );
	}

	return p->count;
}

std::shared_ptr< Trie::Node > Trie::eval_node( std::shared_ptr< Node > &p )
{
	if ( !p )
	{
		p = std::make_shared< Node >( W );
	}
	return p;
}

Trie::Node::Node( const int w ) : children( w )
{
	return;
}
