//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
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
#include <igatools/base/function_lib.h>
#include <igatools/basis_functions/new_bspline_space.h>
#include <igatools/basis_functions/new_physical_space.h>
#include <igatools/basis_functions/physical_space_element.h>
#include <igatools/basis_functions/space_element_handler.h>


template<int dim, int codim=0>
auto
create_function(shared_ptr<CartesianGrid<dim>> grid)
{
    using Function = functions::CilinderFunction<dim>;
  //  auto map = CylindricalAnnulus::create(grid,1.0,2.0,1.0,numbers::PI/3.0);
    return Function::create(grid);
}

template <int dim, int order = 0, int range=1, int rank=1, int codim = 0>
void elem_values(const int n_knots = 2, const int deg=1)
{
    out << "========== test PhysSpaceElemAccessor on CylindricalAnnulus --- begin =========" << endl;
   const int k = dim;
    using RefSpace = NewBSplineSpace<dim, range, rank>;
    using Space = NewPhysicalSpace<RefSpace, codim, Transformation::h_grad>;
    using ElementHandler = typename Space::ElementHandler;

    auto grid  = CartesianGrid<dim>::create(n_knots);

    auto ref_space = RefSpace::create(deg, grid);
    auto map_func = create_function(grid);

    auto space = Space::create(ref_space, map_func);

    const int n_qp = 2;
    auto quad = QGauss<k>(n_qp);
    auto flag = NewValueFlags::value |
    		NewValueFlags::gradient |
			NewValueFlags::hessian |
			NewValueFlags::point;

    ElementHandler sp_values(space);
    sp_values.template reset<k> (flag, quad);

    auto elem = space->begin();
    auto end = space->end();
    sp_values.template init_cache<k>(elem);

    for (; elem != end; ++elem)
    {
    	sp_values.template fill_cache<k>(elem,0);

    	out << "Basis values: " << endl;
    	elem->template get_values<0, k>().print_info(out);
    	out << endl;

    	out << "Basis gradients: " << endl;
    	elem->template get_values<1, k>().print_info(out);
    	out << endl;

    	out << "Basis hessians: " << endl;
    	elem->template get_values<2, k>().print_info(out);
    }
    out << "========== test PhysSpaceElemAccessor on CylindricalAnnulus --- end   =========" << endl;
    out << endl << endl;
}


int main()
{
    out.depth_console(10);

    elem_values<3>();

    return 0;
}
