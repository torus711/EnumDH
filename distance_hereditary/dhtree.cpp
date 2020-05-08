#include "./dhtree.h"

DHTree::DHTree( const std::string &str ) : N_( count( begin( str ), end( str ), 'L' ) ),
	root_( parse_node( std::cbegin( str ), std::cend( str ) ) )
{
	normalize();
	std::cerr << "original   : " << str << std::endl;
	std::cerr << "normalized : " << root_->representation_ << std::endl << std::endl;
	return;
}

template < typename Iterator >
std::shared_ptr< DHTree::Node > DHTree::parse_node( Iterator &&first, Iterator &&last ) const
{
	assert( isalpha( *first ) );

	char type = *first++;
	++first; // ignore '('
	std::vector< std::shared_ptr< DHTree::Node > > children;
	while ( isalpha( *first ) )
	{
		children.push_back( parse_node( first, last ) );
	}
	++first; // ignore ')'

	if ( children.size() == 1 )
	{
		type = children[0]->type_;
		children = children[0]->children_;
	}

	int depth = 0;
	for ( auto &child : children )
	{
		depth = std::max( depth, child->depth_ );
	}

	const auto p = std::make_shared< Node >( type, depth );
	p->children_ = children;
	return p;
}

DHTree& DHTree::normalize()
{
	root_->normalize();
	return *this;
};

std::string DHTree::representation() const
{
	return root_->representation_;
}

DHTree::Node::Node( const char t, const int d ) : type_( t ), depth_( d )
{
	return;
}

void DHTree::Node::normalize()
{
	contraction();
	reordering();

	return;
}

void DHTree::Node::contraction()
{
	std::for_each( std::begin( children_ ), std::end( children_ ),
			[]( const auto &p ){ p->contraction(); } );

	if ( type_ == 'P' )
	{
		return;
	}

	std::vector< std::shared_ptr< DHTree::Node > > will_be_moved;
	for ( auto child = std::begin( children_ ); child != std::end( children_ ); ++child )
	{
		if ( ( *child )->type_ != type_ )
		{
			continue;
		}
		
		std::copy( std::begin( ( *child )->children_ ), std::end( ( *child )->children_ ),
				std::back_inserter( will_be_moved ) );
		child = --children_.erase( child );
	}
	std::copy( std::begin( will_be_moved ), std::end( will_be_moved ),
			std::back_inserter( children_ ) );

	return;
}

void DHTree::Node::reordering()
{
	std::for_each( std::begin( children_ ), std::end( children_ ),
			[]( const auto &p ){ p->reordering(); } );
	std::sort( std::begin( children_ ), std::end( children_ ),
			[]( const auto &p, const auto &q ){ return *p < *q; } );

	representation_ = "";
	representation_ += type_;
	representation_ += '(';
	std::for_each( std::begin( children_ ), std::end( children_ ),
			[&]( const auto &child ){ representation_ += child->representation_; } );
	representation_ += ')';

	return;
}

bool DHTree::Node::operator<( const Node &rhs ) const
{
	if ( depth_ != rhs.depth_ )
	{
		return depth_ < rhs.depth_;
	}
	return representation_ < rhs.representation_;
}

