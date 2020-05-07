#pragma once

#if USE_MPI
#include <mpi.h>
extern int NUM_PROC;
extern int MY_RANK;
extern MPI_Status mpi_status;

enum MSG_TAGS
{
	COMMAND,
	GRAPH
};

enum SLAVE_STATUS
{
	READY,
	SUBMIT
};

enum COMMANDS
{
	JOB,
	QUIT
};
#endif
