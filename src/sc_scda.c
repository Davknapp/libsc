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
#include <sc_io.h>

/* file section header data */
#define SC_SCDA_MAGIC "scdata0" /**< magic encoding format identifier and version */
#define SC_SCDA_MAGIC_BYTES 7   /**< number of magic bytes */
#define SC_SCDA_VENDOR_STRING "libsc" /**< implementation defined data */
#define SC_SCDA_VENDOR_STRING_FIELD 24 /**< byte count for vendor string entry
                                            including the padding */
#define SC_SCDA_VENDOR_STRING_BYTES 20 /**< maximal number of vendor string bytes */
#define SC_SCDA_USER_STRING_FIELD 62   /**< byte count for user string entry
                                            including the padding */
#define SC_SCDA_PADDING_MOD 32  /**< divisor for variable length padding */
#define SC_SCDA_FUZZY_SEED 42   /**< seed for the fuzzy error return */
#define SC_SCDA_FUZZY_FREQUENCY 3 /**< frequency for the fuzzy error return */

/** The opaque file context for for scda files. */
struct sc_scda_fcontext
{
  /* *INDENT-OFF* */
  sc_MPI_Comm         mpicomm; /**< associated MPI communicator */
  int                 mpisize; /**< number of MPI ranks */
  int                 mpirank; /**< MPI rank */
  int                 fuzzy_errors; /**< boolean for fuzzy error return */
  sc_MPI_File         file;    /**< file object */
  /* *INDENT-ON* */
};

static void
sc_scda_copy_bytes (char *dest, const char *src, size_t n)
{
  SC_ASSERT (dest != NULL);
  SC_ASSERT (n == 0 || src != NULL);

  void               *pointer;

  if (n == 0) {
    return;
  }

  pointer = memcpy (dest, src, n);
  SC_EXECUTE_ASSERT_TRUE (pointer == (void *) dest);
}

static void
sc_scda_set_bytes (char *dest, int c, size_t n)
{
  SC_ASSERT (dest != NULL);

  memset (dest, c, n);
}

static void
sc_scda_init_nul (char *dest, size_t len)
{
  SC_ASSERT (dest != NULL);

  sc_scda_set_bytes (dest, '\0', len);
}

/** Pad the input data to a fixed length.
 *
 * The result is written to \b output_data, which must be allocated.
 *
 */
static void
sc_scda_pad_to_fix_len (const char *input_data, size_t input_len,
                        char *output_data, size_t pad_len)
{
  SC_ASSERT (input_data != NULL);
  SC_ASSERT (output_data != NULL);
  SC_ASSERT (input_len <= pad_len - 4);

#if 0
  uint8_t            *byte_arr;
#endif

  /* We assume that output_data has at least pad_len allocated bytes. */

  /* copy input data into output_data */
  sc_scda_copy_bytes (output_data, input_data, input_len);

  /* append padding */
#if 0
  byte_arr = (uint8_t *) padded_data;
#endif
  output_data[input_len] = ' ';
  sc_scda_set_bytes (&output_data[input_len + 1], '-',
                     pad_len - input_len - 2);
  output_data[pad_len - 1] = '\n';
}

/** This function checks if \b padded_data is actually padded to \b pad_len.
 *
 */
static int
sc_scda_get_pad_to_fix_len (char *padded_data, size_t pad_len, char *raw_data,
                            size_t *raw_len)
{
  SC_ASSERT (padded_data != NULL);
  SC_ASSERT (raw_data != NULL);
  SC_ASSERT (raw_len != NULL);

  size_t              si;

  if (pad_len < 4) {
    /* data too short to satisfy padding */
    return -1;
  }

  if (padded_data[pad_len - 1] != '\n') {
    /* wrong termination */
    return -1;
  }

  for (si = pad_len - 2; si != 0; --si) {
    if (padded_data[si] != '-') {
      break;
    }
  }
  if (padded_data[si] != ' ') {
    return -1;
  }

  /* the padding was valid and the remaining data is the actual data */
  *raw_len = si;
  sc_scda_copy_bytes (raw_data, padded_data, *raw_len);

  return 0;
}

/** Get the number of padding bytes with respect to the padding of \ref
 *  sc_scda_pad_to_mod.
 *
 */
static size_t
sc_scda_pad_to_mod_len (size_t input_len)
{
  size_t              num_pad_bytes;

  /* compute the number of padding bytes */
  num_pad_bytes =
    (SC_SCDA_PADDING_MOD -
     (input_len % SC_SCDA_PADDING_MOD)) % SC_SCDA_PADDING_MOD;

  if (num_pad_bytes < 7) {
    /* not sufficient number of padding bytes for the padding format */
    num_pad_bytes += SC_SCDA_PADDING_MOD;
  }

  return num_pad_bytes;
}

static void
sc_scda_pad_to_mod (const char *input_data, size_t input_len,
                    char *output_data)
{
  SC_ASSERT (input_len == 0 || input_data != NULL);
  SC_ASSERT (output_data != NULL);

  size_t              num_pad_bytes;

  /* compute the number of padding bytes */
  num_pad_bytes = sc_scda_pad_to_mod_len (input_len);

  SC_ASSERT (num_pad_bytes >= 6);
  SC_ASSERT (num_pad_bytes <= SC_SCDA_PADDING_MOD + 6);

  sc_scda_copy_bytes (output_data, input_data, input_len);

  /* check for last byte to decide on padding format */
  if (input_len > 0 && input_data[input_len - 1] == '\n') {
    /* input data ends with a line break */
    output_data[input_len] = '=';
  }
  else {
    /* add a line break add the beginning of the padding */
    output_data[input_len] = '\n';
  }
  output_data[input_len + 1] = '=';

  /* append the remaining padding bytes */
  sc_scda_set_bytes (&output_data[input_len + 2], '=', num_pad_bytes - 4);
  output_data[input_len + num_pad_bytes - 2] = '\n';
  output_data[input_len + num_pad_bytes - 1] = '\n';
}

/** Checks if \b padded_data is actually padded with respect to
 *  \ref SC_SCDA_PADDING_MOD.
 *
 * Given the raw data length, this function checks if the padding format is
 * correct and extracts the raw data.
 */
static int
sc_scda_get_pad_to_mod (char *padded_data, size_t padded_len, size_t raw_len,
                        char *raw_data)
{
  SC_ASSERT (padded_data != NULL);
  SC_ASSERT (raw_len == 0 || raw_data != NULL);

  size_t              si;
  size_t              num_pad_bytes;

  num_pad_bytes = sc_scda_pad_to_mod_len (raw_len);

  /* check if padding data length conforms to the padding format */
  if (num_pad_bytes + raw_len != padded_len) {
    /* raw_len and padded_len are not consistent */
    return -1;
  }
  SC_ASSERT (padded_len >= 7);

  /* check the content of the padding bytes */
  if (padded_data[padded_len - 1] != '\n' ||
      padded_data[padded_len - 2] != '\n') {
    /* terminating line breaks are missing */
    return -1;
  }

  for (si = padded_len - 3; si != padded_len - num_pad_bytes; --si) {
    if (padded_data[si] != '=') {
      /* wrong padding character */
      return -1;
    }
  }
  SC_ASSERT (si == raw_len);

  if ((!((padded_data[si] == '=' && raw_len != 0 &&
          padded_data[si - 1] == '\n') || padded_data[si] == '\n'))) {
    /* wrong padding start */
    return -1;
  }

  if (raw_len != 0) {
    /* get the raw data if we required raw_data != NULL */
    sc_scda_copy_bytes (raw_data, padded_data, raw_len);
  }

  return 0;
}

/**
 * This function is for creating and reading a file.
 * TODO: We may want to add an error parameter in indicate if the options are
 * invalid.
 */
static              sc_MPI_Info
sc_scda_examine_options (sc_scda_fopen_options_t * opt, int *fuzzy)
{
  /* TODO: Check options if opt is valid? */

  sc_MPI_Info         info;

  if (opt != NULL) {
    info = opt->info;
    *fuzzy = opt->fuzzy_errors;
  }
  else {
    info = sc_MPI_INFO_NULL;
    /* no fuzzy error return by default */
    *fuzzy = 0;
  }

  return info;
}

static void
sc_scda_fill_mpi_data (sc_scda_fcontext_t * fc, sc_MPI_Comm mpicomm)
{
  SC_ASSERT (fc != NULL);

  int                 mpiret;

  mpiret = sc_MPI_Comm_size (mpicomm, &fc->mpisize);
  SC_CHECK_MPI (mpiret);

  mpiret = sc_MPI_Comm_rank (mpicomm, &fc->mpirank);
  SC_CHECK_MPI (mpiret);
}

/** Get the user string length for writing.
 *
 * This functions return -1 if the input user string is not
 * compliant with the scda file format.
 */
static int
sc_scda_get_user_string_len (const char *user_string,
                             const size_t *in_len, size_t *out_len)
{
  SC_ASSERT (user_string != NULL);
  SC_ASSERT (out_len != NULL);

  if (in_len != NULL) {
    /* binary user string */
    if (*in_len > SC_SCDA_USER_STRING_BYTES) {
      /* binary user string is too long */
      return -1;
    }

    if (user_string[*in_len] != '\0') {
      /* missing nul-termination */
      return -1;
    }

    *out_len = *in_len;
    return 0;
  }
  else {
    /* we expect a nul-terminated C string */
    char                user_string_copy[SC_SCDA_USER_STRING_BYTES + 1];
    size_t              len;

    sc_strcopy (user_string_copy, SC_SCDA_USER_STRING_BYTES + 1, user_string);

    /* user_string_copy is guaranteed to be nul-terminated */
    len = strlen (user_string_copy);
    if (len < SC_SCDA_USER_STRING_BYTES) {
      /* user string is nul-terminated */
      *out_len = len;
      return 0;
    }

    SC_ASSERT (len == SC_SCDA_USER_STRING_BYTES);

    /* check for nul at last byte position of user string */
    if (user_string[SC_SCDA_USER_STRING_BYTES - 1] == '\0') {
      *out_len = len;
      return 0;
    }

    /* user string is not nul-terminated */
    return -1;
  }

  /* unreachable */
  SC_ABORT_NOT_REACHED ();
}

/** Create a random but consistent scdaret.
 */
static void
sc_scda_get_fuzzy_scdaret (sc_scda_ferror_t * scda_errorcode)
{
  /* TODO: Generate uniform sample from valid scdare values */
  /* TODO: Do we want to get the actual scdaret to be able to draw
   * a weighted sample?
   */
}

/** Converts a scdaret error code into a sc_scda_ferror_t code.
 */
static void
sc_scda_scdaret_to_errcode (sc_scda_ret_t scda_ret,
                            sc_scda_ferror_t * scda_errorcode, int fuzzy_errors)
{
  SC_ASSERT (SC_SCDA_FERR_SUCCESS <= scda_ret &&
             scda_ret < SC_SCDA_FERR_LASTCODE);
  SC_ASSERT (scda_errorcode != NULL);

  /* if we have an MPI error; we need \ref sc_scda_mpiret_to_errcode */
  SC_ASSERT (scda_ret != SC_SCDA_FERR_MPI);

  if (fuzzy_errors) {
    scda_errorcode->scdaret = scda_ret;
    scda_errorcode->mpiret = sc_MPI_SUCCESS;
  }
  else {
    /* TODO: fuzzy error testing */
  }
}

/** Converts an MPI or libsc error code into a sc_scda_ferror_t code.
 */
static void
sc_scda_mpiret_to_errcode (int mpiret, sc_scda_ferror_t * scda_errorcode,
                           int fuzzy_errors)
{
  SC_ASSERT ((sc_MPI_SUCCESS <= mpiret && mpiret < sc_MPI_ERR_LASTCODE));
  SC_ASSERT (scda_errorcode != NULL);

  if (!fuzzy_errors) {
  scda_errorcode->scdaret =
    (mpiret == sc_MPI_SUCCESS) ? SC_SCDA_FERR_SUCCESS : SC_SCDA_FERR_MPI;
  scda_errorcode->mpiret = mpiret;
  }
  else {
    /* TODO: fuzzy error testing */
  }
}

static int
sc_scda_is_success (sc_scda_ferror_t * scda_errorcode)
{
  SC_ASSERT (scda_errorcode != NULL);

  return !scda_errorcode->scdaret && !scda_errorcode->mpiret;
}

sc_scda_fcontext_t *
sc_scda_fopen_write (sc_MPI_Comm mpicomm,
                     const char *filename,
                     const char *user_string, size_t *len,
                     sc_scda_fopen_options_t * opt,
                     sc_scda_ferror_t * errcode)
{
  SC_ASSERT (filename != NULL);
  SC_ASSERT (user_string != NULL);
  SC_ASSERT (errcode != NULL);

  int                 mpiret;
  sc_MPI_Info         info;
  sc_scda_fcontext_t *fc;

  /* We assume the filename to be nul-terminated. */

  /* allocate the file context */
  fc = SC_ALLOC (sc_scda_fcontext_t, 1);

  /* examine options */
  info = sc_scda_examine_options (opt, &fc->fuzzy_errors);
  /* TODO: check if the options are valid */

  /* fill convenience MPI information */
  sc_scda_fill_mpi_data (fc, mpicomm);

  /* open the file for writing */
  mpiret =
    sc_io_open (mpicomm, filename, SC_IO_WRITE_CREATE, info, &fc->file);
  sc_scda_mpiret_to_errcode (mpiret, errcode, fc->fuzzy_errors);
  if (!sc_scda_is_success (errcode)) {
    /* TODO: print error string with SC_GLOBAL_LERRORF */
    /* TODO: cleanup fc->file */
    SC_FREE (fc);
    return NULL;
  }
  /* TODO: check return value */

  if (fc->mpirank == 0) {
    int                 count;
    int                 current_len;
    char                file_header_data[SC_SCDA_HEADER_BYTES];
    size_t              user_string_len;

    /* get scda file header section */
    /* magic */
    sc_scda_copy_bytes (file_header_data, SC_SCDA_MAGIC, SC_SCDA_MAGIC_BYTES);
    current_len = SC_SCDA_MAGIC_BYTES;

    file_header_data[current_len++] = ' ';

    /* vendor string */
    sc_scda_pad_to_fix_len (SC_SCDA_VENDOR_STRING,
                            strlen (SC_SCDA_VENDOR_STRING),
                            &file_header_data[current_len],
                            SC_SCDA_VENDOR_STRING_FIELD);
    current_len += SC_SCDA_VENDOR_STRING_FIELD;

    /* file section specifying character */
    file_header_data[current_len++] = 'F';
    file_header_data[current_len++] = ' ';

    /* user string */
    /* check the user string */
    /* According to 'A.2 Parameter conventions' in the scda specification
     * it is an unchecked runtime error if the user string is not collective,
     * and it leads to undefined behavior.
     * Therefore, we just check the user string on rank 0.
     */
    if (sc_scda_get_user_string_len (user_string, len, &user_string_len)) {
      /* TODO: clean up and snyc */
      return NULL;
    }
    sc_scda_pad_to_fix_len (user_string, user_string_len,
                            &file_header_data[current_len],
                            SC_SCDA_USER_STRING_FIELD);
    current_len += SC_SCDA_USER_STRING_FIELD;

    /* pad the file header section */
    sc_scda_pad_to_mod (NULL, 0, &file_header_data[current_len]);
    current_len += SC_SCDA_PADDING_MOD;

    SC_ASSERT (current_len == SC_SCDA_HEADER_BYTES);

    /* write scda file header section */
    mpiret =
      sc_io_write_at (fc->file, 0, file_header_data, SC_SCDA_HEADER_BYTES,
                      sc_MPI_BYTE, &count);
    /* TODO: check return value and count */
  }

  return fc;
}

static int
sc_scda_check_file_header (char *file_header_data, char *user_string,
                           size_t *len)
{
  SC_ASSERT (file_header_data != NULL);
  SC_ASSERT (user_string != NULL);
  SC_ASSERT (len != NULL);

  int                 current_pos;
  char                vendor_string[SC_SCDA_VENDOR_STRING_BYTES];
  size_t              vendor_len;

  /* TODO: Add errcode as output parameter */

  /* check structure that is not padding */
  /* *INDENT-OFF* */
  if (!(file_header_data[SC_SCDA_MAGIC_BYTES] == ' ' &&
        file_header_data[SC_SCDA_MAGIC_BYTES + 1 +
                         SC_SCDA_VENDOR_STRING_FIELD] == 'F' &&
        file_header_data[SC_SCDA_MAGIC_BYTES + 1 +
                         SC_SCDA_VENDOR_STRING_FIELD + 1] == ' ')) {
    /* wrong file header structure */
    return -1;
  }
  /* *INDENT-ON* */

  /* check the entries of the file header */

  /* check magic */
  if (memcmp (SC_SCDA_MAGIC, file_header_data, SC_SCDA_MAGIC_BYTES)) {
    /* wrong magic */
    return -1;
  }
  current_pos = SC_SCDA_MAGIC_BYTES + 1;

  /* check the padding of the vendor string */
  if (sc_scda_get_pad_to_fix_len (&file_header_data[current_pos],
                                  SC_SCDA_VENDOR_STRING_FIELD, vendor_string,
                                  &vendor_len)) {
    /* wrong padding format */
    return -1;
  }
  /* vendor string content is not checked */

  current_pos += SC_SCDA_VENDOR_STRING_FIELD + 2;
  /* check the user string */
  if (sc_scda_get_pad_to_fix_len
      (&file_header_data
       [current_pos], SC_SCDA_USER_STRING_FIELD, user_string, len)) {
    /* wrong padding format */
    return -1;
  }
  /* the user string content is not checked */
  user_string[*len] = '\0';

  current_pos += SC_SCDA_USER_STRING_FIELD;
  /* check the padding of zero data bytes */
  if (sc_scda_get_pad_to_mod
      (&file_header_data[current_pos], SC_SCDA_PADDING_MOD, 0, NULL)) {
    /* wrong padding format */
    return -1;
  }

  return 0;
}

sc_scda_fcontext_t *
sc_scda_fopen_read (sc_MPI_Comm mpicomm,
                    const char *filename,
                    char *user_string, size_t *len,
                    sc_scda_fopen_options_t * opt, sc_scda_ferror_t * errcode)
{
  SC_ASSERT (filename != NULL);
  SC_ASSERT (user_string != NULL);
  SC_ASSERT (len != NULL);
  SC_ASSERT (errcode != NULL);

  int                 mpiret;
  sc_MPI_Info         info;
  sc_scda_fcontext_t *fc;

  /* We assume the filename to be nul-terminated. */

  /* allocate the file context */
  fc = SC_ALLOC (sc_scda_fcontext_t, 1);

  /* examine options */
  info = sc_scda_examine_options (opt, &fc->fuzzy_errors);
  /* TODO: check if options are valid */

  /* fill convenience MPI information */
  sc_scda_fill_mpi_data (fc, mpicomm);

  /* open the file in reading mode */
  mpiret = sc_io_open (mpicomm, filename, SC_IO_READ, info, &fc->file);
  /* TODO: check return value */

  /* read file header section on rank 0 */
  if (fc->mpirank == 0) {
    int                 count;
    char                file_header_data[SC_SCDA_HEADER_BYTES];

    mpiret =
      sc_io_read_at (fc->file, 0, file_header_data, SC_SCDA_HEADER_BYTES,
                     sc_MPI_BYTE, &count);
    /* TODO: check return value and count */

    /* initialize user_string */
    sc_scda_init_nul (user_string, SC_SCDA_USER_STRING_BYTES + 1);

    if (sc_scda_check_file_header (file_header_data, user_string, len)) {
      /* invalid file header data */
      /* TODO: clean up */
      return NULL;
    }
  }
  /* Bcast the user string */
  mpiret = sc_MPI_Bcast (user_string, SC_SCDA_USER_STRING_BYTES + 1,
                         sc_MPI_BYTE, 0, mpicomm);
  SC_CHECK_MPI (mpiret);

  return fc;
}

int
sc_scda_fclose (sc_scda_fcontext_t * fc, sc_scda_ferror_t * errcode)
{
  SC_ASSERT (fc != NULL);
  SC_ASSERT (errcode != NULL);

  int                 retval;

  /* TODO: further checks before calling sc_io_close? */

  retval = sc_io_close (&fc->file);
  /* TODO: handle return value */

  SC_FREE (fc);

  return 0;
}
