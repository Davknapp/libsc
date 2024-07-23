/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System
  Additional copyright (C) 2011 individual authors

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

#include <sc_scda.h>
#include <sc_options.h>

#define SC_SCDA_FILE_EXT "scd"
#define SC_SCDA_TEST_FILE "sc_test_scda." SC_SCDA_FILE_EXT

int
main (int argc, char **argv)
{
  sc_MPI_Comm         mpicomm = sc_MPI_COMM_WORLD;
  int                 mpiret, mpirank, mpisize;
  int                 first_argc;
  int                 int_everyn, int_seed;
  const char         *filename = SC_SCDA_TEST_FILE;
  const char         *file_user_string = "This is a test file";
  char                read_user_string[SC_SCDA_USER_STRING_BYTES + 1];
  sc_scda_fcontext_t *fc;
  sc_scda_fopen_options_t scda_opt, scda_opt_err;
  sc_scda_ferror_t    errcode;
  size_t              len;
  sc_options_t       *opt;
  sc_array_t          data;
  const char         *inline_data = "Test inline data               \n";

  mpiret = sc_MPI_Init (&argc, &argv);
  SC_CHECK_MPI (mpiret);
  sc_init (mpicomm, 1, 1, NULL, SC_LP_INFO);

  /* parse command line options */
  opt = sc_options_new (argv[0]);

  sc_options_add_int (opt, 'N', "fuzzy-everyn", &int_everyn, 0,
                      "average fuzzy error return; 0 means no fuzzy returns "
                      "and must be >= 0");
  sc_options_add_int (opt, 'S', "fuzzy-seed", &int_seed, -1,
                      "seed "
                      "for fuzzy error return of scda functions; ignored for "
                      "fuzzy-everyn == 0");

  first_argc =
    sc_options_parse (sc_package_id, SC_LP_DEFAULT, opt, argc, argv);

  if (first_argc < 0 || first_argc != argc) {
    sc_options_print_usage (sc_package_id, SC_LP_ERROR, opt, NULL);
  }

  sc_options_print_summary (sc_package_id, SC_LP_PRODUCTION, opt);

  if (int_everyn < 0) {
    SC_GLOBAL_LERROR ("Usage error: fuzzy-everyn must be >= 0\n");
    sc_options_print_usage (sc_package_id, SC_LP_ERROR, opt, NULL);
    return 1;
  }

  /* Test checking of non-collective fuzzy parameters. */
  mpiret = sc_MPI_Comm_rank (mpicomm, &mpirank);
  SC_CHECK_MPI (mpiret);
  mpiret = sc_MPI_Comm_size (mpicomm, &mpisize);
  SC_CHECK_MPI (mpiret);
  scda_opt_err.info = sc_MPI_INFO_NULL;
  if (mpirank == 0) {
    scda_opt_err.fuzzy_everyn = 0;
    scda_opt_err.fuzzy_seed = 0;
  }
  else {
    scda_opt_err.fuzzy_everyn = 1;
    scda_opt_err.fuzzy_seed = 0;
  }
  if (mpisize > 1) {
    SC_GLOBAL_ESSENTIAL
      ("We expect two invalid scda function parameter errors."
       " This is just for testing purposes and do not imply"
       " erroneous code behavior.\n");
  }
  /* fopen_write with non-collective fuzzy error parameters */
  fc = sc_scda_fopen_write (mpicomm, filename, file_user_string, NULL,
                            &scda_opt_err, &errcode);
  if (mpisize > 1) {
    SC_CHECK_ABORT (fc == NULL && errcode.scdaret == SC_SCDA_FERR_ARG,
                    "Test fuzzy error parameters check");
  }
  else {
    /* we can not provoke non-collective parameter error in serial */
    SC_CHECK_ABORT (fc != NULL && sc_scda_ferror_is_success (errcode),
                    "Test fuzzy error parameters check in serial");
    sc_scda_fclose (fc, &errcode);
    SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                    "scda_fclose after read failed");
  }
  /* fopen_read with non-collective fuzzy error parameters */
  fc =
    sc_scda_fopen_read (mpicomm, filename, read_user_string, &len,
                        &scda_opt_err, &errcode);
  if (mpisize > 1) {
    SC_CHECK_ABORT (fc == NULL && errcode.scdaret == SC_SCDA_FERR_ARG,
                    "Test fuzzy error parameters check");
  }
  else {
    /* we can not provoke non-collective parameter error in serial */
    SC_CHECK_ABORT (fc != NULL && sc_scda_ferror_is_success (errcode),
                    "Test fuzzy error parameters check in serial");
    sc_scda_fclose (fc, &errcode);
    SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                    "scda_fclose after read failed");
  }

  /* Create valid scda options structure. */
  scda_opt.fuzzy_everyn = (unsigned) int_everyn;
  if (int_seed < 0) {
    scda_opt.fuzzy_seed = (sc_rand_state_t) sc_MPI_Wtime ();
    mpiret =
      sc_MPI_Bcast (&scda_opt.fuzzy_seed, 1, sc_MPI_UNSIGNED, 0, mpicomm);
    SC_CHECK_MPI (mpiret);
  }
  else {
    scda_opt.fuzzy_seed = (sc_rand_state_t) int_seed;
  }
  scda_opt.info = sc_MPI_INFO_NULL;

  fc = sc_scda_fopen_write (mpicomm, filename, file_user_string, NULL,
                            &scda_opt, &errcode);
  /* TODO: check errcode */
  SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                  "scda_fopen_write failed");

  /* write an inline section to the file */
  sc_array_init_data (&data, (void *) inline_data, 32, 1);
  fc = sc_scda_fwrite_inline (fc, "Inline section test without user-defined "
                              "padding", NULL, &data, mpisize - 1, &errcode);
  SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                  "scda_fwrite_inline failed");

  sc_scda_fclose (fc, &errcode);
  /* TODO: check errcode and return value */
  SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                  "scda_fclose after write" " failed");

  /* set the options to actiavate fuzzy error testing */
  /* WARNING: Fuzzy error testing means that the code randomly produces
   * errors.
   */

  fc =
    sc_scda_fopen_read (mpicomm, filename, read_user_string, &len, &scda_opt,
                        &errcode);
  /* TODO: check errcode */
  SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                  "scda_fopen_read failed");

  SC_INFOF ("File header user string: %s\n", read_user_string);

  sc_scda_fclose (fc, &errcode);
  /* TODO: check errcode and return value */
  SC_CHECK_ABORT (sc_scda_ferror_is_success (errcode),
                  "scda_fclose after read failed");

  sc_options_destroy (opt);

  sc_finalize ();

  mpiret = sc_MPI_Finalize ();
  SC_CHECK_MPI (mpiret);

  return 0;
}
