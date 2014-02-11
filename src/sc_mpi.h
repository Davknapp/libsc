/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef SC_MPI_H
#define SC_MPI_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

typedef enum
{
  SC_TAG_AG_ALLTOALL = 's' + 'c',       /* anything really */
  SC_TAG_AG_RECURSIVE_A,
  SC_TAG_AG_RECURSIVE_B,
  SC_TAG_AG_RECURSIVE_C,
  SC_TAG_NOTIFY_RECURSIVE,
  SC_TAG_REDUCE,
  SC_TAG_PSORT_LO,
  SC_TAG_PSORT_HI
}
sc_tag_t;

#ifndef SC_MPI

#define MPI_SUCCESS 0
#define MPI_COMM_NULL           ((MPI_Comm) 0x04000000)
#define MPI_COMM_WORLD          ((MPI_Comm) 0x44000000)
#define MPI_COMM_SELF           ((MPI_Comm) 0x44000001)

#define MPI_THREAD_SINGLE       0
#define MPI_THREAD_FUNNELED     1
#define MPI_THREAD_SERIALIZED   2
#define MPI_THREAD_MULTIPLE     3

#define MPI_ANY_SOURCE          (-2)
#define MPI_ANY_TAG             (-1)
#define MPI_STATUS_IGNORE       (MPI_Status *) 1
#define MPI_STATUSES_IGNORE     (MPI_Status *) 1

#define MPI_REQUEST_NULL        ((MPI_Request) 0x2c000000)

#define MPI_CHAR                ((MPI_Datatype) 0x4c000101)
#define MPI_SIGNED_CHAR         ((MPI_Datatype) 0x4c000118)
#define MPI_UNSIGNED_CHAR       ((MPI_Datatype) 0x4c000102)
#define MPI_BYTE                ((MPI_Datatype) 0x4c00010d)
#define MPI_SHORT               ((MPI_Datatype) 0x4c000203)
#define MPI_UNSIGNED_SHORT      ((MPI_Datatype) 0x4c000204)
#define MPI_INT                 ((MPI_Datatype) 0x4c000405)
#define MPI_UNSIGNED            ((MPI_Datatype) 0x4c000406)
#define MPI_LONG                ((MPI_Datatype) 0x4c000407)
#define MPI_UNSIGNED_LONG       ((MPI_Datatype) 0x4c000408)
#define MPI_LONG_LONG_INT       ((MPI_Datatype) 0x4c000809)
#define MPI_FLOAT               ((MPI_Datatype) 0x4c00040a)
#define MPI_DOUBLE              ((MPI_Datatype) 0x4c00080b)
#define MPI_LONG_DOUBLE         ((MPI_Datatype) 0x4c000c0c)

#define MPI_MAX                 ((MPI_Op) 0x58000001)
#define MPI_MIN                 ((MPI_Op) 0x58000002)
#define MPI_SUM                 ((MPI_Op) 0x58000003)
#define MPI_PROD                ((MPI_Op) 0x58000004)
#define MPI_LAND                ((MPI_Op) 0x58000005)
#define MPI_BAND                ((MPI_Op) 0x58000006)
#define MPI_LOR                 ((MPI_Op) 0x58000007)
#define MPI_BOR                 ((MPI_Op) 0x58000008)
#define MPI_LXOR                ((MPI_Op) 0x58000009)
#define MPI_BXOR                ((MPI_Op) 0x5800000a)
#define MPI_MINLOC              ((MPI_Op) 0x5800000b)
#define MPI_MAXLOC              ((MPI_Op) 0x5800000c)
#define MPI_REPLACE             ((MPI_Op) 0x5800000d)

#define MPI_UNDEFINED           (-32766)

typedef int         MPI_Comm;
typedef int         MPI_Datatype;
typedef int         MPI_Op;
typedef int         MPI_Request;
typedef struct MPI_Status
{
  int                 count;
  int                 cancelled;
  int                 MPI_SOURCE;
  int                 MPI_TAG;
  int                 MPI_ERROR;
}
MPI_Status;

/* These functions are valid and functional for a single process. */

int                 MPI_Init (int *, char ***);
int                 MPI_Init_thread (int *argc, char ***argv,
                                     int required, int *provided);

int                 MPI_Finalize (void);
int                 MPI_Abort (MPI_Comm, int)
  __attribute__ ((noreturn));

int                 MPI_Comm_dup (MPI_Comm, MPI_Comm *);
int                 MPI_Comm_free (MPI_Comm *);
int                 MPI_Comm_size (MPI_Comm, int *);
int                 MPI_Comm_rank (MPI_Comm, int *);

int                 MPI_Barrier (MPI_Comm);
int                 MPI_Bcast (void *, int, MPI_Datatype, int, MPI_Comm);
int                 MPI_Gather (void *, int, MPI_Datatype,
                                void *, int, MPI_Datatype, int, MPI_Comm);
int                 MPI_Gatherv (void *, int, MPI_Datatype, void *,
                                 int *, int *, MPI_Datatype, int, MPI_Comm);
int                 MPI_Allgather (void *, int, MPI_Datatype,
                                   void *, int, MPI_Datatype, MPI_Comm);
int                 MPI_Allgatherv (void *, int, MPI_Datatype, void *,
                                    int *, int *, MPI_Datatype, MPI_Comm);
int                 MPI_Reduce (void *, void *, int, MPI_Datatype,
                                MPI_Op, int, MPI_Comm);
int                 MPI_Allreduce (void *, void *, int, MPI_Datatype,
                                   MPI_Op, MPI_Comm);

double              MPI_Wtime (void);

/* These functions will abort. */
int                 MPI_Recv (void *, int, MPI_Datatype, int, int, MPI_Comm,
                              MPI_Status *);
int                 MPI_Irecv (void *, int, MPI_Datatype, int, int, MPI_Comm,
                               MPI_Request *);
int                 MPI_Send (void *, int, MPI_Datatype, int, int, MPI_Comm);
int                 MPI_Isend (void *, int, MPI_Datatype, int, int, MPI_Comm,
                               MPI_Request *);
int                 MPI_Probe (int, int, MPI_Comm, MPI_Status *);
int                 MPI_Iprobe (int, int, MPI_Comm, int *, MPI_Status *);
int                 MPI_Get_count (MPI_Status *, MPI_Datatype, int *);

/* These functions are only allowed to be called with zero size arrays. */
int                 MPI_Wait (MPI_Request *, MPI_Status *);
int                 MPI_Waitsome (int, MPI_Request *,
                                  int *, int *, MPI_Status *);
int                 MPI_Waitall (int, MPI_Request *, MPI_Status *);

#endif /* !SC_MPI */

/** Return the size of MPI data types.
 * \param [in] t    MPI data type.
 * \return          Returns the size in bytes.
 */
size_t              sc_mpi_sizeof (MPI_Datatype t);

SC_EXTERN_C_END;

#endif /* !SC_MPI_H */
