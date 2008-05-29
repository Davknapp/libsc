/*
  This file is part of the SC Library.
  The SC library provides support for parallel scientific applications.

  Copyright (C) 2008 Carsten Burstedde, Lucas Wilcox.

  The SC Library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the SC Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SC_MPI_DUMMY_H
#define SC_MPI_DUMMY_H

#ifndef SC_H
#error "sc.h should be included before this header file"
#endif

#define MPI_SUCCESS 0
#define MPI_COMM_NULL           ((MPI_Comm) 0x04000000)
#define MPI_COMM_WORLD          ((MPI_Comm) 0x44000000)
#define MPI_COMM_SELF           ((MPI_Comm) 0x44000001)

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
#define MPI_FLOAT               ((MPI_Datatype) 0x4c00040a)
#define MPI_DOUBLE              ((MPI_Datatype) 0x4c00080b)
#define MPI_LONG_DOUBLE         ((MPI_Datatype) 0x4c000c0c)
#define MPI_LONG_LONG_INT       ((MPI_Datatype) 0x4c000809)
#define MPI_UNSIGNED_LONG_LONG  ((MPI_Datatype) 0x4c000819)
#define MPI_LONG_LONG           MPI_LONG_LONG_INT

typedef int         MPI_Comm;
typedef int         MPI_Datatype;

int                 MPI_Init (int *, char ***);
int                 MPI_Finalize (void);
int                 MPI_Abort (MPI_Comm, int)
  __attribute__ ((noreturn));

int                 MPI_Comm_size (MPI_Comm, int *);
int                 MPI_Comm_rank (MPI_Comm, int *);

int                 MPI_Barrier (MPI_Comm);
int                 MPI_Bcast (void *, int, MPI_Datatype, int, MPI_Comm);
int                 MPI_Gather (void *, int, MPI_Datatype,
                                void *, int, MPI_Datatype, int, MPI_Comm);
int                 MPI_Allgather (void *, int, MPI_Datatype,
                                   void *, int, MPI_Datatype, MPI_Comm);

double              MPI_Wtime (void);

#endif /* !SC_MPI_DUMMY_H */
