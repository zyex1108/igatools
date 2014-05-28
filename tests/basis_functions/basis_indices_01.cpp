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
 *  Test for basis index
 *  author: pauletti
 *  date:
 *
 */

#include "../tests.h"
#include <igatools/basis_functions/space_spec.h>
#include <igatools/geometry/cartesian_grid_element_accessor.h>


/**
 * Class to manage the basis indices distribution
 * It provides an implementation of the local to global
 * basis indices.
 */
template<int dim, int range = 1, int rank = 1>
class BasisIndex
{
public:
    using Space = SplineSpace<dim, range, rank>;
    using MultiplicityTable = typename Space::MultiplicityTable;
    using SpaceDimensionTable = typename Space::SpaceDimensionTable;

    enum class DistributionPolicy
    {
        standard, component, other
    };

    BasisIndex(std::shared_ptr<CartesianGrid<dim> > grid,
               MultiplicityTable &accum_mult,
               const SpaceDimensionTable &n_basis,
               const SpaceDimensionTable &n_elem_basis,
               DistributionPolicy pol = DistributionPolicy::standard)
    :
    	element_loc_to_global_(grid->get_num_elements_dim())
    {
        Assert(pol == DistributionPolicy::standard, ExcNotImplemented());

        // fills the standard distribution, sorted by component and
        // by direction x moves faster
        for (int comp = 0, j = 0; comp < Space::n_components; ++comp)
        {
            index_distribution_(comp).resize(n_basis(comp));
            for (auto &x : index_distribution_(comp))
                x = j++;
        }


        for (const auto elem : *grid)
        {
            const auto index = elem.get_tensor_index();
            auto &basis_list = element_loc_to_global_(index);
            auto basis = basis_list.begin();

            for (int comp = 0; comp < Space::n_components; ++comp)
            {
                auto origin = accum_mult(comp).cartesian_product(index);
                auto increment = n_elem_basis(comp);

                auto comp_dofs = index_distribution_(comp).get_flat_view(origin, increment);
                element_loc_to_global_(index).insert
                        (basis, comp_dofs.begin(), comp_dofs.end());
                for (auto x : element_loc_to_global_)
                basis = element_loc_to_global_(index).end();
            }
        }

    }


    const std::vector<Index> &get_loc_to_global_indices(const TensorIndex<dim> &j) const
        {
        return element_loc_to_global_(j);
        }

    /**
     * Print the class content
     */
    void print_info(LogStream &out) const
    {
        for (int comp = 0; comp < Space::n_components; ++comp)
            index_distribution_(comp).print_info(out);
        out << endl;

        for (auto x : element_loc_to_global_)
            out << x << endl;

    }

private:
    using IndexDistributionTable =
            typename Space::template ComponentContainer<DynamicMultiArray<Index,dim>>;
    IndexDistributionTable index_distribution_;

    DynamicMultiArray<std::vector<Index>, dim> element_loc_to_global_;

};



int main()
{
    out.depth_console(10);

    {
        const int dim=1;
        using SplineSpace = SplineSpace<dim>;
        using MultiplicityTable = typename SplineSpace::MultiplicityTable;

        typename SplineSpace::DegreeTable deg{{2}};

        auto grid = CartesianGrid<dim>::create(4);

        auto int_mult = shared_ptr<MultiplicityTable>(new MultiplicityTable ({ {{1,3}} }));
        SplineSpace sp_spec(grid, int_mult, deg);

        CartesianProductArray<Real,2> bn_x{{-0.5, 0, 0}, {1.1, 1.2, 1.3}};
        typename SplineSpace::BoundaryKnotsTable bdry_knots{ {bn_x} };
        auto rep_knots = sp_spec.compute_knots_with_repetition(bdry_knots);
        auto acum_mult = sp_spec.accumulated_interior_multiplicities();

        auto n_basis = sp_spec.get_num_basis_table();
        auto n_basis_elem = sp_spec.get_num_basis_per_element_table();

        BasisIndex<dim> basis_index(grid, acum_mult, n_basis, n_basis_elem);
        basis_index.print_info(out);
    }

    {
        const int dim=2;
        using SplineSpace = SplineSpace<dim>;
        using MultiplicityTable = typename SplineSpace::MultiplicityTable;

        typename SplineSpace::DegreeTable deg{{1,2}};

        auto grid = CartesianGrid<dim>::create({4,3});

        SplineSpace sp_spec(grid, SplineSpace::InteriorReg::maximum, deg);


        auto rep_knots = sp_spec.compute_knots_with_repetition(SplineSpace::EndBehaviour::interpolatory);
        auto acum_mult = sp_spec.accumulated_interior_multiplicities();

        auto n_basis = sp_spec.get_num_basis_table();
        auto n_basis_elem = sp_spec.get_num_basis_per_element_table();

        BasisIndex<dim> basis_index(grid, acum_mult, n_basis, n_basis_elem);
        basis_index.print_info(out);
    }

    return 0;
}
