#include <bits/stdc++.h>

#include "./../common/cmdline.h"
#include "./dh.h"

bool use_pendant = false;
bool use_wtwin = false;
bool use_stwin = false;

#ifdef USE_MPI

int NUM_PROC;
int MY_RANK;
MPI_Status mpi_status;
#endif

#define DUMP( x ) std::cerr << #x << " = " << ( x ) << std::endl;
#include "dhtree.h"
// #include "dh.h"
void test()
{
	{
		const std::string s = "P(L()L()L())";
		DUMP( s );
		DHTree dhtree( s );
		DUMP( dhtree.representation() );
		DHEnumerator enumerator( 4, false );
		std::cerr << "original:" << std::endl;
		enumerator.output( std::cerr, s );
		const auto cs = enumerator.children_candidates( s );
		for ( const auto c : cs )
		{
			std::cerr << "candidate:" << std::endl;
			enumerator.output( std::cerr, c );
			std::cerr << std::endl;
			std::cerr << "parent:" << std::endl;
			enumerator.output( std::cerr, enumerator.parent( c ) );
			std::cerr << std::endl;
		}
	}


	return;
}
#undef DUMP

std::string filename();

int main( int argc, char *argv[] )
{
#ifdef USE_MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size( MPI_COMM_WORLD, &NUM_PROC );
	MPI_Comm_rank( MPI_COMM_WORLD, &MY_RANK );

	assert( 2 <= NUM_PROC );
#endif


	cmdline::parser optparser;
	optparser.add< int >( "size", 'n', "Upper bound of # of vertices to enumerate. It should be greater or equal to 3", true );
	optparser.add( "pendant", 'p', "Use adding pendant operation" );
	optparser.add( "weak_twin", 'w', "Use splitting weak twin operation" );
	optparser.add( "strong_twin", 's', "Use splitting strong twin operation" );

	if ( !optparser.parse( argc, argv ) )
	{
		std::cerr << optparser.error_full();
		std::cerr << optparser.usage();
		std::cerr << std::flush;
		std::cerr << "Call test function..." << std::endl;
		test();
		return 1;
	}

	const int N = optparser.get< int >( "size" );
	use_pendant |= optparser.exist( "pendant" );
	use_wtwin |= optparser.exist( "weak_twin" );
	use_stwin |= optparser.exist( "strong_twin" );

	DHEnumerator enumerator( N, false );
#ifdef USE_MPI
	if ( MY_RANK == 0 )
#endif
	{
		std::ostringstream oss;
		oss << filename() << '_' << N << ".out";
		std::ofstream out( oss.str() );

		std::ofstream exec_time( "exec_times.csv", std::ofstream::app );
		const clock_t clock_start = clock();

		enumerator.exec( out );

		exec_time << N << '\t';
		exec_time << std::setprecision( 2 ) << std::fixed << ( ( 1. * clock() - clock_start ) / CLOCKS_PER_SEC ) << std::endl;
	}
#ifdef USE_MPI
	else
	{
		enumerator.slave();
	}
#endif

#ifdef USE_MPI
	MPI_Finalize();  
#endif

	return 0;
}

std::string filename()
{
	int flags = 0;
	flags |= use_pendant;
	flags |= 2 * use_wtwin;
	flags |= 4 * use_stwin;

	switch ( flags )
	{
	case 3:
		return "bipartite_distance_hereditary";
	case 7:
		return "distance_hereditary";
	}

	std::string result;
	if ( use_pendant )
	{
		result += "p_";
	}
	if ( use_wtwin )
	{
		result += "w_";
	}
	if ( use_stwin )
	{
		result += "s_";
	}
	return result;
}
