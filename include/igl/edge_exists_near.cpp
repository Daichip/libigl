// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "edge_exists_near.h"

template <
  typename DeriveduE,
  typename DerivedEMAP,
  typename uE2EType,
  typename Index>
IGL_INLINE bool igl::edge_exists_near(
  const Eigen::MatrixBase<DeriveduE> & uE,
  const Eigen::MatrixBase<DerivedEMAP> & EMAP,
  const std::vector<std::vector< uE2EType> > & uE2E,
  const Index & a,
  const Index & b,
  const Index & uei)
{
  std::vector<Index> face_queue;
  face_queue.reserve(32);
  std::vector<Index> pushed;
  // 32 is faster than 8
  pushed.reserve(32);
  assert(a!=b);
  // starting with the (2) faces incident on e, consider all faces
  // incident on edges containing either a or b.
  //
  // face_queue  Queue containing faces incident on exactly one of a/b
  // Using a vector seems mildly faster
  assert(EMAP.size()%3 == 0);
  const Index num_faces = EMAP.size()/3;
  const Index f1 = uE2E[uei][0]%num_faces;
  const Index f2 = uE2E[uei][1]%num_faces;
  // map is faster than unordered_map here, and vector + brute force
  // is_member check is even faster
  face_queue.push_back(f1);
  pushed.push_back(f1);
  face_queue.push_back(f2);
  pushed.push_back(f2);
  while(!face_queue.empty())
  {
    const Index f = face_queue.back();
    face_queue.pop_back();
    // consider each edge of this face
    for(int c = 0;c<3;c++)
    {
      // Unique edge id
      const Index uec = EMAP(c*num_faces+f);
      const Index s = uE(uec,0);
      const Index d = uE(uec,1);
      const bool ona = s == a || d == a;
      const bool onb = s == b || d == b;
      // Is this the edge we're looking for?
      if(ona && onb)
      {
        return true;
      }
      // not incident on either?
      if(!ona && !onb)
      {
        continue;
      }
      // loop over all incident half-edges
      for(const auto & he : uE2E[uec])
      {
        // face of this he
        const Index fhe = he%num_faces;
        bool already_pushed = false;
        for(const auto & fp : pushed)
        {
          if(fp == fhe)
          {
            already_pushed = true;
            break;
          }
        }
        if(!already_pushed)
        {
          pushed.push_back(fhe);
          face_queue.push_back(fhe);
        }
      }
    }
  }
  return false;
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template bool igl::edge_exists_near<Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, int, int>(Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> > const&, std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&, int const&, int const&, int const&);
#endif 