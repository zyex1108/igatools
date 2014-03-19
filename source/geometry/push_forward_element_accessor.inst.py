#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------

# QA (pauletti, Mar 19, 2014): review the transform value how is handle
from init_instantiation_data import *
include_files = ['geometry/push_forward.h',
                 'geometry/cartesian_grid_element_accessor.h',
                 'geometry/mapping_element_accessor.h',
                 '../../source/geometry/grid_forward_iterator.cpp']
data = Instantiation(include_files)
(f, inst) = (data.file_output, data.inst)

output = []
containers = ['ValueTable', 'ValueVector']

# Trasformation for values
for row in unique(inst.all_table + inst.extended_table):
    PF = 'PushForward<Transformation::%s,%d,%d>' %(row.trans_type, row.dim, row.codim)
    push_fwd_elem_acc = 'PushForwardElementAccessor<%s>' %(PF)
    output.append('template class %s ;\n' %(push_fwd_elem_acc) )
    output.append('template class GridForwardIterator<%s> ;\n' %(push_fwd_elem_acc) )
    value_ref  = ("Values<%d,%d,%d>" %(row.dim, row.range, row.rank)) 
    value_phys = ("Values<%d,%d,%d>" %(row.space_dim, row.phys_range, row.phys_rank)) 
    for container in containers:
        v_ref  = 'const %s<%s> &' %(container, value_ref)
        v_phys = '%s<%s> &' %(container, value_phys)
        output.append(
                    'template void %s::' %(push_fwd_elem_acc) +
                    'transform_values<%d,%d,%s,Transformation::%s>' %(row.range, row.rank, container,row.trans_type) +
                    '(%s, %s, void *) const ;\n' %(v_ref, v_phys)
                    )
        output.append(
                    'template void %s::' %(push_fwd_elem_acc) +
                    'transform_face_values<%d,%d,%s,Transformation::%s>' %(row.range, row.rank, container,row.trans_type) +
                    '(const Index, %s, %s, void *) const ;\n' %(v_ref, v_phys)
                    )
        order = 1
        deriv_ref  = ("Derivatives<%d,%d,%d,%d>" %(row.dim, row.range, row.rank, order)) 
        deriv_phys = ("Derivatives<%d,%d,%d,%d>" %(row.space_dim, row.phys_range, row.phys_rank, order)) 
        dv_ref  = 'const %s<%s> &' %(container, deriv_ref)
        dv_phys = '%s<%s> &' %(container, deriv_phys)
        output.append(
            'template void %s::' %(push_fwd_elem_acc) +
            'transform_gradients<%d,%d,%s,Transformation::%s>' %(row.range, row.rank, container,row.trans_type) +
            '(%s,%s,%s, void * ) const;\n' %(v_ref,dv_ref,dv_phys)
        )
        output.append(
            'template void %s::' %(push_fwd_elem_acc) +
            'transform_face_gradients<%d,%d,%s,Transformation::%s>' %(row.range, row.rank, container,row.trans_type) +
            '(const Index, %s,%s,%s, void * ) const;\n' %(v_ref, dv_ref, dv_phys)
        )
 


for s in unique(output):
    f.write(s)


