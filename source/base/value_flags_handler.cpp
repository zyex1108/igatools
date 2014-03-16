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


#include <igatools/base/value_flags_handler.h>
#include <igatools/base/exceptions.h>


IGA_NAMESPACE_OPEN


/**
 * Exception used when a ValueFlag is not admissibile for the caller object.
 */
DeclException2(ExcFillFlagNotSupported, ValueFlags, ValueFlags,
               << "The passed ValueFlag " << arg2
               << " contains a non admissible flag " << (arg1 ^arg2));



//====================================================
ValueFlagsHandler::
ValueFlagsHandler()
    :
    fill_none_(true),
    fill_values_(false),
    values_filled_(false),
    fill_gradients_(false),
    gradients_filled_(false),
    fill_hessians_(false),
    hessians_filled_(false)
{}


bool
ValueFlagsHandler::
fill_values() const
{
    return fill_values_;
}
bool

ValueFlagsHandler::
values_filled() const
{
	return values_filled_;
}

void
ValueFlagsHandler::
set_values_filled(const bool status)
{
	values_filled_ = status;
}

bool
ValueFlagsHandler::
fill_gradients() const
{
    return fill_gradients_;
}

bool
ValueFlagsHandler::
gradients_filled() const
{
	return gradients_filled_;
}

void
ValueFlagsHandler::
set_gradients_filled(const bool status)
{
	gradients_filled_ = status;
}

bool
ValueFlagsHandler::
fill_hessians() const
{
    return fill_hessians_;
}

bool
ValueFlagsHandler::
hessians_filled() const
{
	return hessians_filled_;
}

void
ValueFlagsHandler::
set_hessians_filled(const bool status)
{
	hessians_filled_ = status;
}
//====================================================



//====================================================
GridElemValueFlagsHandler::
GridElemValueFlagsHandler()
    :
    fill_points_(false),
    points_filled_(false),
    fill_measures_(false),
    measures_filled_(false),
    fill_w_measures_(false),
    w_measures_filled_(false)
{}


GridElemValueFlagsHandler::
GridElemValueFlagsHandler(const ValueFlags &flags)
{
    if (contains(flags, ValueFlags::point))
        fill_points_ = true;

    if (contains(flags, ValueFlags::measure))
        fill_measures_ = true ;

    if (contains(flags, ValueFlags::w_measure))
    {
        fill_measures_ = true ;
        fill_w_measures_ = true ;
    }
}

bool
GridElemValueFlagsHandler::
fill_points() const
{
    return fill_points_;
}

bool
GridElemValueFlagsHandler::
points_filled() const
{
	return points_filled_;
}

void
GridElemValueFlagsHandler::
set_points_filled(const bool status)
{
	points_filled_ = status;
}

bool
GridElemValueFlagsHandler::
fill_measures() const
{
    return fill_measures_;
}

bool
GridElemValueFlagsHandler::
measures_filled() const
{
	return measures_filled_;
}

void
GridElemValueFlagsHandler::
set_measures_filled(const bool status)
{
	measures_filled_ = status;
}

bool
GridElemValueFlagsHandler::
fill_w_measures() const
{
    return fill_w_measures_;
}

bool
GridElemValueFlagsHandler::
w_measures_filled() const
{
	return w_measures_filled_;
}

void
GridElemValueFlagsHandler::
set_w_measures_filled(const bool status)
{
	w_measures_filled_ = status;
}
//====================================================


//====================================================
GridFaceValueFlagsHandler::
GridFaceValueFlagsHandler()
    :
	GridElemValueFlagsHandler(),
    fill_normals_(false),
    normals_filled_(false)
{}


GridFaceValueFlagsHandler::
GridFaceValueFlagsHandler(const ValueFlags &flags)
{
    if (contains(flags, ValueFlags::face_point))
        fill_points_ = true;

    if (contains(flags, ValueFlags::face_measure))
        fill_measures_ = true ;

    if (contains(flags, ValueFlags::face_w_measure))
    {
        fill_measures_ = true ;
        fill_w_measures_ = true ;
    }

    if (contains(flags, ValueFlags::face_normal))
    {
        fill_normals_ = true ;
    }
}


bool
GridFaceValueFlagsHandler::
fill_normals() const
{
    return fill_normals_;
}

bool
GridFaceValueFlagsHandler::
normals_filled() const
{
	return normals_filled_;
}

void
GridFaceValueFlagsHandler::
set_normals_filled(const bool status)
{
	normals_filled_ = status;
}
//====================================================


//====================================================
MappingValueFlagsHandler::
MappingValueFlagsHandler()
    :
    ValueFlagsHandler(),
    GridElemValueFlagsHandler(),
    fill_inv_gradients_(false),
    inv_gradients_filled_(false),
    fill_inv_hessians_(false),
    inv_hessians_filled_(false)
{}


MappingValueFlagsHandler::
MappingValueFlagsHandler(const ValueFlags &flags)
{
    if (contains(flags, ValueFlags::point) ||
        contains(flags, ValueFlags::map_value))
    {
        fill_points_ = true;
        fill_values_ = true;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_gradient))
    {
        fill_gradients_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_hessian))
    {
        fill_hessians_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_inv_gradient))
    {
        fill_inv_gradients_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_inv_hessian))
    {
        fill_inv_hessians_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::measure))
    {
        Assert(fill_gradients_, ExcNotInitialized());
        fill_measures_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::w_measure))
    {
        Assert(fill_measures_, ExcNotInitialized());
        fill_w_measures_ = true ;
        fill_none_ = false;
    }

//    Assert(fill_none_ == false,ExcMessage("Nothing to be filled."))
}


bool
MappingValueFlagsHandler::
fill_inv_gradients() const
{
    return fill_inv_gradients_;
}

bool
MappingValueFlagsHandler::
inv_gradients_filled() const
{
	return inv_gradients_filled_;
}

void
MappingValueFlagsHandler::
set_inv_gradients_filled(const bool status)
{
	inv_gradients_filled_ = status;
}

bool
MappingValueFlagsHandler::
fill_inv_hessians() const
{
    return fill_inv_hessians_;
}

bool
MappingValueFlagsHandler::
inv_hessians_filled() const
{
	return inv_hessians_filled_;
}

void
MappingValueFlagsHandler::
set_inv_hessians_filled(const bool status)
{
	inv_hessians_filled_ = status;
}
//====================================================





//====================================================
MappingFaceValueFlagsHandler::
MappingFaceValueFlagsHandler()
    :
    MappingValueFlagsHandler(),
    fill_normals_(false),
    normals_filled_(false)
{}


MappingFaceValueFlagsHandler::
MappingFaceValueFlagsHandler(const ValueFlags &flags)
{
    if (contains(flags, ValueFlags::face_point) ||
        contains(flags, ValueFlags::map_face_value))
    {
        fill_points_ = true;
        fill_values_ = true;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_face_gradient))
    {
        fill_gradients_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_face_hessian))
    {
        fill_hessians_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_face_inv_gradient))
    {
        fill_inv_gradients_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::map_face_inv_hessian))
    {
        fill_inv_hessians_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::face_measure))
    {
        Assert(fill_gradients_, ExcNotInitialized());
        fill_measures_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::face_w_measure))
    {
        Assert(fill_measures_, ExcNotInitialized());
        fill_w_measures_ = true ;
        fill_none_ = false;
    }

    if (contains(flags, ValueFlags::face_normal))
    {
        fill_normals_ = true ;
        fill_none_ = false;
    }

//    Assert(fill_none_ == false,ExcMessage("Nothing to be filled."))
}



bool
MappingFaceValueFlagsHandler::
fill_normals() const
{
    return fill_normals_;
}

bool
MappingFaceValueFlagsHandler::
normals_filled() const
{
	return normals_filled_;
}

void
MappingFaceValueFlagsHandler::
set_normals_filled(const bool status)
{
	normals_filled_ = status;
}
//====================================================





IGA_NAMESPACE_CLOSE




