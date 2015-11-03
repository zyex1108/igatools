//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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

/*
 *  Test for the evaluation of physical space basis functions
 *  values and gradients the cylindrical mapping.
 *
 *  author:
 *  date:
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
//#include <igatools/functions/function_lib.h>
//#include <igatools/functions/identity_function.h>
#include <igatools/geometry/grid_function_lib.h>
#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/physical_space_basis.h>
#include <igatools/basis_functions/physical_space_element.h>
#include <igatools/basis_functions/phys_space_element_handler.h>


template<int dim, int codim=0>
auto
create_domain(const shared_ptr<const Grid<dim>> &grid)
{
  /*
  using Function = functions::CylindricalAnnulus<dim>;
  auto map = Function::const_create(grid, IdentityFunction<dim>::const_create(grid),
                              1.0, 2.0, 0.0, 1.0, 0.0, numbers::PI/3.0);
  return map;
  //*/

  using GridFunc = grid_functions::CylindricalAnnulusGridFunction;
  auto grid_func = GridFunc::const_create(grid,
                                          1.0, 2.0, 0.0, 1.0, 0.0, numbers::PI/3.0);

  using Domain = Domain<dim,codim>;
  auto domain = Domain::const_create(grid_func);

  return domain;
}

template <int dim, int order = 0, int range=1, int rank=1, int codim = 0>
void elem_values(const int n_knots = 2, const int deg=1)
{

  const int k = dim;
  using BspSpace = BSpline<dim, range, rank>;
  using Basis = PhysicalSpaceBasis<dim,range,rank,codim>;

  auto grid  = Grid<dim>::const_create(n_knots);

  auto ref_space = BspSpace::const_create(SplineSpace<dim,range,rank>::const_create(deg,grid));
  auto phys_domain = create_domain(grid);

  auto space = Basis::const_create(ref_space, phys_domain, Transformation::h_grad);

  const int n_qp = 2;
  auto quad = QGauss<k>::create(n_qp);
  using Flags = space_element::Flags;
  auto flag = Flags::value |
              Flags::gradient |
              Flags::hessian;

  auto elem_cache_handler = space->create_cache_handler();
  elem_cache_handler->template set_flags<dim>(flag);

  auto elem = space->begin();
  auto end = space->end();
  elem_cache_handler->init_element_cache(elem,quad);

  using Elem = typename Basis::ElementAccessor;
  using _Value = typename Elem::_Value;
  using _Gradient = typename Elem::_Gradient;
  using _Hessian = typename Elem::_Hessian;
  for (; elem != end; ++elem)
  {
    elem_cache_handler->fill_element_cache(elem);

    out.begin_item("Basis values:");
    elem->template get_basis_data<_Value, k>(0,DofProperties::active).print_info(out);
    out.end_item();

    out.begin_item("Basis gradients:");
    elem->template get_basis_data<_Gradient, k>(0,DofProperties::active).print_info(out);
    out.end_item();

    out.begin_item("Basis hessians:");
    elem->template get_basis_data<_Hessian, k>(0,DofProperties::active).print_info(out);
    out.end_item();
  }

  out << endl << endl;
}


int main()
{
  out.depth_console(10);

  elem_values<3>();

  return 0;
}
