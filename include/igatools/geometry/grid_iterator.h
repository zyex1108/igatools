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

#ifndef __CARTESIAN_GRID_ITERATOR_H_
#define __CARTESIAN_GRID_ITERATOR_H_

#include <igatools/base/config.h>
#include <igatools/base/properties.h>

#include <iterator>
#include <memory>

IGA_NAMESPACE_OPEN

/**
 * @brief Iterator for traversing a collection of objects build over a Grid and
 * having in common a certain property (see Grid documentation).
 *
 * Its purpose is to iterate over the elements of a Grid.
 *
 * Its main features are:
 * - it takes an accessor's type as template parameter;
 * - the property that define the collection of elements on which the iterator will advance with the
 * <tt>++</tt> operator is passed as input argument of the constructor;
 * - can be incremented (using the prefix operator <tt>++</tt>) in order to point to the next element
 * in the container;
 * - when dereferenced (using the dereferencing operator <tt>*</tt> or <tt>-></tt>),
 * it returns an accessor object.
 *
 * It not fulfills all the requirements to be an
 * <a href="http://www.cplusplus.com/reference/iterator/ForwardIterator/">STL forward iterator</a>
 * because:
 * - it is not default constructible;
 * - the postfix operator <tt>++</tt> is not defined.
 *
 * The object pointed to the GridIteratorBase is called <em>accessor</em>
 * and its type is passed as template argument <tt>Element</tt>
 * of the GridIteratorBase.
 *
 * The <em>accessor</em> is an object that can fetch and use data stored in objects that have
 * a "grid-like" structure. The type of the object with this "grid-like" structure,
 * associated to the type <tt>Element</tt> can be retrieved with the type
 * <tt>Element::ContainerType</tt>.
 *
 * Using the accessor, the structure of the "grid-like" classes is hidden
 * from the application program.
 *
 * <h3>Purpose</h3>
 *
 * Iterators are used whenever a loop over all (or some) elements
 * is to be performed. These loops can then be coded like this:
 * @code
   auto elem = grid.begin();
   const auto elem_end = grid.end();
   for (; elem!=end; ++elem)
     if (elem->at_boundary())
        elem->vertex(k);
  @endcode
 * Note the usage of <tt>++elem</tt> instead of <tt>elem++</tt> since this
 * does not involve temporaries and copying. It is recommended to use
 * a fixed value <tt>end</tt> inside the loop instead of
 * <tt>grid.end()</tt>, since the creation and copying of these
 * iterators is rather expensive compared to normal pointers.
 *
 * The same previous loop can be performed or using the C++11 syntax called
 * <em>range-based for loop</em>
 * @code
   for (const auto & elem : grid) // the elem type is: const Element&
     if (elem.at_boundary())
        elem.vertex(k);
  @endcode
 *
 *
 * Iterators are not much slower than operating directly on the data
 * structures, since they perform the loops that you had to handcode
 * yourself anyway. Most iterator and accessor functions are inlined.
 *
 * The main functionality of iterators, resides in the <tt>++</tt> operator.
 * This move the iterator forward
 * just as if it were a pointer into an array.
 * Here, this operation is
 * not so easy, since it may include dealing with a tensor product
 * structure or something else. But this is completely
 * hidden from the user, though you can still create an iterator
 * pointing to an arbitrary element.  Actually, the operation of
 * moving iterator forward is not done in the iterator
 * classes, but rather in the <tt>Element</tt> classes.
 * Since these are passed
 * as template arguments, you can write your own versions here to add
 * more functionality.
 *
 * Furthermore, the iterators described here satisfy the requirement of
 * input and forward iterators as stated by the C++ standard and
 * the STL documentation. It is therefore possible to use the
 * functions from the algorithm section of the C++ standard,
 * e.g. <em>count_if</em>.
 *
 * <h3>Implementation</h3>
 *
 * The iterator class itself does not have much functionality. It only
 * becomes useful when assigned an <tt>Element</tt> (the template
 * parameter), which really does the access to data. An <tt>Element</tt>
 * has to fulfill some requirements:
 *
 * <ul>
 * <li> It must have a type called <tt>ContainerType</tt> representing
 * the type of the "grid-like" container from which the accessor is getting the data.
 *
 * <li> It must have a member named
 * <tt>present_index</tt> storing the address of the element in the
 * <tt>ContainerType</tt> object presently pointed to. These data have to be
 * accessible by all grid iterator.
 *
 * <li> It must have a constructor which takes a <tt>std::unique_ptr<Element></tt> argument
 * representing the accessed container and
 * an <tt>Index</tt> argument, denoting the index within the "grid-like container".
 *
 * <li> It must have void operator <tt>++</tt> that implements the advance of the accessor
 * whitin the container.
 * </ul>
 * Then the iterator is able to do what it is supposed to. All of the
 * necessary functions are implemented in the <tt>Element</tt> base
 * class, but you may write your own version (non-virtual, since we
 * use templates) to add functionality.
 *
 *
 *
 * <h3>Past-the-end iterators</h3>
 *
 * There is a representation of past-the-end-pointers, denoted by special
 * values of the member variable @p present_index:
 * <ul>
 * <li> <tt>present_index != IteratorState::past_the_end</tt>, then the object is valid
 * <li> <tt>present_index == IteratorState::past_the_end</tt>, then the iterator points
 * past the end; in all other cases, the iterator is considered invalid.
 * </ul>
 *
 * @see IteratorState
 *
 *
 * @tparam Element Type of the accessor.
 *
 * @ingroup iterators
 *
 * @author martinelli 2012,2013,2014,2015
 * @author pauletti 2012,2013,2014,2015
 */
template <class Element>
class GridIterator
  : public std::iterator<std::random_access_iterator_tag, Element>
{
public:
  /** Type of the accessor. */
  using AccessorType = Element;

  /** Type of the grid-like container . */
  using ContainerType = typename Element::ContainerType;

  /**
   * Alias for the tensor index.
   */
  using ListIt = typename ContainerType::List::iterator;

  /** @name Constructors & destructor */
  ///@{
protected:
  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  GridIterator() = default;

public:
#if 0
  /**
   * Construct an iterator on the elements (with the specified <tt>property</tt>)
   * over a grid-type container.
   * @note The <tt>index</tt> parameter is used to place the iterator on the specified element.
   * @warning If the <tt>index</tt> refers to an element that has not the given <tt>property</tt>
   * an assertion will be raised (in DEBUG mode).
   */
  GridIterator(const std::shared_ptr<ContainerType> &container,
               const ListIt &index,
               const PropId &property)
    :
    elem_(std::move(container->create_element(index, property)))
  {}
#endif

  /**
   * Construct an iterator using the underlying element pointer.
   * @warning If the <tt>accessor_ptr</tt> refers to an element that has not
   * the given <tt>property</tt>
   * an assertion will be raised (in DEBUG mode).
   */
  GridIterator(std::unique_ptr<Element> &&elem)
    :
    elem_(std::move(elem))
  {
    Assert(elem_ != nullptr,ExcNullPtr());
  }

  /**
   * Copy constructor. Not allowed to be used.
   */
  GridIterator(const GridIterator<Element> &it) = delete;

  /** Move constructor. */
  GridIterator(GridIterator<Element> &&it) = default;

  /** Destructor */
  ~GridIterator() = default ;
  ///@}

  /** @name Assignment operators */
  ///@{
  /**
   * Copy assignment operator. Not allowe dto be used.
   */
  GridIterator<Element> &
  operator=(const GridIterator<Element> &it) = delete;
#if 0
  {
    elem_ = it.elem_;
    return *this;
  }
#endif

  /** Move assignment operator. */
  GridIterator<Element> &
  operator=(GridIterator<Element> &&) = delete;
  ///@}

  /** @name Comparison operators */
  ///@{
  /**
   * Compares for equality.
   * @note Internally uses the equality comparison operator implemented by the Element object.
   */
  bool operator==(const GridIterator &i) const
  {
    return *elem_ == *i.elem_;
  }

  /**
   * Compares for inequality.
   * @note Internally uses the inequality comparison operator implemented by the Element object.
   */
  bool operator!=(const GridIterator &i) const
  {
    return elem_->operator != (*(i.elem_));
  }
  ///@}


  /**
   *  Prefix <tt>++</tt> operator: <tt>++i</tt>. This
   *  operator advances the iterator to
   *  the next element and returns
   *  a reference to <tt>*this</tt>.
   */
  GridIterator<Element> &operator++()
  {
    elem_->operator++();
    return *this;
  }


  /** @name Dereferencing operators */
  ///@{
  /**
   *  Dereferencing operator, returns a
   *  reference to the Element object.
   */
  Element &operator*()
  {
    Assert(this->elem_->has_valid_position(),
           ExcMessage("The element has an invalid position."));
    return *this->elem_;
  }

  /**
   *  Dereferencing operator, returns a
   *  pointer to the Element object.
   */
  Element *operator->()
  {
    Assert(this->elem_->has_valid_position(),
           ExcMessage("The element has an invalid position."));
    return this->elem_.get();
  }

  /**
   *  Dereferencing operator, returns a
   *  const reference to the Element object.
   */
  const Element &operator*() const
  {
    Assert(this->elem_->has_valid_position(),
           ExcMessage("The element has an invalid position."));
    return *this->elem_;
  }

  /**
   *  Dereferencing operator, returns a
   *  pointer to the const Element object.
   */
  const Element *operator->() const
  {
    Assert(this->elem_->has_valid_position(),
           ExcMessage("The element has an invalid position."));
    return this->elem_.get();
  }
  ///@}

protected:
  /**
   * Pointer to the object holding the Real data.
   * @note We use a pointer instead of a reference object because the type Element
   * can be a pure abstract class (and therefore have some virtual functions)
   * that must be resolved at run-time.
   */
  std::unique_ptr<Element> elem_ ;

};



IGA_NAMESPACE_CLOSE

#endif /* __GRID_ITERATOR_H_ */
