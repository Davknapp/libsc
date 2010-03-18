/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2008,2009 Carsten Burstedde, Lucas Wilcox,
                          Toby Isaac.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <p4est_to_p8est.h>
#include <p8est_connectivity.h>
#include <sc_io.h>

/* *INDENT-OFF* */
const int           p8est_face_corners[6][4] =
{{ 0, 2, 4, 6 },
 { 1, 3, 5, 7 },
 { 0, 1, 4, 5 },
 { 2, 3, 6, 7 },
 { 0, 1, 2, 3 },
 { 4, 5, 6, 7 }};
const int           p8est_face_edges[6][4] =
{{ 4, 6,  8, 10 },
 { 5, 7,  9, 11 },
 { 0, 2,  8,  9 },
 { 1, 3, 10, 11 },
 { 0, 1,  4,  5 },
 { 2, 3,  6,  7 }};
const int           p8est_face_dual[6] = { 1, 0, 3, 2, 5, 4 };
const int           p8est_face_permutations[8][4] =
{{ 0, 1, 2, 3 },                /* no.  0 of 0..23 */
 { 0, 2, 1, 3 },                /* no.  2 of 0..23 */
 { 1, 0, 3, 2 },                /* no.  7 of 0..23 */
 { 1, 3, 0, 2 },                /* no. 10 of 0..23 */
 { 2, 0, 3, 1 },                /* no. 13 of 0..23 */
 { 2, 3, 0, 1 },                /* no. 16 of 0..23 */
 { 3, 1, 2, 0 },                /* no. 21 of 0..23 */
 { 3, 2, 1, 0 }};               /* no. 23 of 0..23 */
const int           p8est_face_permutation_sets[3][4] =
{{ 1, 2, 5, 6 },
 { 0, 3, 4, 7 },
 { 0, 4, 3, 7 }};
const int           p8est_face_permutation_refs[6][6] =
{{ 0, 1, 1, 0, 0, 1 },
 { 2, 0, 0, 1, 1, 0 },
 { 2, 0, 0, 1, 1, 0 },
 { 0, 2, 2, 0, 0, 1 },
 { 0, 2, 2, 0, 0, 1 },
 { 2, 0, 0, 2, 2, 0 }};

const int           p8est_edge_faces[12][2] =
{{ 2, 4 },
 { 3, 4 },
 { 2, 5 },
 { 3, 5 },
 { 0, 4 },
 { 1, 4 },
 { 0, 5 },
 { 1, 5 },
 { 0, 2 },
 { 1, 2 },
 { 0, 3 },
 { 1, 3 }};
const int           p8est_edge_corners[12][2] =
{{ 0, 1 },
 { 2, 3 },
 { 4, 5 },
 { 6, 7 },
 { 0, 2 },
 { 1, 3 },
 { 4, 6 },
 { 5, 7 },
 { 0, 4 },
 { 1, 5 },
 { 2, 6 },
 { 3, 7 }};
const int           p8est_edge_face_corners[12][6][2] =
{{{ -1, -1 }, { -1, -1 }, {  0,  1 }, { -1, -1 }, {  0,  1 }, { -1, -1 }},
 {{ -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  1 }, {  2,  3 }, { -1, -1 }},
 {{ -1, -1 }, { -1, -1 }, {  2,  3 }, { -1, -1 }, { -1, -1 }, {  0,  1 }},
 {{ -1, -1 }, { -1, -1 }, { -1, -1 }, {  2,  3 }, { -1, -1 }, {  2,  3 }},
 {{  0,  1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  2 }, { -1, -1 }},
 {{ -1, -1 }, {  0,  1 }, { -1, -1 }, { -1, -1 }, {  1,  3 }, { -1, -1 }},
 {{  2,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  0,  2 }},
 {{ -1, -1 }, {  2,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, {  1,  3 }},
 {{  0,  2 }, { -1, -1 }, {  0,  2 }, { -1, -1 }, { -1, -1 }, { -1, -1 }},
 {{ -1, -1 }, {  0,  2 }, {  1,  3 }, { -1, -1 }, { -1, -1 }, { -1, -1 }},
 {{  1,  3 }, { -1, -1 }, { -1, -1 }, {  0,  2 }, { -1, -1 }, { -1, -1 }},
 {{ -1, -1 }, {  1,  3 }, { -1, -1 }, {  1,  3 }, { -1, -1 }, { -1, -1 }}};

const int           p8est_corner_faces[8][3] =
{{ 0, 2, 4 },
 { 1, 2, 4 },
 { 0, 3, 4 },
 { 1, 3, 4 },
 { 0, 2, 5 },
 { 1, 2, 5 },
 { 0, 3, 5 },
 { 1, 3, 5 }};
const int           p8est_corner_edges[8][3] =
{{ 0, 4,  8 },
 { 0, 5,  9 },
 { 1, 4, 10 },
 { 1, 5, 11 },
 { 2, 6,  8 },
 { 2, 7,  9 },
 { 3, 6, 10 },
 { 3, 7, 11 }};
const int           p8est_corner_face_corners[8][6] =
{{  0, -1,  0, -1,  0, -1 },
 { -1,  0,  1, -1,  1, -1 },
 {  1, -1, -1,  0,  2, -1 },
 { -1,  1, -1,  1,  3, -1 },
 {  2, -1,  2, -1, -1,  0 },
 { -1,  2,  3, -1, -1,  1 },
 {  3, -1, -1,  2, -1,  2 },
 { -1,  3, -1,  3, -1,  3 }};

const int           p8est_child_edge_faces[8][12] =
{{ -1,  4,  2, -1, -1,  4,  0, -1, -1,  2,  0, -1 },
 { -1,  4,  2, -1,  4, -1, -1,  1,  2, -1, -1,  1 },
 {  4, -1, -1,  3, -1,  4,  0, -1,  0, -1, -1,  3 },
 {  4, -1, -1,  3,  4, -1, -1,  1, -1,  1,  3, -1 },
 {  2, -1, -1,  5,  0, -1, -1,  5, -1,  2,  0, -1 },
 {  2, -1, -1,  5, -1,  1,  5, -1,  2, -1, -1,  1 },
 { -1,  3,  5, -1,  0, -1, -1,  5,  0, -1, -1,  3 },
 { -1,  3,  5, -1, -1,  1,  5, -1, -1,  1,  3, -1 }};
const int           p8est_child_corner_faces[8][8] =
{{ -1, -1, -1,  4, -1,  2,  0, -1 },
 { -1, -1,  4, -1,  2, -1, -1,  1 },
 { -1,  4, -1, -1,  0, -1, -1,  3 },
 {  4, -1, -1, -1, -1,  1,  3, -1 },
 { -1,  2,  0, -1, -1, -1, -1,  5 },
 {  2, -1, -1,  1, -1, -1,  5, -1 },
 {  0, -1, -1,  3, -1,  5, -1, -1 },
 { -1,  1,  3, -1,  5, -1, -1, -1 }};
const int           p8est_child_corner_edges[8][8] =
{{ -1,  0,  4, -1,  8, -1, -1, -1 },
 {  0, -1, -1,  5, -1,  9, -1, -1 },
 {  4, -1, -1,  1, -1, -1, 10, -1 },
 { -1,  5,  1, -1, -1, -1, -1, 11 },
 {  8, -1, -1, -1, -1,  2,  6, -1 },
 { -1,  9, -1, -1,  2, -1, -1,  7 },
 { -1, -1, 10, -1,  6, -1, -1,  3 },
 { -1, -1, -1, 11, -1,  7,  3, -1 }};
/* *INDENT-ON* */

#include "p4est_connectivity.c"

p4est_connectivity_t *
p8est_connectivity_new_unitcube (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_ett = 0;
  const p4est_topidx_t num_ctt = 0;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    0, 1, 2, 3, 4, 5,
  };

  return p4est_connectivity_new_copy (num_vertices, num_trees, 0, 0,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      NULL, &num_ett, NULL, NULL,
                                      NULL, &num_ctt, NULL, NULL);
}

p4est_connectivity_t *
p8est_connectivity_new_periodic (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_edges = 3;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    1, 0, 3, 2, 5, 4,
  };
  const p4est_topidx_t tree_to_edge[1 * 12] = {
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
  };
  const p4est_topidx_t ett_offset[3 + 1] = {
    0, 4, 8, 12,
  };
  const p4est_topidx_t edge_to_tree[12] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        edge_to_edge[12] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  };
  const p4est_topidx_t tree_to_corner[1 * 8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = {
    0, 8,
  };
  const p4est_topidx_t corner_to_tree[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        corner_to_corner[8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };

  return p4est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p4est_connectivity_t *
p8est_connectivity_new_rotwrap (void)
{
  const p4est_topidx_t num_vertices = 8;
  const p4est_topidx_t num_trees = 1;
  const p4est_topidx_t num_edges = 4;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[8 * 3] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
    0, 0, 1,
    1, 0, 1,
    0, 1, 1,
    1, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[1 * 8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };
  const p4est_topidx_t tree_to_tree[1 * 6] = {
    0, 0, 0, 0, 0, 0,
  };
  const int8_t        tree_to_face[1 * 6] = {
    1, 0, 2, 3, 11, 10,
  };
  const p4est_topidx_t tree_to_edge[1 * 12] = {
    0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 3, 3,
  };
  const p4est_topidx_t ett_offset[4 + 1] = {
    0, 4, 8, 10, 12,
  };
  const p4est_topidx_t edge_to_tree[12] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        edge_to_edge[12] = {
    0, 7, 1, 6, 2, 16, 3, 17, 8, 9, 10, 11,
  };
  const p4est_topidx_t tree_to_corner[1 * 8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = {
    0, 8,
  };
  const p4est_topidx_t corner_to_tree[8] = {
    0, 0, 0, 0, 0, 0, 0, 0,
  };
  const int8_t        corner_to_corner[8] = {
    0, 1, 2, 3, 4, 5, 6, 7,
  };

  return p4est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p4est_connectivity_t *
p8est_connectivity_new_twocubes (void)
{
  const p4est_topidx_t num_vertices = 12;
  const p4est_topidx_t num_trees = 2;
  const p4est_topidx_t num_ett = 0;
  const p4est_topidx_t num_ctt = 0;
  const double        vertices[12 * 3] = {
    0, 0, 0,
    1, 0, 0,
    2, 0, 0,
    0, 1, 0,
    1, 1, 0,
    2, 1, 0,
    0, 0, 1,
    1, 0, 1,
    2, 0, 1,
    0, 1, 1,
    1, 1, 1,
    2, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[2 * 8] = {
    0, 1, 3, 4, 6, 7, 9, 10,
    1, 2, 4, 5, 7, 8, 10, 11,
  };
  const p4est_topidx_t tree_to_tree[2 * 6] = {
    0, 1, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1,
  };
  const int8_t        tree_to_face[2 * 6] = {
    0, 0, 2, 3, 4, 5,
    1, 1, 2, 3, 4, 5,
  };

  return p4est_connectivity_new_copy (num_vertices, num_trees, 0, 0,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      NULL, &num_ett, NULL, NULL,
                                      NULL, &num_ctt, NULL, NULL);
}

p4est_connectivity_t *
p8est_connectivity_new_twowrap (void)
{
  const p4est_topidx_t num_vertices = 12;
  const p4est_topidx_t num_trees = 2;
  const p4est_topidx_t num_ett = 0;
  const p4est_topidx_t num_ctt = 0;
  const double        vertices[12 * 3] = {
    0, 0, 0,
    1, 0, 0,
    2, 0, 0,
    0, 1, 0,
    1, 1, 0,
    2, 1, 0,
    0, 0, 1,
    1, 0, 1,
    2, 0, 1,
    0, 1, 1,
    1, 1, 1,
    2, 1, 1,
  };
  const p4est_topidx_t tree_to_vertex[2 * 8] = {
    3, 9, 0, 6, 4, 10, 1, 7,
    8, 2, 7, 1, 11, 5, 10, 4,
  };
  const p4est_topidx_t tree_to_tree[2 * 6] = {
    0, 0, 0, 0, 1, 1,
    1, 1, 0, 0, 1, 1,
  };
  const int8_t        tree_to_face[2 * 6] = {
    0, 1, 2, 3, 20, 21,
    0, 1, 22, 23, 4, 5,
  };

  return p4est_connectivity_new_copy (num_vertices, num_trees, 0, 0,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      NULL, &num_ett, NULL, NULL,
                                      NULL, &num_ctt, NULL, NULL);
}

/* This function is contributed by Toby Isaac. */
p4est_connectivity_t *
p8est_connectivity_new_brick (int mi, int ni, int pi, int periodic_a,
                              int periodic_b, int periodic_c)
{
  const p4est_topidx_t m = (p4est_topidx_t) mi;
  const p4est_topidx_t n = (p4est_topidx_t) ni;
  const p4est_topidx_t p = (p4est_topidx_t) pi;
  const p4est_topidx_t num_trees = m * n * p;
  const p4est_topidx_t num_vertices = (m + 1) * (n + 1) * (p + 1);
  const p4est_topidx_t mc = periodic_a ? m : (m - 1);
  const p4est_topidx_t nc = periodic_b ? n : (n - 1);
  const p4est_topidx_t pc = periodic_c ? p : (p - 1);
  const p4est_topidx_t num_corners = mc * nc * pc;
  const p4est_topidx_t num_ctt = 8 * num_corners;
  const p4est_topidx_t num_edges = m * nc * pc + mc * n * pc + mc * nc * p;
  const p4est_topidx_t num_ett = 4 * num_edges;
  const int           periodic[3] = { periodic_a, periodic_b, periodic_c };
  const p4est_topidx_t max[3] = { m - 1, n - 1, p - 1 };
  double             *vertices;
  p4est_topidx_t     *tree_to_vertex;
  p4est_topidx_t     *tree_to_tree;
  int8_t             *tree_to_face;
  p4est_topidx_t     *tree_to_edge;
  p4est_topidx_t     *ett_offset;
  p4est_topidx_t     *edge_to_tree;
  int8_t             *edge_to_edge;
  p4est_topidx_t     *tree_to_corner;
  p4est_topidx_t     *ctt_offset;
  p4est_topidx_t     *corner_to_tree;
  int8_t             *corner_to_corner;
  p4est_topidx_t      cube_length, n_iter;
  int                 log_cl;
  int                 i, j, k, l;
  p4est_topidx_t      ti, tj, tk, tl;
  p4est_topidx_t      tx, ty, tz;
  p4est_topidx_t      tfx[6], tfy[6], tfz[6];
  p4est_topidx_t      tex[12], tey[12], tez[12];
  p4est_topidx_t      tcx[8], tcy[8], tcz[8];
  p4est_topidx_t      tf[6], te[12], tc[8];
  p4est_topidx_t      coord[3], ttemp;
  p4est_topidx_t     *linear_to_tree;
  p4est_topidx_t     *tree_to_corner2;
  p4est_topidx_t     *tree_to_edge2;
  p4est_topidx_t      vcount = 0, vicount = 0;
  int                 dir1, dir2;
  int                 c[3];
  p4est_connectivity_t *conn;

  P4EST_ASSERT (m > 0 && n > 0 && p > 0);
  conn = p4est_connectivity_new (num_vertices, num_trees, num_edges, num_ett,
                                 num_corners, num_ctt);

  vertices = conn->vertices;
  tree_to_vertex = conn->tree_to_vertex;
  tree_to_tree = conn->tree_to_tree;
  tree_to_face = conn->tree_to_face;
  tree_to_edge = conn->tree_to_edge;
  ett_offset = conn->ett_offset;
  edge_to_tree = conn->edge_to_tree;
  edge_to_edge = conn->edge_to_edge;
  tree_to_corner = conn->tree_to_corner;
  ctt_offset = conn->ctt_offset;
  corner_to_tree = conn->corner_to_tree;
  corner_to_corner = conn->corner_to_corner;

  for (ti = 0; ti < num_edges + 1; ti++) {
    ett_offset[ti] = 4 * ti;
  }

  for (ti = 0; ti < num_corners + 1; ti++) {
    ctt_offset[ti] = 8 * ti;
  }

  for (ti = 0; ti < 8 * num_trees; ti++) {
    tree_to_vertex[ti] = -1;
  }

  cube_length = (m > n) ? m : n;
  cube_length = (cube_length > p) ? cube_length : p;
  log_cl = SC_LOG2_32 (cube_length - 1) + 1;
  cube_length = ((p4est_locidx_t) 1) << log_cl;
  n_iter = cube_length * cube_length * cube_length;

  linear_to_tree = P4EST_ALLOC (p4est_topidx_t, n_iter);
  tree_to_corner2 = P4EST_ALLOC (p4est_topidx_t, num_trees);
  tree_to_edge2 = P4EST_ALLOC (p4est_topidx_t, 3 * num_trees);

  tj = 0;
  tk = 0;
  tl = 0;
  for (ti = 0; ti < n_iter; ti++) {
    tx = 0;
    ty = 0;
    tz = 0;
    for (i = 0; i < log_cl; i++) {
      tx |= (ti & (1 << (3 * i))) >> (2 * i);
      ty |= (ti & (1 << (3 * i + 1))) >> (2 * i + 1);
      tz |= (ti & (1 << (3 * i + 2))) >> (2 * i + 2);
    }
    if (tx < m && ty < n && tz < p) {
      linear_to_tree[ti] = tj;
      if ((tx < m - 1 || periodic_a) && (ty < n - 1 || periodic_b) &&
          (tz < p - 1 || periodic_c)) {
        tree_to_corner2[tj] = tk++;
        tree_to_edge2[3 * tj] = tl++;
        tree_to_edge2[3 * tj + 1] = tl++;
        tree_to_edge2[3 * tj + 2] = tl++;
      }
      else {
        tree_to_corner2[tj] = -1;
        if ((ty < n - 1 || periodic_b) && (tz < p - 1 || periodic_c)) {
          tree_to_edge2[3 * tj] = tl++;
        }
        else {
          tree_to_edge2[3 * tj] = -1;
        }
        if ((tx < m - 1 || periodic_a) && (tz < p - 1 || periodic_c)) {
          tree_to_edge2[3 * tj + 1] = tl++;
        }
        else {
          tree_to_edge2[3 * tj + 1] = -1;
        }
        if ((tx < m - 1 || periodic_a) && (ty < n - 1 || periodic_b)) {
          tree_to_edge2[3 * tj + 2] = tl++;
        }
        else {
          tree_to_edge2[3 * tj + 2] = -1;
        }
      }
      tj++;
    }
    else {
      linear_to_tree[ti] = -1;
    }
  }
  P4EST_ASSERT (tj == num_trees);
  P4EST_ASSERT (tk == num_corners);
  P4EST_ASSERT (tl == num_edges);

  for (ti = 0; ti < n_iter; ti++) {
    tx = ty = tz = 0;
    for (i = 0; i < log_cl; i++) {
      tx |= (ti & (1 << (3 * i))) >> (2 * i);
      ty |= (ti & (1 << (3 * i + 1))) >> (2 * i + 1);
      tz |= (ti & (1 << (3 * i + 2))) >> (2 * i + 2);
    }
    coord[0] = tx;
    coord[1] = ty;
    coord[2] = tz;
    if (tx < m && ty < n && tz < p) {
      tj = linear_to_tree[ti];
      P4EST_ASSERT (tj >= 0);
      for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
          l = i * 2 + j;
          tfx[l] = ((tx + ((i == 0) ? (2 * j - 1) : 0)) + m) % m;
          tfy[l] = ((ty + ((i == 1) ? (2 * j - 1) : 0)) + n) % n;
          tfz[l] = ((tz + ((i == 2) ? (2 * j - 1) : 0)) + p) % p;
          tf[l] = 0;
          for (k = 0; k < log_cl; k++) {
            tf[l] |= (tfx[l] & (1 << k)) << (2 * k);
            tf[l] |= (tfy[l] & (1 << k)) << (2 * k + 1);
            tf[l] |= (tfz[l] & (1 << k)) << (2 * k + 2);
          }
          tf[l] = linear_to_tree[tf[l]];
          P4EST_ASSERT (tf[l] >= 0);
        }
        for (j = 0; j < 4; j++) {
          l = 4 * i + j;
          tex[l] = ((tx + ((i == 0) ? 0 : (2 * (j & 1) - 1))) + m) % m;
          tey[l] = ((ty + ((i == 1) ? 0 :
                           (2 * ((i == 0) ? (j & 1) : (j / 2)) - 1))) +
                    n) % n;
          tez[l] = ((tz + ((i == 2) ? 0 : (2 * (j / 2) - 1))) + p) % p;
          te[l] = 0;
          for (k = 0; k < log_cl; k++) {
            te[l] |= (tex[l] & (1 << k)) << (2 * k);
            te[l] |= (tey[l] & (1 << k)) << (2 * k + 1);
            te[l] |= (tez[l] & (1 << k)) << (2 * k + 2);
          }
          te[l] = linear_to_tree[te[l]];
          P4EST_ASSERT (te[l] >= 0);
        }
      }
      for (i = 0; i < 8; i++) {
        tcx[i] = ((tx + (((i & 1) == 0) ? -1 : 1)) + m) % m;
        tcy[i] = ((ty + ((((i >> 1) & 1) == 0) ? -1 : 1)) + n) % n;
        tcz[i] = ((tz + (((i >> 2) == 0) ? -1 : 1)) + p) % p;
        tc[i] = 0;
        for (j = 0; j < log_cl; j++) {
          tc[i] |= (tcx[i] & (1 << j)) << (2 * j);
          tc[i] |= (tcy[i] & (1 << j)) << (2 * j + 1);
          tc[i] |= (tcz[i] & (1 << j)) << (2 * j + 2);
        }
        tc[i] = linear_to_tree[tc[i]];
        P4EST_ASSERT (tc[i] >= 0);
      }
      for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
          l = i * 2 + j;
          if (!periodic[i] &&
              ((coord[i] == 0 && j == 0) || (coord[i] == max[i] && j == 1))) {
            tree_to_tree[tj * 6 + l] = tj;
            tree_to_face[tj * 6 + l] = (int8_t) l;
          }
          else {
            tree_to_tree[tj * 6 + l] = tf[l];
            tree_to_face[tj * 6 + l] = (int8_t) (i * 2 + (j ^ 1));
          }
        }
        if (tree_to_edge != NULL) {
          /** dir1, dir2 should be in correct z order */
          dir1 = (i == 0) ? 1 : 0;
          dir2 = (i == 2) ? 1 : 2;
          for (j = 0; j < 4; j++) {
            l = i * 4 + j;
            if ((!periodic[dir1] &&
                 ((coord[dir1] == 0 && (j & 1) == 0) ||
                  (coord[dir1] == max[dir1] && (j & 1) == 1))) ||
                (!periodic[dir2] &&
                 ((coord[dir2] == 0 && (j / 2) == 0) ||
                  (coord[dir2] == max[dir2] && (j / 2) == 1)))) {
              tree_to_edge[tj * 12 + l] = -1;
            }
            else {
              switch (j) {
              case 0:
                ttemp = tree_to_edge2[te[l] * 3 + i];
                break;
              case 1:
                ttemp = tree_to_edge2[tf[dir2 * 2] * 3 + i];
                break;
              case 2:
                ttemp = tree_to_edge2[tf[dir1 * 2] * 3 + i];
                break;
              default:
                ttemp = tree_to_edge2[tj * 3 + i];
              }
              P4EST_ASSERT (ttemp >= 0);
              tree_to_edge[tj * 12 + l] = ttemp;
              edge_to_tree[4 * ttemp + (3 - j)] = tj;
              edge_to_edge[4 * ttemp + (3 - j)] = (int8_t) l;
            }
          }
        }
      }
      for (i = 0; i < 8; i++) {
        if (tree_to_corner != NULL) {
          c[0] = i & 1;
          c[1] = (i >> 1) & 1;
          c[2] = i >> 2;
          if ((!periodic[0] &&
               ((coord[0] == 0 && c[0] == 0) ||
                (coord[0] == max[0] && c[0] == 1))) ||
              (!periodic[1] &&
               ((coord[1] == 0 && c[1] == 0) ||
                (coord[1] == max[1] && c[1] == 1))) ||
              (!periodic[2] &&
               ((coord[2] == 0 && c[2] == 0) ||
                (coord[2] == max[2] && c[2] == 1)))) {
            tree_to_corner[tj * 8 + i] = -1;
          }
          else {
            switch (i) {
            case 0:
              ttemp = tc[0];
              break;
            case 1:
              ttemp = te[0];
              break;
            case 2:
              ttemp = te[4];
              break;
            case 3:
              ttemp = tf[4];
              break;
            case 4:
              ttemp = te[8];
              break;
            case 5:
              ttemp = tf[2];
              break;
            case 6:
              ttemp = tf[0];
              break;
            default:
              ttemp = tj;
              break;
            }
            ttemp = tree_to_corner2[ttemp];
            P4EST_ASSERT (ttemp >= 0);
            tree_to_corner[tj * 8 + i] = ttemp;
            corner_to_tree[ttemp * 8 + (7 - i)] = tj;
            corner_to_corner[ttemp * 8 + (7 - i)] = (int8_t) i;
          }
        }
        if (tz > 0 && (i >> 2) == 0) {
          tree_to_vertex[tj * 8 + i] = tree_to_vertex[tf[4] * 8 + i + 4];
        }
        else if (ty > 0 && ((i >> 1) & 1) == 0) {
          tree_to_vertex[tj * 8 + i] = tree_to_vertex[tf[2] * 8 + i + 2];
        }
        else if (tx > 0 && (i & 1) == 0) {
          tree_to_vertex[tj * 8 + i] = tree_to_vertex[tf[0] * 8 + i + 1];
        }
        else {
          tree_to_vertex[tj * 8 + i] = vcount++;
          vertices[vicount++] = (double) (tx + (i & 1));
          vertices[vicount++] = (double) (ty + ((i >> 1) & 1));
          vertices[vicount++] = (double) (tz + (i >> 2));
        }
      }
    }
  }

  P4EST_ASSERT (vcount == num_vertices);

  P4EST_FREE (linear_to_tree);
  P4EST_FREE (tree_to_corner2);
  P4EST_FREE (tree_to_edge2);

  P4EST_ASSERT (p4est_connectivity_is_valid (conn));

  return conn;
}

p4est_connectivity_t *
p8est_connectivity_new_rotcubes (void)
{
  const p4est_topidx_t num_vertices = 26;
  const p4est_topidx_t num_trees = 6;
  const p4est_topidx_t num_edges = 3;
  const p4est_topidx_t num_corners = 1;
  const double        vertices[26 * 3] = {
    0, 0, 0,
    1, 0, 2,
    2, 0, 0,
    0, 1, 0,
    1, 1, 0,
    2, 1, 0,
    1, -1, 0,
    2, -1, 0,
    1, -1, 1,
    2, -1, 1,
    2, 1, 1,
    1, 0, 1,
    2, 0, 1,
    0, 1, 1,
    1, 1, 1,
    0, 0, 1,
    0, 0, 2,
    1, 0, 0,
    1, 1, 2,
    0, 1, 2,
    2.5, 1.5, 2,
    2, 1.5, 2,
    2, 1.5, 2.5,
    2, .5, 2.5,
    2.5, .5, 2,
    2, .5, 2,
  };
  const p4est_topidx_t tree_to_vertex[6 * 8] = {
    0, 17, 3, 4, 15, 11, 13, 14,
    7, 2, 6, 17, 9, 12, 8, 11,
    2, 12, 5, 10, 17, 11, 4, 14,
    19, 13, 18, 14, 16, 15, 1, 11,
    14, 11, 21, 25, 18, 1, 22, 23,
    21, 20, 25, 24, 14, 10, 11, 12,
  };
  const p4est_topidx_t tree_to_tree[6 * 6] = {
    0, 2, 0, 0, 0, 3,
    1, 2, 1, 1, 1, 1,
    2, 5, 1, 2, 2, 0,
    3, 0, 3, 4, 3, 3,
    4, 4, 3, 4, 5, 4,
    4, 5, 5, 5, 5, 2,
  };
  const int8_t        tree_to_face[6 * 6] = {
    0, 5, 2, 3, 4, 13,
    0, 2, 2, 3, 4, 5,
    0, 23, 1, 3, 4, 1,
    0, 17, 2, 8, 4, 5,
    0, 1, 9, 3, 12, 5,
    16, 1, 2, 3, 4, 19,
  };
  const p4est_topidx_t tree_to_edge[6 * 12] = {
    -1, -1, -1, -1, -1, -1, -1, 0, -1, 2, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, 2,
    -1, -1, 2, -1, -1, -1, -1, 0, -1, 1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,
    0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ett_offset[3 + 1] = { 0, 5, 8, 11 };
  const p4est_topidx_t edge_to_tree[11] = {
    0, 2, 3, 4, 5, 1, 2, 5, 0, 1, 2
  };
  const int8_t        edge_to_edge[11] = {
    7, 7, 23, 12, 18, 7, 9, 15, 9, 11, 2
  };
  const p4est_topidx_t tree_to_corner[6 * 8] = {
    -1, -1, -1, -1, -1, 0, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 0,
    -1, -1, -1, -1, -1, 0, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 0,
    -1, 0, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, 0, -1,
  };
  const p4est_topidx_t ctt_offset[1 + 1] = { 0, 6 };
  const p4est_topidx_t corner_to_tree[6] = { 0, 1, 2, 3, 4, 5 };
  const int8_t        corner_to_corner[6] = { 5, 7, 5, 7, 1, 6 };

  return p4est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      tree_to_corner, ctt_offset,
                                      corner_to_tree, corner_to_corner);
}

p4est_connectivity_t *
p8est_connectivity_new_shell (void)
{
/* *INDENT-OFF* */
  const p4est_topidx_t num_vertices = 18;
  const p4est_topidx_t num_trees =    24;
  const p4est_topidx_t num_edges =    18;
  const p4est_topidx_t num_corners =   0;
  const p4est_topidx_t ctt_offset =    0;
  const double        vertices[18 * 3] = {
    -1, -1,  1,
     0, -1,  1,
     1, -1,  1,
    -1,  0,  1,
     0,  0,  1,
     1,  0,  1,
    -1,  1,  1,
     0,  1,  1,
     1,  1,  1,
    -1, -1,  2,
     0, -1,  2,
     1, -1,  2,
    -1,  0,  2,
     0,  0,  2,
     1,  0,  2,
    -1,  1,  2,
     0,  1,  2,
     1,  1,  2,
  };
  const p4est_topidx_t tree_to_vertex[24 * 8] = {
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
    0, 1, 3, 4,  9, 10, 12, 13,
    1, 2, 4, 5, 10, 11, 13, 14,
    3, 4, 6, 7, 12, 13, 15, 16,
    4, 5, 7, 8, 13, 14, 16, 17,
  };
  const p4est_topidx_t tree_to_tree[24 * 6] = {
    18,  1, 14,  2,  0,  0,
     0, 23, 15,  3,  1,  1,
    16,  3,  0,  4,  2,  2,
     2, 21,  1,  5,  3,  3,
    16,  5,  2,  6,  4,  4,
     4, 21,  3,  7,  5,  5,
    17,  7,  4,  8,  6,  6,
     6, 20,  5,  9,  7,  7,
    17,  9,  6, 10,  8,  8,
     8, 20,  7, 11,  9,  9,
    19, 11,  8, 12, 10, 10,
    10, 22,  9, 13, 11, 11,
    19, 13, 10, 14, 12, 12,
    12, 22, 11, 15, 13, 13,
    18, 15, 12,  0, 14, 14,
    14, 23, 13,  1, 15, 15,
     2, 17,  4, 18, 16, 16,
    16,  8,  6, 19, 17, 17,
     0, 19, 16, 14, 18, 18,
    18, 10, 17, 12, 19, 19,
     9, 21,  7, 22, 20, 20,
    20,  3,  5, 23, 21, 21,
    11, 23, 20, 13, 22, 22,
    22,  1, 21, 15, 23, 23,
  };
  const int8_t        tree_to_face[24 * 6] = {
    6, 0, 3, 2, 4, 5,
    1, 7, 3, 2, 4, 5,
    6, 0, 3, 2, 4, 5,
    1, 7, 3, 2, 4, 5,
    2, 0, 3, 2, 4, 5,
    1, 8, 3, 2, 4, 5,
    2, 0, 3, 2, 4, 5,
    1, 8, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    1, 0, 3, 2, 4, 5,
    9, 0, 3, 2, 4, 5,
    1, 3, 3, 2, 4, 5,
    9, 0, 3, 2, 4, 5,
    1, 3, 3, 2, 4, 5,
    6, 0, 0, 2, 4, 5,
    1, 0, 0, 2, 4, 5,
    6, 0, 3, 6, 4, 5,
    1, 0, 3, 6, 4, 5,
    1, 0, 7, 2, 4, 5,
    1, 7, 7, 2, 4, 5,
    1, 0, 3, 1, 4, 5,
    1, 7, 3, 1, 4, 5,
  };
  const p4est_topidx_t tree_to_edge[24 * 12] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1,  8,  6,  0,
    -1, -1, -1, -1, -1, -1, -1, -1,  8, -1,  0,  7,
    -1, -1, -1, -1, -1, -1, -1, -1,  6,  0, -1,  9,
    -1, -1, -1, -1, -1, -1, -1, -1,  0,  7,  9, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,  9, 10,  1,
    -1, -1, -1, -1, -1, -1, -1, -1,  9, -1,  1, 11,
    -1, -1, -1, -1, -1, -1, -1, -1, 10,  1, -1, 12,
    -1, -1, -1, -1, -1, -1, -1, -1,  1, 11, 12, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13,  2,
    -1, -1, -1, -1, -1, -1, -1, -1, 12, -1,  2, 14,
    -1, -1, -1, -1, -1, -1, -1, -1, 13,  2, -1, 15,
    -1, -1, -1, -1, -1, -1, -1, -1,  2, 14, 15, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 16,  3,
    -1, -1, -1, -1, -1, -1, -1, -1, 15, -1,  3, 17,
    -1, -1, -1, -1, -1, -1, -1, -1, 16,  3, -1,  8,
    -1, -1, -1, -1, -1, -1, -1, -1,  3, 17,  8, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 10,  6,  4,
    -1, -1, -1, -1, -1, -1, -1, -1, 10, -1,  4, 13,
    -1, -1, -1, -1, -1, -1, -1, -1,  6,  4, -1, 16,
    -1, -1, -1, -1, -1, -1, -1, -1,  4, 13, 16, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 14,  5,
    -1, -1, -1, -1, -1, -1, -1, -1, 11, -1,  5 , 7,
    -1, -1, -1, -1, -1, -1, -1, -1, 14,  5, -1, 17,
    -1, -1, -1, -1, -1, -1, -1, -1,  5,  7, 17, -1,
  };
  const p4est_topidx_t ett_offset[18 + 1] = {
     0,  4 , 8, 12, 16, 20, 24, 28, 32,
    36, 40, 44, 48, 52, 56, 60, 64, 68, 72,
  };
  const p4est_topidx_t edge_to_tree[72] = {
     0,  1,  2,  3,
     4,  5,  6,  7,
     8,  9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23,
     0,  2, 16, 18,
     1,  3, 21, 23,
     0,  1, 14, 15,
     2,  3,  4,  5,
     4,  6, 16, 17,
     5,  7, 20, 21,
     6,  7,  8,  9,
     8, 10, 17, 19,
     9, 11, 20, 22,
    10, 11, 12, 13,
    12, 14, 18, 19,
    13, 15, 22, 23,
  };
  const int8_t        edge_to_edge[72] = {
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    11, 10,  9,  8,
    10,  8, 10,  8,
    11,  9, 11,  9,
     9,  8, 11, 10,
    11, 10,  9,  8,
    10,  8,  9,  8,
    11,  9,  9,  8,
    11, 10,  9,  8,
    10,  8, 11,  9,
    11,  9, 10,  8,
    11, 10,  9,  8,
    10,  8, 11, 10,
    11,  9, 11, 10,
  };
/* *INDENT-ON* */

  return p4est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      NULL, &ctt_offset, NULL, NULL);
}

p4est_connectivity_t *
p8est_connectivity_new_sphere (void)
{
/* *INDENT-OFF* */
  const p4est_topidx_t num_vertices = 16;
  const p4est_topidx_t num_trees =    13;
  const p4est_topidx_t num_edges =    12;
  const p4est_topidx_t num_corners =   0;
  const p4est_topidx_t ctt_offset = 0;
  const double        vertices[16 * 3] = {
    -1, -1,  1,
     1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
    -1, -1,  2,
     1, -1,  2,
    -1,  1,  2,
     1,  1,  2,
    -1, -1, -1,
     1, -1, -1,
    -1,  1, -1,
     1,  1, -1,
    -1, -1,  1,
     1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
  };
  const p4est_topidx_t tree_to_vertex[13 * 8] = {
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15,
  };
  const p4est_topidx_t tree_to_tree[13 * 6] = {
     5,  3,  4,  1,  6,  0,
     5,  3,  0,  2,  7,  1,
     5,  3,  1,  4,  8,  2,
     2,  0,  1,  4,  9,  3,
     2,  0,  3,  5, 10,  4,
     2,  0,  4,  1, 11,  5,
    11,  9, 10,  7, 12,  0,
    11,  9,  6,  8, 12,  1,
    11,  9,  7, 10, 12,  2,
     8,  6,  7, 10, 12,  3,
     8,  6,  9, 11, 12,  4,
     8,  6, 10,  7, 12,  5,
    11,  9,  6,  8, 10,  7,
  };
  const int8_t        tree_to_face[13 * 6] = {
     1,  7,  7,  2,  5,  5,
     9,  8,  3,  2,  5,  5,
     6,  0,  3,  6,  5,  5,
     1,  7,  7,  2,  5,  5,
     9,  8,  3,  2,  5,  5,
     6,  0,  3,  6,  5,  5,
     1,  7,  7,  2,  2,  4,
     9,  8,  3,  2,  5,  4,
     6,  0,  3,  6, 15,  4,
     1,  7,  7,  2, 19,  4,
     9,  8,  3,  2, 22,  4,
     6,  0,  3,  6,  6,  4,
    10, 22,  4, 16, 22,  4,
  };
  const p4est_topidx_t tree_to_edge[13 * 12] = {
     0,  2, -1, -1,  8,  9, -1, -1, -1, -1, -1, -1,
     2,  3, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1,
     3,  1, -1, -1, 10, 11, -1, -1, -1, -1, -1, -1,
     7,  5, -1, -1, 11,  9, -1, -1, -1, -1, -1, -1,
     5,  4, -1, -1,  1,  0, -1, -1, -1, -1, -1, -1,
     4,  6, -1, -1, 10,  8, -1, -1, -1, -1, -1, -1,
    -1, -1,  0,  2, -1, -1,  8,  9, -1, -1, -1, -1,
    -1, -1,  2,  3, -1, -1,  6,  7, -1, -1, -1, -1,
    -1, -1,  3,  1, -1, -1, 10, 11, -1, -1, -1, -1,
    -1, -1,  7,  5, -1, -1, 11,  9, -1, -1, -1, -1,
    -1, -1,  5,  4, -1, -1,  1,  0, -1, -1, -1, -1,
    -1, -1,  4,  6, -1, -1, 10,  8, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ett_offset[12 + 1] = {
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
  };
  const p4est_topidx_t edge_to_tree[48] = {
    0,  4,  6, 10,
    2,  4,  8, 10,
    0,  1,  6,  7,
    1,  2,  7,  8,
    4,  5, 10, 11,
    3,  4,  9, 10,
    1,  5,  7, 11,
    1,  3,  7,  9,
    0,  5,  6, 11,
    0,  3,  6,  9,
    2,  5,  8, 11,
    2,  3,  8,  9,
  };
  const int8_t        edge_to_edge[48] = {
     0, 17,  2, 19,
     1, 16,  3, 18,
     1,  0,  3,  2,
     1,  0,  3,  2,
    13, 12, 15, 14,
    13, 12, 15, 14,
     4, 13,  6, 15,
     5, 12,  7, 14,
     4,  5,  6,  7,
     5, 17,  7, 19,
    16,  4, 18,  6,
    17, 16, 19, 18,
  };
#if 0   /* corner information would be redundant */
  const p4est_topidx_t tree_to_corner[13 * 8] = {
     0,  1,  4,  5, -1, -1, -1, -1,
     4,  5,  6,  7, -1, -1, -1, -1,
     6,  7,  2,  3, -1, -1, -1, -1,
     7,  5,  3,  1, -1, -1, -1, -1,
     3,  1,  2,  0, -1, -1, -1, -1,
     2,  0,  6,  4, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  1,  4,  5,
    -1, -1, -1, -1,  4,  5,  6,  7,
    -1, -1, -1, -1,  6,  7,  2,  3,
    -1, -1, -1, -1,  7,  5,  3,  1,
    -1, -1, -1, -1,  3,  1,  2,  0,
    -1, -1, -1, -1,  2,  0,  6,  4,
    -1, -1, -1, -1, -1, -1, -1, -1,
  };
  const p4est_topidx_t ctt_offset[8 + 1] = {
    0, 6, 12, 18, 24, 30, 36, 42, 48,
  };
  const p4est_topidx_t corner_to_tree[48] = {
    0,  4,  5,  6, 10, 11,
    0,  3,  4,  6,  9, 10,
    2,  4,  5,  8, 10, 11,
    2,  3,  4,  8,  9, 10,
    0,  1,  5,  6,  7, 11,
    0,  1,  3,  6,  7,  9,
    1,  2,  5,  7,  8, 11,
    1,  2,  3,  7,  8,  9,
  };
  const int8_t        corner_to_corner[48] = {
    0, 3, 1, 4, 7, 5,
    1, 3, 1, 5, 7, 5,
    2, 2, 0, 6, 6, 4,
    3, 2, 0, 7, 6, 4,
    2, 0, 3, 6, 4, 7,
    3, 1, 1, 7, 5, 5,
    2, 0, 2, 6, 4, 6,
    3, 1, 0, 7, 5, 4,
  };
#endif  /* 0 */
/* *INDENT-ON* */

  return p4est_connectivity_new_copy (num_vertices, num_trees,
                                      num_edges, num_corners,
                                      vertices, tree_to_vertex,
                                      tree_to_tree, tree_to_face,
                                      tree_to_edge, ett_offset,
                                      edge_to_tree, edge_to_edge,
                                      NULL, &ctt_offset, NULL, NULL);
}

void
p8est_find_edge_transform (p4est_connectivity_t * conn,
                           p4est_topidx_t itree, int iedge,
                           p8est_edge_info_t * ei)
{
  int                 i;
  int                 redge, nedge, iflip, nflip;
  int                 pref, pset, fc[2];
  int                 faces[2], nfaces[2], orients[2];
  int                 founds[2], nows[2];
  p4est_topidx_t      edge_trees, etree, ietree;
  p4est_topidx_t      aedge, ntree, ntrees[2];
  p8est_edge_transform_t *et;
  sc_array_t         *ta = &ei->edge_transforms;
  const int           noncorners[2] = { -1, -1 };
  const int          *fcorners[2] = { noncorners, noncorners };
  const int          *nfcorners;
#ifdef P4EST_DEBUG
  int                 flipped = 0;
#endif

  P4EST_ASSERT (0 <= itree && itree < conn->num_trees);
  P4EST_ASSERT (0 <= iedge && iedge < 12);
  P4EST_ASSERT (ta->elem_size == sizeof (p8est_edge_transform_t));

  ei->iedge = (int8_t) iedge;
  sc_array_resize (ta, 0);
  if (conn->num_edges == 0) {
    return;
  }
  aedge = conn->tree_to_edge[12 * itree + iedge];
  if (aedge == -1) {
    return;
  }

  /* identify touching faces */
  for (i = 0; i < 2; ++i) {
    faces[i] = p8est_edge_faces[iedge][i];
    ntrees[i] = conn->tree_to_tree[6 * itree + faces[i]];
    nfaces[i] = (int) conn->tree_to_face[6 * itree + faces[i]];
    if (ntrees[i] == itree && nfaces[i] == faces[i]) {  /* domain boundary */
      ntrees[i] = -1;
      nfaces[i] = orients[i] = -1;
    }
    else {
      orients[i] = nfaces[i] / 6;
      nfaces[i] %= 6;
      fcorners[i] = p8est_edge_face_corners[iedge][faces[i]];
      P4EST_ASSERT (fcorners[i][0] >= 0 && fcorners[i][1] >= 0);
    }
    founds[i] = 0;
  }

  edge_trees =                  /* same type */
    conn->ett_offset[aedge + 1] - conn->ett_offset[aedge];

  /* find orientation of this edge */
  ietree = -1;
  iflip = -1;
  for (etree = 0; etree < edge_trees; ++etree) {
    ntree = conn->edge_to_tree[conn->ett_offset[aedge] + etree];
    redge = (int) conn->edge_to_edge[conn->ett_offset[aedge] + etree];
    P4EST_ASSERT (redge >= 0 && redge < 24);
    nedge = redge % 12;
    if (nedge == iedge && ntree == itree) {
      iflip = redge / 12;
      ietree = etree;
      break;
    }
  }
  P4EST_ASSERT (ietree >= 0 && iflip >= 0);

  /* loop through all trees connected through this edge */
  for (etree = 0; etree < edge_trees; ++etree) {
    if (etree == ietree) {
      continue;
    }
    ntree = conn->edge_to_tree[conn->ett_offset[aedge] + etree];
    redge = (int) conn->edge_to_edge[conn->ett_offset[aedge] + etree];
    P4EST_ASSERT (redge >= 0 && redge < 24);
    nedge = redge % 12;
    nflip = (redge / 12) ^ iflip;

    nows[0] = nows[1] = 0;
    for (i = 0; i < 2; ++i) {
      if (ntree == ntrees[i]) {
        /* check if the edge touches this neighbor contact face */
        nfcorners = p8est_edge_face_corners[nedge][nfaces[i]];
        if (nfcorners[0] >= 0) {
          P4EST_ASSERT (fcorners[i][0] >= 0);
          pref = p8est_face_permutation_refs[faces[i]][nfaces[i]];
          pset = p8est_face_permutation_sets[pref][orients[i]];
          fc[0] = p8est_face_permutations[pset][fcorners[i][0]];
          fc[1] = p8est_face_permutations[pset][fcorners[i][1]];

          if (fc[0] == nfcorners[nflip] && fc[1] == nfcorners[!nflip]) {
            P4EST_ASSERT (!founds[i] && !nows[!i]);
            founds[i] = nows[i] = 1;
          }
#ifdef P4EST_DEBUG
          else if (fc[0] == nfcorners[!nflip] && fc[1] == nfcorners[nflip]) {
            ++flipped;
          }
#endif
        }
      }
    }
    if (nows[0] || nows[1]) {
      continue;
    }

    /* else we have a 1 diagonal edge with ntree */
    et = (p8est_edge_transform_t *) sc_array_push (ta);
    et->ntree = ntree;
    et->nedge = (int8_t) nedge;
    et->naxis[0] = (int8_t) (nedge / 4);
    et->naxis[1] = (int8_t) (nedge < 4 ? 1 : 0);
    et->naxis[2] = (int8_t) (nedge < 8 ? 2 : 1);
    et->nflip = (int8_t) nflip;
    et->corners = (int8_t) (nedge % 4);
  }
  P4EST_ASSERT (edge_trees == (p4est_topidx_t) ta->elem_count
                + 1 + (ntrees[0] != -1) + (ntrees[1] != -1) - flipped);
}