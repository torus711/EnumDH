#include <bits/stdc++.h>

#ifdef USE_MPI
#include "./parallel.h"
#endif

template < typename Graph >
class AbstructEnumerator
{
protected:
	int N;

public:
	AbstructEnumerator( const int n );

	void enumerate( std::ostream & ) const;

private:
	std::vector< Graph > children_candidates( const Graph & ) const;
	std::vector< Graph > children( const Graph & ) const;

	virtual Graph root( const int N ) const = 0;

// 	virtual bool exist_edge( const Graph &, const int, const int ) const = 0;
	virtual Graph remove_edge( const Graph &, const int u, const int v ) const = 0;
	virtual Graph parent( const Graph & ) const = 0;

	virtual bool recognition( const Graph & ) const = 0;
	virtual bool isomorphic( const Graph &, const Graph & ) const = 0;

	virtual void output( std::ostream &out, const Graph & ) const = 0;

#ifdef USE_MPI
public:
	void slave() const;
private:
	void send_command( const int target, const int command ) const;
	int receive_command( const int target ) const;
	void send_graph( const int target, const Graph &G ) const;
	Graph receive_graph( const int target ) const;
#endif
};

template < typename Graph >
AbstructEnumerator< Graph >::AbstructEnumerator( const int n ) : N( n )
{
	return;
}

template < typename Graph >
void AbstructEnumerator< Graph >::enumerate( std::ostream &out ) const
{
	std::queue< Graph > que;
	que.push( root( N ) );

#ifdef USE_MPI
	std::queue< int > free_processes;

	for ( int processing = NUM_PROC - 1; !que.empty() || processing; )
	{
		int slave_status;
		MPI_Recv( &slave_status, 1, MPI_INTEGER, MPI_ANY_SOURCE, MSG_TAGS::COMMAND, MPI_COMM_WORLD, &mpi_status );
		const int target = mpi_status.MPI_SOURCE;

		switch ( slave_status )
		{
			case SLAVE_STATUS::READY:
			{
				free_processes.push( target );
				--processing;
				break;
			}
			case SLAVE_STATUS::SUBMIT:
			{
				const Graph G = receive_graph( target );
				que.push( G );
				break;
			}
		}

		if ( !que.empty() && !free_processes.empty() )
		{
			const Graph G = que.front();
			que.pop();
			output( out, G );

			const int p = free_processes.front();
			free_processes.pop();

			send_command( p, COMMANDS::JOB );
			send_graph( p, G );
			++processing;
		}
	}

	while ( !free_processes.empty() )
	{
		const int p = free_processes.front();
		free_processes.pop();
		send_command( p, COMMANDS::QUIT );
	}
#else
	long long number = 0;
	while ( !que.empty() )
	{
		const Graph G = que.front();
		que.pop();

		output( out, G );
		++number;

		const std::vector< Graph > CH = children( G );
		for_each( std::begin( CH ), std::end( CH ),
				[&]( const Graph &c ){ que.push( c ); } );
	}

	std::cout << "# of size " << N << " : " << number << std::endl;
#endif

	return;
}

template < typename Graph >
std::vector< Graph > AbstructEnumerator< Graph >::children_candidates( const Graph &G ) const
{
	std::set< Graph > result;

	for ( int i = 0; i < N; ++i )
	{
		for ( int j = 0; j < N; ++j )
		{
			if ( i == j )
			{
				continue;
			}

			const Graph c = remove_edge( G, i, j );
			if ( recognition( c ) )
			{
				result.insert( c );
			}
		}
	}

	return { std::begin( result ), std::end( result ) };
}

template < typename Graph >
std::vector< Graph > AbstructEnumerator< Graph >::children( const Graph &G ) const
{
	const auto &CH = children_candidates( G );

	std::vector< Graph > results;
	for ( const Graph &c : CH )
	{
		// 			std::cout << "attempt : ";
		// 			output( c );
		// 			std::cout << "parent  : ";
		// 			output( parent( c ) );
		// 			std::cout << std::endl;
		// 			output( c );
		// 			output( MPQ_Tree( c ).normalize().relabel_nodes().canonical_form() );
		// 			assert( c == MPQ_Tree( c ).normalize().relabel_nodes().canonical_form() );
		// 			assert( c == MPQ_Tree( c ).normalize().normalize().relabel_nodes().canonical_form() );
		if ( isomorphic( G, parent( c ) ) )
		{
			// 				std::cout << "OK" << std::endl;
			results.push_back( c );
		}
	}

	return std::move( results );
}


#ifdef USE_MPI
template < typename Graph >
void AbstructEnumerator< Graph >::slave() const
{
	send_command( 0, SLAVE_STATUS::READY );

	while ( receive_command( 0 ) == COMMANDS::JOB )
	{
		const Graph G = receive_graph( 0 );
		const auto CH = children( G );

		for ( const Graph &g : CH )
		{
			send_command( 0, SLAVE_STATUS::SUBMIT );
			send_graph( 0, g );
		}

		send_command( 0, SLAVE_STATUS::READY );
	}

	return;
}

template < typename Graph >
void AbstructEnumerator< Graph >::send_command( const int target, const int i ) const
{
	MPI_Send( &i, 1, MPI_INTEGER, target, MSG_TAGS::COMMAND, MPI_COMM_WORLD );
	return;
}

template < typename Graph >
int AbstructEnumerator< Graph >::receive_command( const int target ) const
{
	int result;
	MPI_Recv( &result, 1, MPI_INTEGER, target, MSG_TAGS::COMMAND, MPI_COMM_WORLD, &mpi_status );
	return result;
}

template < typename Graph >
void AbstructEnumerator< Graph >::send_graph( const int target, const Graph &G ) const
{
	MPI_Send( G.data(), 2 * N, MPI_INTEGER, target, MSG_TAGS::GRAPH, MPI_COMM_WORLD );
	return;
}

template < typename Graph >
Graph AbstructEnumerator< Graph >::receive_graph( const int target ) const
{
	const auto buffer = std::make_unique< int[] >( 2 * N );
	MPI_Recv( buffer.get(), 2 * N, MPI_INTEGER, target, MSG_TAGS::GRAPH, MPI_COMM_WORLD, &mpi_status );
	return Graph( buffer.get(), buffer.get() + 2 * N );
}

#endif
