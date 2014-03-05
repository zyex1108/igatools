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

#TODO: remove ref and phys space variables
#TODO: remove table variable replace by userspaces
#TODO: structure that are not of common use shuld be defined in their
#      respectiy .inst.py file. Ex: tensor_index, value_table, etc

"""@package init_instantiation_data

This module is loaded from all instantiation scripts.
It will read a .txt table created by another script called
generate_instantiation_table.py

Instantiation rational

Instantiation dependencies

The user supplies at configure time the physical spaces that library 
shall be used for.

1) The user physical space require a face physical space

2) Each physical space requires:
   - A ref space
   - A mapping
   - a push-foward
   
3) In igatools each reference space is treated as a special physical space,
   requiring:
   - a mapping of codimenion  0
   - a push-foward of h_grad type
   
"""

# Removes duplicates of a list while keeping the original order
def unique(seq):
   checked = []
   for e in seq:
      if e not in checked:
         checked.append(e)
   return checked

# Object to store a row containig the description of a physical space.
class PhysSpaceTableRow:
   # Constructor of the class.
   def __init__(self, arg_list):
      self.dim        = arg_list[0]
      self.codim      = arg_list[1]
      self.range      = arg_list[2]
      self.rank       = arg_list[3]
      self.trans_type = arg_list[4]
      self.space_dim  = self.dim +  self.codim
      self.phys_range = self.physical_range(self.range,  self.space_dim, self.trans_type)
      self.phys_rank  = self.physical_rank(self.rank)
      return None

   def physical_range(self, ref_range, space_dim, trans_type):
      if trans_type == 'h_grad':
         return ref_range
      if trans_type == 'h_div':
         return space_dim

   def physical_rank(self, ref_rank):
      return ref_rank

class FunctionRow:
   #function dim, range and rank
   def __init__(self, arg_list):
      self.dim        = arg_list[0]
      self.range      = arg_list[1]  
      self.rank       = arg_list[2]
      return None

class MappingRow:
   #mappings dim, codim and space_dim
   def __init__(self, arg_list):
      self.dim        = arg_list[0]
      self.codim      = arg_list[1]  
      self.space_dim  = self.dim + self.codim
      return None

class PForwRow:
   #mappings dim, codim and space_dim
   def __init__(self, arg_list):
      self.dim        = arg_list[0]
      self.codim      = arg_list[1]  
      self.trans_type = arg_list[2]
      return None
   
class RefSpaceRow:
   #mappings dim, codim and space_dim
   def __init__(self, arg_list):
      self.dim        = arg_list[0]
      self.range      = arg_list[1]  
      self.rank       = arg_list[2]
      return None
   
   
class InstantiationInfo:
   """ Stores "tables" with useful entries to be used for instantiations.
   
   This information is generated using a
   physical spaces tables that was genererated at configure time
   by the user.

   """
  
   def __init__(self, filename, max_der_order):
      """The constructor."""
      self.user_table =[] # Spaces that the library is suppussed to be used on
      self.face_table =[] # Spaces that are faces of the user spaces
      self.all_table  =[] #the physical spaces the user provides plus the one that are necesary on top
      self.extended_table=[] #Added the reference spaces extended to physical

      self.function_dims=[] # table of dim, range, rank for functions
      
      self.user_mapping_dims =[] # table of dim codim
      self.mapping_dims =[] # table of dim codim
      
      self.user_ref_sp_dims=[]
      self.face_ref_sp_dims=[]
      self.all_ref_sp_dims=[]
      
    #  self.all_pf_args=[]
      
      self.deriv_order = range(int(max_der_order)+1)
      self.derivatives=[]  # allderivative classes
      self.values=[]
      
      self.domain_dims = [] # list all domain dimensions
      self.user_domain_dims = []
      self.face_domain_dims = []
#----------------------------------------------------------
      #self.UserRefDims=[]   # the list of the dimension  <d,d,d> of all ref spaces
      #self.RefDims=[]       # the list of the dimension  <d,d,d> of all ref spaces
      self.RefSpaces=[]     # all required reference spaces
      self.UserRefSpaces=[]
      self.UserFilteredRefSpaces=[] #iga mapping required ref spaces

      #self.UserMappingDims=[] #list of <dim, codims>
      #self.MappingDims=[] #list of <dim, codims>
      

      #self.PushForwards=[]
      self.UserPhysSpaces=[]
      self.PhysSpaces=[]
      
      self.read_dimensions_file(filename)
      
      self.create_mapping_dims()
      self.create_function_dims()
      self.create_derivatives()
      self.create_ref_dim()
      
      self.create_ref_spaces()
      self.create_PhysSpaces()
    
      self.tensor_indices=[] #list TensorIndex classes
      self.create_tensor_indices()

      self.tensor_sized_containers=[] #list TensorSizedContainer classes
      self.create_tensor_sized_containers()

      self.dynamic_multi_arrays=[] #list DynamicMultiArray classes
      self.create_dynamic_multi_array()

      self.cartesian_product_arrays=[] #list CartesainProductArray classes
      self.create_cartesian_product_array()

      self.tensor_product_arrays=[] #list TensorProductArray classes
      self.create_tensor_product_array()

      self.value_vectors=[] #list ValueVector classes
      self.create_value_vector()

      self.value_tables=[] #list ValueTable classes
      self.create_value_table()

      self.cartesian_product_indexers=[] #list CartesianProductIndexer classes
      self.create_cartesian_product_indexer()

      self.unit_elements=[] #list UnitElement classes
      self.create_unit_element()

      self.multiplicities=[] #list Multiplicity classes
      self.create_multiplicity()

      self.quadratures=[] #list Quadrature classes
      self.create_quadrature()

      return None


   def read_dimensions_file(self, filename):
      '''Reads a text file where each line describes a physical space and
            genereate the main tables '''

      file_input = open(filename, 'r')
      user_spaces=[]
      for i in file_input:
         row = i.strip().split()
         if (len(row) > 0) and (row[0] != '#') :
            user_spaces.append( [int(x) for x in row[0:4]] + row[-1:])
      file_input.close()
    
      
      for row in user_spaces:
         self.user_table.append(PhysSpaceTableRow(row))
         self.all_table.append(PhysSpaceTableRow(row))

      #Add the spaces for the faces     
      face_spaces = unique ([ [sp.dim-1, sp.codim+1, sp.range, sp.rank, sp.trans_type]
                    for sp in self.user_table ] )

      for row in face_spaces:
         self.face_table.append(PhysSpaceTableRow(row))
         self.all_table.append(PhysSpaceTableRow(row))

      self.all_table = unique(self.all_table)
      
      ref_dims = unique( [ [x.dim, 0, x.range, x.rank, 'h_grad'] 
                          for x in self.all_table ] )
      self.extended_table = [PhysSpaceTableRow(row) for row in ref_dims]
     
      self.domain_dims = unique([sp.dim for sp in self.all_table])
      
      
      
      return None



   def create_mapping_dims(self):
      '''Fills mapping_dims with a list of all mappings '''
      dims_list = unique([ [row.dim,  row.codim] for row in self.all_table])
      self.mapping_dims = [MappingRow(row) for row in dims_list]
               
      dims_list = unique([ [row.dim,  row.codim] for row in self.user_table])
      self.user_mapping_dims = [MappingRow(row) for row in dims_list]     
      return None


   
   def create_function_dims(self):
      dims_list=[]
      for row in self.all_table:
         # Add derivative for the reference space and physical space
         dims_list.append((row.dim,  row.range, row.rank))
         dims_list.append((row.space_dim, row.phys_range, row.phys_rank))
         
      for row in self.mapping_dims:
         dims_list.append((row.dim,  row.space_dim, 1))
         dims_list.append((row.space_dim,  row.dim, 1))
         
      for row in unique(dims_list):
          self.function_dims.append(FunctionRow(row))
     
      #print(dims_list)
      return None
     

   def create_ref_spaces(self):
      ''' Creates a list of Reference spaces '''
      ref_dims = unique( [ [x.dim, x.range, x.rank] for x in self.all_table ] )
      self.all_ref_sp_dims = [RefSpaceRow(x) for x in ref_dims]
      

#       self.user_ref_sp_dims = unique( [RefSpaceRow([x.dim, x.range, x.rank])
#                                        for x in self.user_table] )
# 
#       self.face_ref_sp_dims = unique( [RefSpaceRow([x.dim, x.range, x.rank])
#                                        for x in self.face_table] )

     
      RefDims = unique( ['<%d,%d,%d>' % (x.dim, x.range, x.rank)
                                for x in self.all_table] )

      UserRefDims = unique( ['<%d,%d,%d>' % (x.dim, x.range, x.rank)
                                    for x in self.user_table] )


      spaces = ('BSplineSpace', 'NURBSSpace')
      self.RefSpaces = ( ['%s%s' % (sp, dims) for sp in spaces
                            for dims in RefDims] )
      self.UserRefSpaces = ( ['%s%s' % (sp, dims)
                                for sp in spaces
                                for dims in UserRefDims] )

      temp = unique( ['<%d,%d,%d>' % (x.dim, x.range, x.rank)
                        for x in self.user_table if x.dim >= x.range] )
      self.UserFilteredRefSpaces = ( ['%s%s' % (sp, dims)
                                        for sp in spaces
                                        for dims in temp] )

      return None


#     # Mapping<dim, codim>
#    def create_Mappings(self):
#       ''' Creates a list of mappings '''
#       self.MappingDims = unique( ['<%d,%d>' % (x.dim, x.codim)
#                                    for x in self.all_table] )
#      
#       self.MappingDims = unique(self.MappingDims)
#       self.UserMappingDims = unique( ['<%d,%d>' % (x.dim, x.codim)
#                                        for x in self.user_table] )
#       return None



   def create_PhysSpaces(self):
      self.PushForwards = unique(['PushForward<Transformation::%s, %d, %d>'
                                    %(x.trans_type, x.dim, x.codim) for x in self.all_table] )
     
      spaces =('BSplineSpace', 'NURBSSpace')
      self.PhysSpaces = unique( ['PhysicalSpace <' +
                                   '%s<%d,%d,%d>' % (sp, x.dim, x.range, x.rank) +
                                   ', PushForward<Transformation::%s, %d, %d> >'
                                   %(x.trans_type, x.dim, x.codim)
                                   for sp in spaces
                                   for x in self.all_table] )

      self.UserPhysSpaces = unique( ['PhysicalSpace <' +
                                     '%s<%d,%d,%d>' % (sp, x.dim, x.range, x.rank) +
                                     ', PushForward<Transformation::%s, %d, %d> >'
                                     %(x.trans_type, x.dim, x.codim)
                                     for sp in spaces
                                     for x in self.user_table] )


   def create_ref_dim(self):
      self.ref_dom_dims = unique([x.dim for x in self.all_table])
      self.user_ref_dom_dims = unique([x.dim for x in self.user_table])
      self.face_ref_dom_dims = unique([x.dim for x in self.face_table])
      return None

  

   def create_derivatives(self):
      '''Creates a list of the tensor types for the required values and derivatives'''
      mapping_list = [FunctionRow([x.dim, x.space_dim, 1]) for x in self.mapping_dims]
      mapping_list.append(FunctionRow([0, 0, 1])) #todo fix approprietly
      dims_list = self.function_dims + mapping_list
      deriv ='Tensor<dim, order, tensor::covariant, Tensor<range, rank, tensor::contravariant, Tdouble>>'
      value ='Tensor<range, rank, tensor::contravariant, Tdouble>'

      deriv_list=[]
      value_list=[]
      for order in self.deriv_order:
         for dims in dims_list:
            (dim, range, rank) = (dims.dim, dims.range, dims.rank)
            if order == 0:
               (dim, order) = (1,1)
            replace_table = (('order', str(order)), ('dim', str(dim)),('range', str(range)),('rank', str(rank)))
            temp = deriv
            temp_v = value
            for rep in replace_table:
               temp = temp.replace(rep[0], rep[1])
               temp_v = temp_v.replace(rep[0], rep[1])
            deriv_list.append(temp)
            value_list.append(temp_v)
          
      self.derivatives = unique(deriv_list)
      self.values = unique(value_list)
    
      return None
 




##################################
   def create_tensor_indices(self):
      '''Creates a list of the TensorIndex class that needs to be instantiated'''

      C_list=[]

      for row in self.all_table:
         dim_domain = row.dim
         C = 'TensorIndex<%d>' % (dim_domain)
         C_list.append(C)
         

      self.tensor_indices = unique(C_list)
      return None

##################################


##################################
   def create_tensor_sized_containers(self):
      '''Creates a list of the TensorSizedContainer class that needs to be instantiated'''
      C_list=[]
 
      for row in self.all_table:
         dim_domain = row.dim
         C = 'TensorSizedContainer<%d>' % (dim_domain)
         C_list.append(C)
        
 
      self.tensor_sized_containers = unique(C_list)
      return None
##################################


##################################
   def create_dynamic_multi_array(self):
      '''Creates a list of the DynamicMultiArray class that needs to be instantiated'''

      C_list=[]

      types=['Real','Index']
      for row in self.all_table:
         dim = row.dim
         C = 'DynamicMultiArray<TensorIndex<%s>,%s>' % (dim,dim)
         C_list.append(C)
         for t in types:
            C = 'DynamicMultiArray<%s,%s>' % (t,dim)
            C_list.append(C)



      for deriv in self.derivatives + self.values:
         C = 'DynamicMultiArray<%s,2>' % (deriv)
         C_list.append(C)


      self.dynamic_multi_arrays = unique(C_list)
      return None
      ##################################


##################################
   def create_cartesian_product_array(self):
      '''Creates a list of the CartesianProductArray class that needs to be instantiated'''

      C_list=[]
      for row in self.all_table:
         dim = row.dim
         C = 'CartesianProductArray<Real,%s>' % (dim)
         
         C_list.append(C)

      types=['Real*','Index']
      for t in types:
         for row in self.all_table:
            dim = row.dim
            C = 'CartesianProductArray<%s,%s>' % (t,dim)
            C_list.append(C)
          


# The following instantiations are for the cache of basisfucntion in Bspline space
# and the bezier operators
# todo: do we need both index types here?
      matrix = "boost::numeric::ublas::matrix<Real>"
      types = [ matrix, "const %s *" %matrix, "vector<%s>" %matrix, "const vector<%s> *" %matrix]
      for t in types:
          for row in self.all_table:
              dim = row.dim
              C = "CartesianProductArray<%s,%d>" % (t,dim)
              C_list.append(C)


      self.cartesian_product_arrays = unique(C_list)
      return None
      ##################################


##################################
   def create_tensor_product_array(self):
      '''Creates a list of the TensorProductArray class that needs to be instantiated'''

      C_list=[]
      for row in self.all_table:
         dim = row.dim
         C = 'TensorProductArray<%d>' % (dim)
         C_list.append(C)


      self.tensor_product_arrays = unique(C_list)
      return None
##################################


##################################
   def create_value_vector(self):
      '''Creates a list of the ValueVector class that needs to be instantiated'''

      self.value_vectors=['ValueVector<Real>']

      for deriv in self.derivatives + self.values:
         self.value_vectors.append('ValueVector<%s>' % (deriv))

      return None
##################################


##################################
   def create_value_table(self):
      '''Creates a list of the ValueTable class that needs to be instantiated'''

      for deriv in self.derivatives + self.values:
         self.value_tables.append('ValueTable<%s>' % (deriv))

      return None
##################################


##################################
   def create_cartesian_product_indexer(self):
      '''Creates a list of the CartesianProductIndexer class that needs to be instantiated'''

      C_list=[]

      for row in self.all_table:
         dim = row.dim
         C = 'CartesianProductIndexer<%d>' % (dim)
         C_list.append(C)

      self.cartesian_product_indexers = unique(C_list)
      return None
##################################


##################################
   def create_unit_element(self):
      '''Creates a list of the UnitElement class that needs to be instantiated'''

      C_list=[]

      for row in self.all_table:
         dim_domain = row.dim
         C = 'UnitElement<%d>' % (dim_domain)
         C_list.append(C)

      self.unit_elements = unique(C_list)
      return None
##################################


##################################
   def create_multiplicity(self):
      '''Creates a list of the Multiplicity class that needs to be instantiated'''

      C_list=[]

      for row in self.all_table:
         dim_domain = row.dim
         C = 'Multiplicity<%d>' % (dim_domain)
         C_list.append(C)

      self.multiplicities = unique(C_list)
      return None
##################################


##################################
   def create_quadrature(self):
      '''Creates a list of the Quadrature class that needs to be instantiated'''

      C_list=[]

      for row in self.all_table:
         dim_domain = row.dim
         C = 'Quadrature<%d>' % (dim_domain)
         C_list.append(C)

      self.quadratures = unique(C_list)
      return None
##################################


# 
# ##################################
# #todo: the iterators should be instantiated in the accessor instantiation
#    def create_grid_forward_iterator(self):
#       '''Creates a list of the GridForwardIterator class that needs to be instantiated'''
# 
#       C_list=[]
# 
#       for row in self.cartesian_grid_element_accessors:
#          C = 'GridForwardIterator<%s>' % (row)
#          C_list.append(C)
# 
#       for row in self.mapping_element_accessors:
#          C = 'GridForwardIterator<%s>' % (row)
#          C_list.append(C)
# 
#       bs=[]
#       for row in self.all_table:
#          CA = 'BSplineElementAccessor< %d, %d, %d >' % (row.dim,row.range, row.rank)
#          bs.append('template class %s \n' % (CA))
#          CA = 'NURBSElementAccessor< %d, %d, %d >' % (row.dim,row.range, row.rank)
#          bs.append('template class %s \n' % (CA))
#       for row in unique(bs):
#          C = 'GridForwardIterator<%s>' % (row)
#          C_list.append(C)
# 
# 
#       self.grid_forward_iterators = unique(C_list)
# 
# 
# # include_files =['#include <igatools/geometry/cartesian_grid.h>\n',
# #                 '#include <igatools/geometry/cartesian_grid_element_accessor.h>\n',
# #                 '#include <igatools/geometry/mapping.h>\n',
# #                 '#include <igatools/geometry/mapping_lib.h>\n',
# #                 '#include <igatools/geometry/ig_mapping.h>\n',
# #                 '#include <igatools/geometry/mapping_element_accessor.h>\n',
# #                 '#include <igatools/geometry/push_forward_element_accessor.h>\n',
# #                 '#include <igatools/basis_functions/bspline_space.h>\n',
# #                 '#include <igatools/basis_functions/bspline_element_accessor.h>\n',
# #                 '#include <igatools/basis_functions/nurbs_space.h>\n',
# #                 '#include <igatools/basis_functions/nurbs_element_accessor.h>\n',
# #                 '#include <igatools/basis_functions/physical_space.h>\n',
# #                 '#include <igatools/basis_functions/physical_space_element_accessor.h>\n']
# #
# # for include in include_files:
# #     file_output.write(include)
# #
# # file_output.write('IGA_NAMESPACE_OPEN\n')
# #
# # elem_accessor=[];
# #
# # for dim in inst.ref_dom_dims:
# #     elem_accessor.append('CartesianGridElementAccessor<%d>' %dim )
# #
# # for dims in inst.MappingDims:
# #     elem_accessor.append('MappingElementAccessor%s' %dims )
# #
# # ref_spaces = ['BSplineElementAccessor', 'NURBSElementAccessor']
# # for sp in ref_spaces:
# #     for dims in inst.RefDims:
# #         elem_accessor.append('%s%s' %(sp,dims) )
# #
# # for phys_space in inst.PhysSpaces:
# #     elem_accessor.append('PhysicalSpaceElementAccessor<%s>' %phys_space )
# 
# 
#       return None
#         ##################################
# 
# 
# 
# 
# 
# ##################################
#    def create_cartesian_grid_element(self):
#       '''Creates a list of the CartesianGridElement class that needs to be instantiated'''
# 
#       C_list=[]
# 
#       for row in self.all_table:
#          dim_domain = row.dim
#          C = 'CartesianGridElement<%d>' % (dim_domain)
#          C_list.append(C)
# 
#       self.cartesian_grid_elements = unique(C_list)
#       return None
# ##################################
# 
# 
# ##################################
#    def create_cartesian_grid_element_accessor(self):
#       '''Creates a list of the CartesianGridElementAccessors class that needs to be instantiated'''
# 
#       C_list=[]
#       for row in self.all_table:
#          dim_domain = row.dim
#          C = 'CartesianGridElementAccessor<%d>' % (dim_domain)
#          C_list.append(C)
# 
#       self.cartesian_grid_element_accessors = unique(C_list)
#       return None
# #################################
# 
# 
# ##################################
#    def create_grid_wrapper(self):
#       '''Creates a list of the GridWrapper class that needs to be instantiated'''
#       C_list=[]
# 
#       for grid in self.cartesian_grids:
#          C = 'GridWrapper<%s>' % (grid)
#          C_list.append(C)
# 
#       self.grid_wrappers = unique(C_list)
#       return None
# ##################################
# 
# 
# ##################################
#    def create_mapping(self):
#       '''Creates a list of the Mapping class that needs to be instantiated'''
# 
#       C_list=[]
#       for dims in self.MappingDims:
#          C = 'Mapping%s' % (dims)
#          C_list.append(C)
#       self.mappings = unique(C_list)
#       return None
# ##################################
# 
# 
# ##################################
#    def create_identity_mapping(self):
#       '''Creates a list of the IdentityMapping class that needs to be instantiated'''
#         
#       C_list=[]
# 
#       for dims in self.MappingDims:
#          C = 'IdentityMapping%s' % (dims)
#          C_list.append(C)
#         
#       self.identity_mappings = unique(C_list)        
#       return None
# ##################################
# 
# 

# 
# 
# ##################################
#    def create_mapping_element_accessor(self):
#       '''Creates a list of the Mapping class that needs to be instantiated'''
# 
#       C_list=[]
# 
#       for dims in self.MappingDims:
#          C = 'MappingElementAccessor%s' % (dims)
#          C_list.append(C)
# 
#       self.mapping_element_accessors = unique(C_list)
#       return None
# ##################################


def intialize_instantiation():
   """ Main function called at the beginning of all instatiation scripts."""
   #Getting a dictionary or arguments.
   from sys import argv as sysargv
   from os import sep as ossep
   args = dict([arg.split('=') for arg in sysargv[1:]])

   # Reading information from dimensions file.
   inst = InstantiationInfo(args['config_file'], args['max_der_order'])
   #  Some debug information printing
   print_info = False
   if print_info:
      print('dim codim range rank space_dim')
      for x in inst.all_table:
         print (x.dim, x.codim, x.range, x.rank, x.space_dim)
         #    print inst.deriv_order

   # Openning the output file.
   file_output = open(args['out_file'], 'w')
   # Writing the header.
   header = ( '// This file was automatically generated' +
              'from %s \n' % (sysargv[0].split(ossep)[-1]) +
              '// DO NOT edit as it will be overwritten.\n\n')
   file_output.write(header)

   return file_output, inst

