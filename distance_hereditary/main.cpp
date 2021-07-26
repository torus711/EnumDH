#include <bits/stdc++.h>

#include "./../common/cmdline.h"
#include "./dh.h"

#ifdef USE_MPI
#include "./../common/parallel.h"
int NUM_PROC;
int MY_RANK;
MPI_Status mpi_status;
#endif

#define DUMP( x ) std::cerr << #x << " = " << ( x ) << std::endl;
#include "dhtree.h"
void test()
{
	{
		const std::string s = "P(S(L()L())L()L())";
		DUMP( s );
		DHTree dhtree( s );
		DUMP( dhtree.representation() );
		dhtree.prune_first_leaf();
		DUMP( dhtree.representation() );
	}


	return;
}
#undef DUMP

std::string filename( const bool, const bool, const bool );

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
	const bool pendant = optparser.exist( "pendant" );
	const bool wtwin = optparser.exist( "weak_twin" );
	const bool stwin = optparser.exist( "strong_twin" );

	DHEnumerator enumerator( N, false, pendant, wtwin, stwin );
#ifdef USE_MPI
	if ( MY_RANK == 0 )
#endif
	{
		std::ostringstream oss;
		oss << filename( pendant, wtwin, stwin ) << '_' << N;
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

std::string filename( const bool pendant, const bool wtwin, const bool stwin )
{
	int flags = 0;
	flags |= pendant;
	flags |= 2 * wtwin;
	flags |= 4 * stwin;

	switch ( flags )
	{
	case 3:
		return "bipartite_distance_hereditary";
	case 7:
		return "distance_hereditary";
	}

	std::string result;
	if ( pendant )
	{
		result += "p_";
	}
	if ( wtwin )
	{
		result += "w_";
	}
	if ( stwin )
	{
		result += "s_";
	}
	return result;
}
