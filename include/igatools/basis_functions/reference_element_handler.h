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

#ifndef REFERENCE_ELEMENT_HANDLER_H_
#define REFERENCE_ELEMENT_HANDLER_H_

#include <igatools/base/config.h>
#include <igatools/base/flags_handler.h>
#include <igatools/base/quadrature.h>

#include <igatools/basis_functions/space_element_handler.h>
#include <igatools/geometry/grid_handler.h>
#include <igatools/basis_functions/reference_space_basis.h>


IGA_NAMESPACE_OPEN

/**
 *
 * @ingroup handlers
 */
template<int dim, int range = 1, int rank = 1>
class ReferenceElementHandler
  :
  public SpaceElementHandler<dim,0,range,rank>
{
private:
  using base_t = SpaceElementHandler<dim,0,range,rank>;
public:
  using Basis = ReferenceSpaceBasis<dim,range,rank>;
  using ElementIterator = typename Basis::ElementIterator;
  using ElementAccessor = typename Basis::ElementAccessor;

  using topology_variant = TopologyVariants<dim>;
  using eval_pts_variant = QuadVariants<dim>;


  using Flags = space_element::Flags;

#if 0
  static std::shared_ptr<ReferenceElementHandler<dim,range,rank> >
  create(const std::shared_ptr<const Basis> &space);
#endif

protected:
  /** @name Constructors */
  ///@{
  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  ReferenceElementHandler() = default;


  ReferenceElementHandler(const std::shared_ptr<const Basis> &space);

  /**
   * Copy constructor. Not allowed to be used.
   */
  ReferenceElementHandler(const ReferenceElementHandler<dim,range,rank> &elem_handler) = delete;

  /**
   * Move constructor. Not allowed to be used.
   */
  ReferenceElementHandler(ReferenceElementHandler<dim,range,rank> &&elem_handler) = delete;

public:

  /**
   * Destructor.
   */
  virtual ~ReferenceElementHandler() = default;

  ///@}

  /**
   * @name Reset functions
   */
  ///@{

#if 0
  virtual void init_cache(SpaceElement<dim,0,range,rank,Transformation::h_grad> &space_elem,
                          const topology_variant &topology) override final;

  virtual void fill_cache(SpaceElement<dim,0,range,rank,Transformation::h_grad> &space_elem,
                          const topology_variant &topology,
                          const int sub_elem_id) override final;
#endif

//    virtual void print_info(LogStream &out) const = 0;

  template <int sub_elem_dim = dim>
  Size get_num_points() const
  {
    return grid_handler_.template get_num_points<sub_elem_dim>();
  }

protected:

#if 0
  virtual void init_ref_elem_cache(ElementAccessor &elem,
                                   const topology_variant &topology)= 0;

  virtual void fill_ref_elem_cache(ElementAccessor &elem,
                                   const topology_variant &topology,
                                   const int sub_elem_id) = 0;
#endif

  GridHandler<dim> grid_handler_;

public:
  /**
   * Returns the const reference of the GridHandler used by the current ReferenceElementHandler.
   * @return
   */
//    const GridHandler<dim> &get_grid_handler() const;


};




template< class Grad, class Div >
void
eval_divergences_from_gradients(const ValueTable<Grad> &gradients, ValueTable<Div> &divergences)
{
  Assert(gradients.get_num_functions() == divergences.get_num_functions(),
         ExcDimensionMismatch(gradients.get_num_functions(),divergences.get_num_functions()));

  Assert(gradients.get_num_points() == divergences.get_num_points(),
         ExcDimensionMismatch(gradients.get_num_points(),divergences.get_num_points()));

  auto div_it = divergences.begin();
  for (const auto &grad : gradients)
  {
    *div_it = trace(grad);
    ++div_it;
  }
}

template< class Grad, class Div >
void
eval_divergences_from_gradients(const ValueVector<Grad> &gradients, ValueVector<Div> &divergences)
{
  Assert(gradients.get_num_points() == divergences.get_num_points(),
         ExcDimensionMismatch(gradients.get_num_points(),divergences.get_num_points()));

  auto div_it = divergences.begin();
  for (const auto &grad : gradients)
  {
    *div_it = trace(grad);
    ++div_it;
  }
}

IGA_NAMESPACE_CLOSE


#endif // REFERENCE_ELEMENT_HANDLER_H_

