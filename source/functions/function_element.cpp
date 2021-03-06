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

#include <igatools/geometry/domain_element.h>
#include <igatools/functions/function.h>
#include <igatools/functions/function_element.h>



IGA_NAMESPACE_OPEN


template<int dim,int codim,int range,int rank>
FunctionElement<dim,codim,range,rank>::
FunctionElement(const std::shared_ptr<ContainerType> &func,
                std::unique_ptr<DomainElem> &&domain_elem)
  :
  func_(func),
  domain_elem_(std::move(domain_elem))
{}


#if 0
template<int dim,int codim,int range,int rank>
FunctionElement<dim,codim,range,rank>::
FunctionElement(const self_t &elem,
                const CopyPolicy &copy_policy)
  :
  func_(elem.func_)
{
  if (copy_policy == CopyPolicy::shallow)
  {
    local_cache_ = elem.local_cache_;
    domain_elem_ = elem.domain_elem_;
  }
  else
  {
    local_cache_ = std::make_shared<AllSubElementsCache<Cache>>(*elem.local_cache_);
    domain_elem_ = std::make_shared<DomainElem>(*elem.domain_elem_,CopyPolicy::deep);
  }
}
#endif


template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_element_values_D0() const
-> const ValueVector<Value>
{
  using _D0 = function_element::template _D<0>;
  return get_values_from_cache<_D0,dim>(0);
}

template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_element_values_D1() const
-> const ValueVector<Derivative<1> >
{
  using _D1 = function_element::template _D<1>;
  return get_values_from_cache<_D1,dim>(0);
}

template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_element_values_D2() const
-> const ValueVector<Derivative<2> >
{
  using _D2 = function_element::template _D<2>;
  return get_values_from_cache<_D2,dim>(0);
}


template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_domain_element() const -> const DomainElem &
{
  return *domain_elem_;
}

template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_domain_element() -> DomainElem &
{
  return *domain_elem_;
}


template<int dim,int codim,int range,int rank>
void
FunctionElement<dim,codim,range,rank>::
operator++()
{
  ++(*domain_elem_);
}

template<int dim,int codim,int range,int rank>
void
FunctionElement<dim,codim,range,rank>::
move_to(const IndexType &elem_id)
{
  domain_elem_->move_to(elem_id);
}

template<int dim,int codim,int range,int rank>
auto
FunctionElement<dim,codim,range,rank>::
get_index() const -> const IndexType &
{
  return domain_elem_->get_index();
}

template<int dim,int codim,int range,int rank>
bool
FunctionElement<dim,codim,range,rank>::
operator==(const self_t &a) const
{
  Assert(func_ == a.func_,
         ExcMessage("The elements cannot be compared because defined with different functions."));
  return (*domain_elem_ == *(a.domain_elem_));
}


template<int dim,int codim,int range,int rank>
bool
FunctionElement<dim,codim,range,rank>::
operator!=(const self_t &a) const
{
  Assert(func_ == a.func_,
         ExcMessage("The elements cannot be compared because defined with different functions."));
  return (*domain_elem_ != *(a.domain_elem_));
}




template<int dim,int codim,int range,int rank>
void
FunctionElement<dim,codim,range,rank>::
print_info(LogStream &out) const
{
  Assert(false, ExcNotImplemented());
}



template<int dim,int codim,int range,int rank>
void
FunctionElement<dim,codim,range,rank>::
print_cache_info(LogStream &out) const
{
  local_cache_.print_info(out);
}

#if 0
#ifdef IGATOOLS_WITH_SERIALIZATION
template<int dim,int codim,int range,int rank>
template<class Archive>
void
FunctionElement<dim,codim,range,rank>::
serialize(Archive &ar, const unsigned int version)
{
  AssertThrow(false,ExcNotImplemented());

  ar &boost::serialization::make_nvp("FunctionElement_base_t",
                                     boost::serialization::base_object<GridElement<dim>>(*this));

  ar &boost::serialization::make_nvp("all_sub_elems_cache_",local_cache_);

  ar &boost::serialization::make_nvp("func_",func_);
  ar &boost::serialization::make_nvp("grid_elem_",grid_elem_);

  ar &boost::serialization::make_nvp("phys_domain_elem_",domain_elem_);
}
#endif // IGATOOLS_WITH_SERIALIZATION
#endif

IGA_NAMESPACE_CLOSE

#include <igatools/functions/function_element.inst>

