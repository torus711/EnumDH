#include "./dhtree.h"

DHTree::DHTree( const std::string &str ) : N_( count( begin( str ), end( str ), 'L' ) ),
	root_( parse_node( std::cbegin( str ), std::cend( str ) ) )
{
	normalize();
	verification();

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

	int size = 1, depth = 0;
	for ( auto &child : children )
	{
		size += child->size_;
		depth = std::max( depth, child->depth_ );
	}

	const auto p = std::make_shared< Node >( type, size, depth + 1 );
	p->children_ = children;
	return p;
}

DHTree& DHTree::normalize()
{
	root_->normalize();
	return *this;
};

void DHTree::verification()
{
	root_->verification();
	root_verification();
	return;
}

void DHTree::root_verification()
{
	// Lemma 15, (b) : root is cneter of the tree
	{
		const int d1 = ( *std::rbegin( root_->children_ ) )->depth_;
		const int d2 = ( *( std::rbegin( root_->children_ ) + 1 ) )->depth_;
// 	std::cerr << root_->valid_ << std::endl;
		root_->valid_ &= abs( d1 - d2 ) <= 1;
// 	std::cerr << root_->valid_ << std::endl;
	}

	// Lemma 15, (d)
	root_->valid_ &= root_->type_ == 'S' || root_->type_ == 'P';
	root_->valid_ &= !( root_->type_ == 'S' && root_->size_ <= 2 );
	root_->valid_ &= !( root_->type_ == 'P' && root_->size_ <= 1 );

// 	if ( root_->type_ == 'P' )
// 	{
// 		root_->valid_ &= 3 <= root_->children_.size();
// 	}

	if ( !root_->valid_ )
	{
		root_->representation_ = "";
		return;
	}

	// Lemma 15 (c) : when root has two children, root is S and children have the same label
	if ( root_->children_.size() == 2 )
	{
		root_->valid_ &= root_->type_ == 'S';
		const int t1 = root_->children_[0]->type_;
		const int t2 = root_->children_[1]->type_;
		root_->valid_ &= t1 == t2;
	}

	// Lemma 15 (d) : when root is P, at least two non-neck child have maximal depth
	if ( root_->type_ == 'P' )
	{
		const int dn = ( *std::begin( root_->children_ ) )->depth_;
		const int d1 = ( *std::rbegin( root_->children_ ) )->depth_;
		const int d2 = ( *( std::rbegin( root_->children_ ) + 1 ) )->depth_;
		root_->valid_ &= dn <= d1 && d1 == d2;
	}

// 	if ( root_->children_.size() == 2 )
// 	{
// 		if ( root_->type_ != 'S' || root_->children_[0]->type_ != root_->children_[1]->type_ )
// 		{
// 			root_->representation_ = "";
// 		}
// 	}
//
// 	if ( root_->type_ == 'P' )
// 	{
// 		assert( ( *std::rbegin( root_->children_ ) )->depth_ == ( *( std::rbegin( root_->children_ ) + 1 ) )->depth_ );
// 	}
//
// 	assert( root_->type_ != 'L' );

// 	std::cerr << "original   : " << str << std::endl;
// 	std::cerr << "normalized : " << root_->representation_ << std::endl << std::endl;

	if ( !root_->valid_ )
	{
		root_->representation_ = "";
	}

	return;
}

void DHTree::prune_first_leaf()
{
	if ( !root_->prune() )
	{
		root_->representation_ = "";
	}
	normalize();
	return;
}

std::string DHTree::representation() const
{
	return root_->representation_;
}

DHTree::Node::Node( const char t, const int s, const int d ) :
	type_( t ), size_( s ), depth_( d )
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
	std::sort(  std::begin( children_ ) + ( type_ == 'P' ), std::end( children_ ),
			[]( const auto &p, const auto &q ){ return *p < *q; } );

	representation_ = "";
	representation_ += type_;
	representation_ += '(';
	std::for_each( std::begin( children_ ), std::end( children_ ),
			[&]( const auto &child ){ representation_ += child->representation_; } );
	representation_ += ')';

	return;
}

void DHTree::Node::verification()
{
	std::for_each( std::begin( children_ ), std::end( children_ ),
			[&]( const auto &p ){ p->verification(); valid_ &= p->valid_; } );

	// Lemma 10, (e)
	if ( type_ == 'P' )
	{
		std::for_each( std::begin( children_ ), std::end( children_ ),
				[&]( const auto &p ){ valid_ &= p->type_ != 'W'; } );
	}

	// Lemma 15, (a)
	if ( type_ != 'P' )
	{
		std::for_each( std::begin( children_ ), std::end( children_ ),
				[&]( const auto &p ){ valid_ &= type_ != p->type_; } );
	}

	// Lemma 10, (b)
	if ( type_ != 'L' )
	{
		valid_ &= 2 <= children_.size();
	}

	return;
}

bool DHTree::Node::prune()
{
	for ( const auto &p : children_ )
	{
		if ( p->prune() )
		{
			return true;
		}
	}

	for ( auto it = std::begin( children_ ) + ( type_ == 'P' ); it != std::end( children_ ); ++it )
	{
		if ( ( *it )->type_ != 'L' )
		{
			continue;
		}

		it = children_.erase( it );
		if ( children_.size() == 1 )
		{
			children_.clear();
			type_ = 'L';
		}
		return true;
	}

	return false;
}

bool DHTree::Node::operator<( const Node &rhs ) const
{
	if ( depth_ != rhs.depth_ )
	{
		return depth_ < rhs.depth_;
	}
	return representation_ < rhs.representation_;
}

