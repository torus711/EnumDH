#include "./dhtree.h"

DHTree::DHTree( const std::string &str ) : N_( count( begin( str ), end( str ), 'L' ) ),
	root_( parse_node( std::cbegin( str ), std::cend( str ) ) )
{
	normalize();
// 	verification();

// 	std::cout << "from string : " << root_->representation_ << std::endl;

	return;
}

DHTree::DHTree( const std::vector< std::vector< int > > &G ) : N_( G.size() ), root_( construct_tree( G ) )
{
	normalize();
// 	verification();

// 	std::cout << "from graph : " << root_->representation_ << std::endl;

	return;
}

std::shared_ptr< DHTree::Node > DHTree::construct_tree( std::vector< std::vector< int > > G )
{
	int N = G.size();
	for_each( std::begin( G ), std::end( G ),
			[]( auto &row ){ std::sort( std::begin( row ), std::end( row ) ); } );

	std::vector< std::shared_ptr< Node > > nodes;
	generate_n( std::back_inserter( nodes ), N,
				[&]{ return std::make_shared< Node >( 'L' ); } );

	std::vector< bool > removed( N );

	while ( true )
	{
		if ( N <= 1 )
		{
			break;
		}

		std::map< std::vector< int >, std::vector< int > > open_neighbor, close_neighbor, pendants;

		for ( int u = 0; u < G.size(); ++u )
		{
			if ( G[u].empty() ) // shrinken vertex
			{
				continue;
			}

			open_neighbor[ G[u] ].push_back( u );

			auto row = G[u];
			row.insert( lower_bound( std::begin( row ), std::end( row ), u ), u );
			assert( is_sorted( std::begin( row ), std::end( row ) ) );
			close_neighbor[ row ].push_back( u );

			if ( G[u].size() == 1 )
			{
				pendants[ G[u] ].push_back( u );
			}
		}

		if ( close_neighbor.size() == 1 ) // it is clique
		{
			auto parent = std::make_shared< Node >( 'S' );
			const auto &row = std::begin( close_neighbor )->second;

			for ( int u : row )
			{
				parent->children_.push_back( std::move( nodes[u] ) );
			}

			for ( int u : row )
			{
				nodes[u] = parent;
			}

			for_each( std::begin( row ) + 1, std::end( row ),
					[&]( const int u ){ assert( !removed[u] ); removed[u] = true; } );

			break;
		}
		if ( open_neighbor.size() == 2 && pendants.size() == 1 ) // it is star
		{
			const int u = std::begin( pendants )->first[0];
			auto row = std::begin( pendants )->second;
			row.insert( std::begin( row ), u );

			auto parent = std::make_shared< Node >( 'P' );
			for ( int v : row )
			{
				parent->children_.push_back( std::move( nodes[v] ) );
			}

			for ( int v : row )
			{
				nodes[v] = parent;
			}

			for_each( std::begin( row ) + 1, std::end( row ),
					[&]( const int u ){ assert( !removed[u] ); removed[u] = true; } );

			break;
		}

		// shrink strong/weak twing and pendants
		int type = 0;
		for ( const auto neighbors : { close_neighbor, open_neighbor, pendants } )
		{
			for ( auto p : neighbors )
			{
				if ( type == 1 && p.first.size() == 1 ) // it is pendant, not weak twin
				{
					continue;
				}

				auto row = p.second;
				if ( type <= 1 && row.size() == 1 ) // there are no peelable operation
				{
					continue;
				}

				if ( type == 2 ) // it is pendant
				{
					const auto neck = p.first[0];
					row.insert( std::begin( row ), neck );
				}

				auto parent = std::make_shared< Node >( "SWP"[ type ] );
				for ( const int u : row )
				{
					parent->children_.push_back( std::move( nodes[u] ) );
				}

				for ( const int u : row )
				{
					nodes[u] = parent;
				}

				for_each( std::begin( row ) + 1, std::end( row ),
						[&]( const int u ){ assert( !removed[u] ); removed[u] = true; } );
			}
			++type;
		}

		// update graph
		std::vector< std::vector< int > > Gn( G.size() );
		for ( int u = 0; u < G.size(); ++u )
		{
			if ( removed[u] )
			{
				continue;
			}
			std::copy_if( std::begin( G[u] ), std::end( G[u] ), std::back_inserter( Gn[u] ),
					[&]( const int v ){ return !removed[v]; } );
		}
		
		G = Gn;
		N = std::count( std::begin( removed ), std::end( removed ), false );
	}
	assert( std::count( std::begin( removed ), std::end( removed ), false ) == 1 );
	const int u = std::find( std::begin( removed ), std::end( removed ), false ) - std::begin( removed );
	return nodes[u];
}

std::vector< std::vector< int > > DHTree::get_graph() const
{
	std::vector< std::vector< int > > G( 1 );
	root_->construct_graph( G, 0 );
	return G;
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

	const auto p = std::make_shared< Node >( type );
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
		root_->valid_ &= abs( d1 - d2 ) <= 1;
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
		assert( false );
	}
	normalize();

// 	std::cout << "from pruning : " << root_->representation_ << std::endl;
	return;
}

std::string DHTree::representation() const
{
	return root_->representation_;
}

DHTree::Node::Node( const char t = 'L', const int s = 1, const int d = 0 ) :
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

	if ( type_ == 'P' || type_ == 'L' )
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

	size_ = 1;
	depth_ = 0;
	for ( const auto p : children_ )
	{
		size_ += p->size_;
		depth_ = std::max( depth_, p->depth_ );
	}

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
	std::cerr << "p: " << use_pendant << std::endl;
	std::cerr << "w: " << use_wtwin << std::endl;
	std::cerr << "s: " << use_stwin << std::endl;
	if ( ( !use_pendant &&  type_ == 'P' ) ||
			( !use_wtwin && type_ == 'W' ) ||
			( !use_stwin && type_ == 'S' ) )
	{
// 		return false;
	}

	for ( const auto &p : children_ )
	{

		if ( p->prune() )
		{
			return true;
		}
	}

	if  ( children_.size() < 2 )
	{
		assert( type_ == 'L' );
		return false;
	}

	if ( any_of( std::begin( children_ ), std::end( children_ ),
				[]( const auto &p ){ return p->type_ != 'L'; } ) )
	{
		return false;
	}

	children_.pop_back();
	if ( children_.size() == 1 )
	{
		type_ = 'L';
		children_.clear();
	}

	return true;
}

bool DHTree::Node::operator<( const Node &rhs ) const
{
	if ( depth_ != rhs.depth_ )
	{
		return depth_ < rhs.depth_;
	}
	return representation_ < rhs.representation_;
}

void DHTree::Node::construct_graph( std::vector< std::vector< int > > &G, int u ) const
{
	if ( type_ == 'L' )
	{
		return;
	}

	std::vector< int > vertices( 1, u );
	for ( size_t i = 0; i < children_.size() - 1; ++i )
	{
		vertices.push_back( G.size() );
		G.emplace_back();
	}

	if ( type_ == 'P' )
	{
		std::copy( std::begin( vertices ) + 1, std::end( vertices ), std::back_inserter( G[u] ) );
		std::for_each( std::begin( vertices ) + 1, std::end( vertices ),
				[&]( const int v ){ G[v].push_back( u ); } );
	}
	else
	{
		std::for_each( std::begin( vertices ) + 1, std::end( vertices ),
				[&]( const int v ){ G[v] = G[u]; } );
		for ( const int w : G[u] )
		{
			std::for_each( std::begin( vertices ) + 1, std::end( vertices ),
					[&]( const int v ){ G[w].push_back( v ); } );
		}

		if ( type_ == 'S' )
		{
			for ( auto i : vertices )
			{
				for ( auto j : vertices )
				{
					if ( i != j )
					{
						G[i].push_back( j );
					}
				}
			}
		}
	}

	for ( size_t i = 0; i < vertices.size(); ++i )
	{
		children_[i]->construct_graph( G, vertices[i] );
	}

	return;
}
