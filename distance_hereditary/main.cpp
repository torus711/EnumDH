#include <bits/stdc++.h>

#include "./../common/cmdline.h"
#include "./dh.h"

#ifdef USE_MPI
#include "./../common/parallel.h"
int NUM_PROC;
int MY_RANK;
MPI_Status mpi_status;
#endif

int main( int argc, char *argv[] )
{
#ifdef USE_MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size( MPI_COMM_WORLD, &NUM_PROC );
	MPI_Comm_rank( MPI_COMM_WORLD, &MY_RANK );

	assert( 2 <= NUM_PROC );
#endif

	cmdline::parser optparser;
	optparser.add< int >( "size", 'n', "Upper bound of # of vertices to enumerate", true );

	if ( !optparser.parse( argc, argv ) )
	{
		std::cerr << optparser.error_full();
		std::cerr << optparser.usage();
		std::cerr << std::flush;
		return 1;
	}

	const int N = optparser.get< int >( "size" );

	DHEnumerator enumerator( N );
#ifdef USE_MPI
	if ( MY_RANK == 0 )
#endif
	{
		std::ostringstream oss;
		oss <<  "distance_hereditary_" << N << ".out";
		std::ofstream out( oss.str() );

		std::ofstream exec_time( "exec_times.csv", std::ofstream::app );
		const clock_t clock_start = clock();

		enumerator.enumerate( out );

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
