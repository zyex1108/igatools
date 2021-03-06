//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

/**
 *  @file
 *  @brief Construction of different spline spaces
 *  @author pauletti
 *  @date 2015-08-10
 */

// TODO (pauletti, Dec 26, 2014): make this test dim independent
#include "spline_space_tests_common.h"



void test_1d()
{
  OUTSTART

  const int dim=1;
  using SplineSpace = SplineSpace<dim>;
  using MultiplicityTable = typename SplineSpace::MultiplicityTable;

  auto grid = Grid<dim>::const_create(4);
  typename SplineSpace::DegreeTable deg {{2}};
  auto int_mult = MultiplicityTable({ {{1,3}} });
  auto spline_space = SplineSpace::const_create(deg, grid, int_mult);

  SafeSTLArray<SafeSTLVector<Real>,2> bn_x {{-0.5, 0, 0}, {1.1, 1.2, 1.3}};
  typename SplineSpace::BoundaryKnotsTable bdry_knots { {bn_x} };

  typename SplineSpace::EndBehaviour eb(BasisEndBehaviour::end_knots);
  typename SplineSpace::EndBehaviourTable ebt(eb);

  print_boundary_and_repeated_knots(*spline_space,bdry_knots,ebt);

  OUTEND
}



void test_2d()
{
  OUTSTART

  const int dim=2;
  using SplineSpace = SplineSpace<dim>;
  using MultiplicityTable = typename SplineSpace::MultiplicityTable;
  auto grid = Grid<dim>::const_create({3,5});
  typename SplineSpace::DegreeTable deg {{1,3}};

  auto int_mult = MultiplicityTable({ {{1}, {1,3,1}} });

  auto spline_space = SplineSpace::const_create(deg, grid, int_mult);

  SafeSTLArray<SafeSTLVector<Real>,2> bk_x {{-0.5, 0}, {1.2, 1.3}};
  SafeSTLArray<SafeSTLVector<Real>,2> bk_y {{-0.6,0,0,0}, {1,1.1,1.6, 1.6}};
  typename SplineSpace::BoundaryKnotsTable bdry_knots { {bk_x, bk_y} };

  typename SplineSpace::EndBehaviour eb(BasisEndBehaviour::end_knots);
  typename SplineSpace::EndBehaviourTable ebt(eb);

  print_boundary_and_repeated_knots(*spline_space,bdry_knots,ebt);

  OUTEND
}


void test_3d()
{
  OUTSTART

  const int dim=3;
  using SplineSpace = SplineSpace<dim>;
  using MultiplicityTable = typename SplineSpace::MultiplicityTable;
  auto grid = Grid<dim>::const_create({3,4,5});
  typename SplineSpace::DegreeTable deg {{1,3,0}};
  auto int_mult = MultiplicityTable({ {{1}, {1,3}, {1,1,1}} });

  auto spline_space = SplineSpace::const_create(deg, grid, int_mult);

  SafeSTLArray<SafeSTLVector<Real>,2> bk_x {{-0.5, 0}, {1.2, 1.3}};
  SafeSTLArray<SafeSTLVector<Real>,2> bk_y {{-0.6,0,0,0}, {1,1,1.6, 1.6}};
  SafeSTLArray<SafeSTLVector<Real>,2> bk_z {{-0.6}, {1.6}};
  typename SplineSpace::BoundaryKnotsTable bdry_knots { {bk_x, bk_y, bk_z} };

  typename SplineSpace::EndBehaviour eb(BasisEndBehaviour::end_knots);
  typename SplineSpace::EndBehaviourTable ebt(eb);

  print_boundary_and_repeated_knots(*spline_space,bdry_knots,ebt);

  OUTEND
}


void test_2d_2()
{
  OUTSTART

  const int dim=2;
  const int range=2;
  using SplineSpace = SplineSpace<dim, range, 1>;
  using MultiplicityTable = typename SplineSpace::MultiplicityTable;
  auto grid = Grid<dim>::const_create({3,4});
  typename SplineSpace::DegreeTable deg {{1,3},{3,1}};

  auto int_mult = MultiplicityTable({ {{1}, {1,3}},{{1}, {1,1}}});

  auto spline_space = SplineSpace::const_create(deg, grid, int_mult);

  SafeSTLArray<SafeSTLVector<Real>,2> bk_x {{-0.5, 0}, {1.2, 1.3}};
  SafeSTLArray<SafeSTLVector<Real>,2> bk_y {{-0.6,0,0,0}, {1,1,1.6, 1.6}};

  typename SplineSpace::BoundaryKnotsTable bdry_knots { {bk_x, bk_y}, {bk_y, bk_x} };

  typename SplineSpace::EndBehaviour eb(BasisEndBehaviour::end_knots);
  typename SplineSpace::EndBehaviourTable ebt(eb);

  print_boundary_and_repeated_knots(*spline_space,bdry_knots,ebt);

  OUTEND
}


int main()
{
  out.depth_console(10);

  test_1d();
  test_2d();
  test_3d();

  test_2d_2();

  return 0;
}
