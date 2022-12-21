# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin create_database}
# {xrst_spell
#     bool
#     dt
#     len
#     subsmooth
#     tuple
# }
# {xrst_comment_ch #}
#
# Create a Dismod_at Database
# ###########################
#
# Prototype
# *********
# {xrst_literal
#     # BEGIN_PROTOTYPE
#     # END_PROTOTYPE
# }
#
# Purpose
# *******
# This routine makes it easy to create a ``dismod_at`` database
# with all of its :ref:`input-name` tables.
# This is only meant for small example and testing cases and is not efficient.
#
# Primary Key
# ***********
# For each of the lists above, the order of the
# elements in the corresponding table is the same as the corresponding list.
# For example, *age_list* [ *i* ] corresponds to the *i*-th row
# of the ``age`` table which has
# :ref:`database@Primary Key` value *age_id* = *i* .
#
# Name Column
# ***********
# The :ref:`name columns<database@Name Column>` are created with th unique
# constraint; i.e., it will be an error to have the same value appear
# twice in a column *table_name* _ ``name`` in the table
# *table_name* .
#
# file_name
# *********
# is as ``str`` containing the name of the file where the data base
# is stored.
# If this file already exists, it is deleted and a database is created.
#
# age_list
# ********
# is a ``list`` of ``float`` that
# specify age values by indices.
#
# time_list
# *********
# is a ``list`` of ``float`` that
# specify time values by indices.
#
# integrand_table
# ***************
# This is a list of ``dict``
# that define the rows of the :ref:`integrand_table-name` .
# The dictionary *integrand_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     name,str,name for the *i*-th integrand
#     minimum_meas_cv,str,minimum measurement cv for this integrand
#
# The key ``minimum_meas_cv`` is optional.
# If it is not present, ``0.0`` is used for the corresponding value.
#
# node_table
# **********
# This is a list of ``dict``
# that define the rows of the :ref:`node_table-name` .
# The dictionary *node_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     name,str,name for the *i*-th node
#     parent,str,name of parent of the *i*-th node
#
# Note that if the i-th node does not have a parent, the empty string
# should be used for the parent of that node.
#
# subgroup_table
# **************
# This is a list of ``dict``
# that define the rows of the :ref:`subgroup_table-name` .
# The dictionary *node_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     subgroup,str,name for the *i*-th subgroup
#     group,str,name of group that subgroup is in
#
# Backward Compatibility
# ======================
# To get backward compatibility to before the subgroup information was added,
# add the following table to the ``create_database`` call
# (just after the *node_table* ):
#
#     *subgroup_table* = [ { ``'subgroup'`` : ``'world'`` , ``'group'`` : ``'world'``  } ]
#
# No other changes to the ``create_database`` call should be necessary
# (for backward compatibility).
#
# weight_table
# ************
# This is a list of ``dict``
# that define the rows of the :ref:`weight_table-name` and
# :ref:`weight_grid_table-name` .
# The dictionary *weight_table* [ *i* ] has the following:
#
# .. list-table::
#
#     * - Key
#       - Value Type
#       - Description
#     * - name
#       - str
#       - name of *i*-th weighting
#     * - age_id
#       - list of int
#       - indices for age grid
#     * - time_id
#       - list of int
#       - indices for time grid
#     * - fun
#       - function
#       - *w* = *fun* ( *a* , *t* )
#
# The float *w* is the value of this weighting a the corresponding
# float age *a* and float time *t* .
# Note that there is an *i* , *j* such that
# *a* = *age_list* [ *age_id* [ *i* ]] and
# *t* = *time_list* [ *time_id* [ *j* ]] .
#
# covariate_table
# ***************
# This is a list of ``dict``
# that define the rows of the :ref:`covariate_table-name` .
# The dictionary *covariate_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     name,str,name for the *i*-th covariate
#     reference,float,reference value for *i*-th covariate
#     max_difference,float,maximum difference for *i*-th covariate
#
# If *max_difference* is ``None`` , the corresponding table entry
# is null and this corresponds to an infinite maximum difference.
# If *max_difference* does not appear, null is written for the
# corresponding covariate entry.
#
# avgint_table
# ************
# This is a list of ``dict``
# that define the rows of the :ref:`avgint_table-name` .
# The dictionary *avgint_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     integrand,str,integrand for *i*-th data
#     node,str,name of node in graph
#     subgroup,str,name of subgroup
#     weight,str,weighting function name
#     age_lower,float,lower age limit
#     age_upper,float,upper age limit
#     time_lower,float,lower time limit
#     time_lower,float,upper time limit
#     *c_0*,float,value of first covariate
#     ...,...,...
#     *c_J*,float,value of last covariate
#
# subgroup
# ========
# If the ``subgroup`` key is not present, the first subgroup in
# :ref:`create_database@subgroup_table` is used
# and a warning is printed.
#
# weight
# ======
# The weighting function name identifies an
# entry in the :ref:`create_database@weight_table`
# by its *name* . If *weight* is the empty string,
# the constant weighting is used.
#
# covariates
# ==========
# Note that *J* = ``len`` ( *covariate_table* ) ``- 1`` and for
# *j* = 0 , ... , *J* ,
#
#     *c_j* = *covariate_table* [ *j* ][ ``'name'`` ]
#
# We refer to the columns above as the required columns for
# *avgint_table* .
#
# avgint_extra_columns
# ====================
# If a *row* of *option_table* has *row* [ ``'name'`` ]
# equal to ``'avgint_extra_columns'`` , the corresponding
# *row* [ ``'value'`` ]. ``split`` () is the list of extra avgint table columns.
# Otherwise the list of extra avgint table columns is empty.
#
# data_table
# **********
# This is a list of ``dict``
# that define the rows of the :ref:`data_table-name` .
# It has all the columns required for the *avgint_table* .
# In addition, the dictionary *data_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     hold_out,bool,hold out flag
#     density,str,:ref:`density_table@density_name`
#     meas_value,float,measured value
#     meas_std,float,standard deviation
#     eta,float,offset in log-transform
#     nu,float,Student's-t degrees of freedom
#
# meas_std, eta, nu
# =================
# The columns keys ``meas_std`` , ``eta`` , and ``nu``
# are optional. If they are not present, the value ``null`` is used
# for the corresponding row of the data table.
#
# subgroup
# ========
# if the ``subgroup`` key is not present, the first subgroup in
# :ref:`create_database@subgroup_table` is used
# and a warning is printed.
#
# data_extra_columns
# ==================
# If a *row* of *option_table* has *row* [ ``'name'`` ]
# equal to ``'data_extra_columns'`` , the corresponding
# *row* [ ``'value'`` ]. ``split`` () is the list of extra data table columns.
# Otherwise the list of extra data table columns is empty.
#
# prior_table
# ***********
# This is a list of ``dict``
# that define the rows of the :ref:`prior_table-name` .
# The dictionary *prior_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     name,str,name of *i*-th prior
#     lower,float,lower limit
#     upper,float,upper limit
#     std,float,standard deviation
#     density,str,:ref:`density_table@density_name`
#     eta,float,offset in log densities
#     nu,float,degrees of freed in Student densities
#
# The columns keys
# ``lower`` , ``upper`` , ``std`` , ``eta`` , and ``nu``
# are optional. If they are not present, the value ``null`` is used
# for the corresponding row of the prior table.
#
# smooth_table
# ************
# This is a list of ``dict``
# that define the rows of the :ref:`smooth_table-name` and
# :ref:`smooth_grid_table-name` .
# The dictionary *smooth_table* [ *i* ] has the following keys:
#
# name
# ====
# an ``str`` specifying the name used to reference the *i*-th smoothing.
#
# age_id
# ======
# a list of ``int`` specifying the age values for this smoothing
# as indices in :ref:`create_database@age_list` .
#
# time_id
# =======
# a list of ``int`` specifying the time values for this smoothing
# as indices in :ref:`create_database@time_list` .
#
# mulstd_value_prior_name
# =======================
# an ``str`` specifying the prior used for the value multiplier
# for the *i*-th smoothing; see
# :ref:`smooth_table@mulstd_value_prior_id`
# This key is optional and its default value is ``None`` which corresponds
# to ``null`` in the database.
#
# mulstd_dage_prior_name
# ======================
# an ``str`` specifying the prior used for the age difference multiplier
# for the *i*-th smoothing; see
# :ref:`smooth_table@mulstd_dage_prior_id`
# This key is optional and its default value is ``None`` which corresponds
# to ``null`` in the database.
#
# mulstd_dtime_prior_name
# =======================
# an ``str`` specifying the prior used for the time difference multiplier
# for the *i*-th smoothing; see
# :ref:`smooth_table@mulstd_dtime_prior_id`
# This key is optional and its default value is ``None`` which corresponds
# to ``null`` in the database.
#
# fun
# ===
# This is a function with the following syntax:
#
#     ( *v* , *da* , *dt* ) = *fun* ( *a* , *t* )
#
# The ``str`` results *v* , *da* , and *dt*
# are the names for the value prior, age difference prior,
# and time difference prior corresponding to the *i*-th smoothing.
# The value *da* is not used,
# when age *a* = *age_id* [ ``-1`` ] .
# The value *dt* is not used,
# when time *t* = *time_id* [ ``-1`` ] .
# Note that there is an *i* , *j* such that
# *a* = *age_list* [ *age_id* [ *i* ]] and
# *t* = *time_list* [ *time_id* [ *j* ]] .
#
# const_value
# ===========
# The *fun* return value *v* may be a ``float`` .
# In this case, the value of the smoothing, at the corresponding age and time,
# is constrained to be *v* using the
# :ref:`smooth_grid_table@const_value` column in the
# ``smooth_grid`` table.
#
# nslist_table
# ************
# This is a ``dict`` that specifies the
# :ref:`nslist_table-name` and the :ref:`nslist_pair_table-name` .
# For each :ref:`nslist_table@nslist_name` ,
#
#     *nslist_table* [ *nslist_name* ] = [ ( *node_name* , *smooth_name* ), ... ]
#
# Note that each pair above is a python ``tuple`` :
#
# .. list-table::
#
#     * - Variable
#       - Value Type
#       - Description
#     * - nslist_name
#       - str
#       - name of one list of node,smoothing pairs
#     * - node_name
#       - str
#       - name of the node for this pair
#     * - smooth_name
#       - str
#       - name of the smoothing for this pair
#
# rate_table
# **********
# This is a list of ``dict``
# that define the rows of the :ref:`rate_table-name` .
# The dictionary *rate_table* [ *i* ] has the following:
#
# .. list-table::
#
#     * - Key
#       - Value Type
#       - Description
#     * - name
#       - str
#       - pini, iota, rho, chi, or omega
#     * - parent_smooth
#       - str
#       - parent smoothing
#     * - child_smooth
#       - str
#       - a single child smoothing
#     * - child_nslist
#       - str
#       - list of child smoothings
#
# The value ``None`` is used to represent a ``null`` value for
# the parent and child smoothings.
# If a key name does not appear, null is used for the corresponding value.
# If a *name* ; e.g. ``rho`` , does not appear, the value
# null is used for the parent and child smoothings for the corresponding rate.
#
# mulcov_table
# ************
# This is a list of ``dict``
# that define the rows of the :ref:`mulcov_table-name` .
# The dictionary *mulcov_table* [ *i* ] has the following:
#
# .. list-table::
#
#     * - Key
#       - Value Type
#       - Description
#     * - covariate
#       - str
#       - is the covariate column
#     * - type
#       - str
#       - ``rate_value`` , ``meas_value`` , or ``meas_noise``
#     * - effected
#       - str
#       - integrand or rate affected
#     * - group
#       - str
#       - the group that is affected
#     * - smooth
#       - str
#       - smoothing at group level
#     * - subsmooth
#       - str
#       - smoothing at subgroup level
#
# effected
# ========
# If *type* is ``rate_value`` , *effected* is a rate.
# Otherwise it is an integrand.
#
# group
# =====
# If the ``group`` key is not present, the first group in
# :ref:`create_database@subgroup_table` is used.
#
# subsmooth
# =========
# If the ``subsmooth`` key is not present, the value null is used for
# the subgroup smoothing in the corresponding row and a warning is printed.
#
# option_table
# ************
# This is a list of ``dict``
# that define the values
# :ref:`option_table@Table Format@option_name` ,
# :ref:`option_table@Table Format@option_value` in the option table.
# The *i*-th row of the table will have
#
# | |tab| *option_name* = *option_table* [ *i* ][ ``'name'`` ]
# | |tab| *option_value* = *option_table* [ *i* ][ ``'value'`` ]
#
# node_cov_table
# **************
# This is a list of ``dict``
# that define the rows of the :ref:`node_cov_table-name` .
# The dictionary *node_cov_table* [ *i* ] has the following:
#
# .. csv-table::
#     :widths: auto
#
#     Key,Value Type,Description
#     ``'node_id'`` ,      int,identifies the node for the *i*-th row
#     ``'covariate_id'`` , int,identifies the covariate for the *i*-th row
#     ``'age'`` ,          float,age value corresponding to this row
#     ``'time'`` ,         float,time corresponding to this row
#     ``'cov_value'`` ,    float,covariate value for this row
#
# Contents
# ********
# {xrst_toc_table
#    example/table/create_database.py
# }
# Example
# *******
# The file :ref:`create_database.py-name` contains
# and example and test of ``create_database`` .
#
# {xrst_end create_database}
# BEGIN_PROTOTYPE
def create_database(
   file_name,
   age_list,
   time_list,
   integrand_table = list(),
   node_table      = list(),
   subgroup_table  = list(),
   weight_table    = list(),
   covariate_table = list(),
   avgint_table    = list(),
   data_table      = list(),
   prior_table     = list(),
   smooth_table    = list(),
   nslist_table    = list(),
   rate_table      = list(),
   mulcov_table    = list(),
   option_table    = list(),
   node_cov_table  = list(),
) :
# END_PROTOTYPE
   import sys
   import dismod_at
   # ----------------------------------------------------------------------
   # avgint_extra_columns, data_extra_columns
   avgint_extra_columns = list()
   data_extra_columns   = list()
   for row in option_table :
      if row['name'] == 'avgint_extra_columns' :
         avgint_extra_columns = row['value'].split()
      if row['name'] == 'data_extra_columns' :
         data_extra_columns = row['value'].split()
   # ----------------------------------------------------------------------
   # create database
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   # ----------------------------------------------------------------------
   # create age table
   col_name = [ 'age' ]
   col_type = [ 'real' ]
   row_list = []
   for age in age_list :
      row_list.append( [age] )
   tbl_name = 'age'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create time table
   col_name = [ 'time' ]
   col_type = [ 'real' ]
   row_list = []
   for time in time_list :
      row_list.append( [time] )
   tbl_name = 'time'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create integrand table
   col_name = [ 'integrand_name', 'minimum_meas_cv' ]
   col_type = [ 'text',           'real' ]
   row_list = []
   for i in range( len(integrand_table) ) :
      minimum_meas_cv = 0.0
      if 'minimum_meas_cv' in integrand_table[i] :
         minimum_meas_cv = integrand_table[i]['minimum_meas_cv']
      row = [ integrand_table[i]['name'], minimum_meas_cv ]
      row_list.append( row )
   tbl_name = 'integrand'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_integrand_name2id = {}
   for i in range( len(row_list) ) :
      global_integrand_name2id[ row_list[i][0] ] = i
   # ----------------------------------------------------------------------
   # create density table
   col_name = [  'density_name'   ]
   col_type = [  'text'        ]
   row_list = [
      ['uniform'],
      ['gaussian'],
      ['laplace'],
      ['students'],
      ['log_gaussian'],
      ['log_laplace'],
      ['log_students'],
      ['cen_gaussian'],
      ['cen_laplace'],
      ['cen_log_gaussian'],
      ['cen_log_laplace'],
   ]
   tbl_name = 'density'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_density_name2id = {}
   for i in range( len(row_list) ) :
      global_density_name2id[ row_list[i][0] ] = i
   # ----------------------------------------------------------------------
   # create covariate table
   col_name = [ 'covariate_name',  'reference', 'max_difference' ]
   col_type = [ 'text',             'real',     'real'           ]
   row_list = [ ]
   for i in range( len(covariate_table) ) :
      max_difference = None
      if 'max_difference' in covariate_table[i] :
         max_difference = covariate_table[i]['max_difference']
      row       = [
         covariate_table[i]['name'],
         covariate_table[i]['reference'],
         max_difference
      ]
      row_list.append(row)
   tbl_name = 'covariate'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_covariate_name2id = {}
   for i in range( len(covariate_table) ) :
      global_covariate_name2id[ covariate_table[i]['name'] ] = i
   # ----------------------------------------------------------------------
   # create node table
   global_node_name2id = {}
   for i in range( len(node_table) ) :
      global_node_name2id[ node_table[i]['name'] ] = i
   #
   col_name = [ 'node_name', 'parent' ]
   col_type = [ 'text',      'integer'   ]
   row_list = []
   for i in range( len(node_table) ) :
      node   = node_table[i]
      name   = node['name']
      parent = node['parent']
      if parent == '' :
         parent = None
      else :
         parent = global_node_name2id[parent]
      row_list.append( [ name, parent ] )
   tbl_name = 'node'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create subgroup table
   global_subgroup_name2id = {}
   global_group_name2id = {}
   group_id   = 0
   group_name = subgroup_table[0]['group']
   global_group_name2id[group_name] = group_id
   for i in range( len(subgroup_table) ) :
      global_subgroup_name2id[ subgroup_table[i]['subgroup'] ] = i
      if subgroup_table[i]['group'] != group_name :
         group_id   = group_id + 1
         group_name = subgroup_table[i]['group']
         global_group_name2id[group_name] = group_id
   #
   col_name      = [ 'subgroup_name', 'group_id', 'group_name' ]
   col_type      = [ 'text',          'integer',  'text'       ]
   row_list      = []
   for i in range( len(subgroup_table) ) :
      if i == 0 :
         group_id   = 0
         group_name = subgroup_table[0]['group']
      elif subgroup_table[i]['group'] != group_name :
         group_id   = group_id + 1
         group_name = subgroup_table[i]['group']
      subgroup_name   = subgroup_table[i]['subgroup']
      row_list.append( [ subgroup_name, group_id, group_name ] )
   tbl_name = 'subgroup'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create prior table
   col_name = [
   'prior_name', 'lower', 'upper', 'mean', 'std',  'density_id', 'eta', 'nu'
   ]
   col_type = [
   'text', 'real',  'real',  'real', 'real', 'integer',  'real', 'real'
   ]
   row_list = [ ]
   for i in range( len( prior_table ) ) :
      prior         = prior_table[i]
      density_id   = global_density_name2id[ prior['density'] ]
      #
      # columns that have null for default value
      for key in [ 'lower', 'upper', 'std', 'eta', 'nu' ] :
         if not key in prior :
            prior[key] = None
      #
      row  = [
         prior['name'],
         prior['lower'],
         prior['upper'],
         prior['mean'],
         prior['std'],
         density_id,
         prior['eta'],
         prior['nu'],
      ]
      row_list.append( row )
   tbl_name = 'prior'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_prior_name2id = {}
   for i in range( len(row_list) ) :
      global_prior_name2id[ row_list[i][0] ] = i
   # ----------------------------------------------------------------------
   # create weight table
   col_name = [ 'weight_name', 'n_age',   'n_time'   ]
   col_type = [ 'text',        'integer', 'integer'  ]
   row_list = [ ]
   for i in range( len(weight_table) ) :
      weight = weight_table[i]
      name   = weight['name']
      n_age  = len( weight['age_id'] )
      n_time = len( weight['time_id'] )
      row_list.append( [ name, n_age, n_time ] )
   tbl_name = 'weight'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_weight_name2id = {}
   for i in range( len(weight_table) ) :
      global_weight_name2id[ weight_table[i]['name'] ] = i
   # null is used for constant weighting
   global_weight_name2id[ '' ] = None
   # ----------------------------------------------------------------------
   # create weight_grid table
   col_name = [  'weight_id', 'age_id',   'time_id',  'weight' ]
   col_type = [  'integer',   'integer',  'integer',  'real'   ]
   row_list = [ ]
   for i in range( len(weight_table) ) :
      weight  = weight_table[i]
      age_id  = weight['age_id']
      time_id = weight['time_id']
      fun     = weight['fun']
      for j in age_id :
         for k in time_id :
            w = fun(age_list[j], time_list[k])
            row_list.append( [ i, j, k, w] )
   tbl_name = 'weight_grid'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create smooth table
   col_name = [
      'smooth_name',
      'n_age',
      'n_time',
      'mulstd_value_prior_id',
      'mulstd_dage_prior_id',
      'mulstd_dtime_prior_id'
   ]
   col_type = [
      'text',
      'integer',
      'integer',
      'integer',
      'integer',
      'integer'
   ]
   row_list = [ ]
   for i in range( len(smooth_table) ) :
      smooth        = smooth_table[i]
      name          = smooth['name']
      n_age         = len( smooth['age_id'] )
      n_time        = len( smooth['time_id'] )
      #
      prior_id = dict()
      for key in [ 'value', 'dage', 'dtime' ] :
         prior_id[key] = None
         mulstd_key    = 'mulstd_' + key + '_prior_name'
         if mulstd_key in smooth :
            prior_name    = smooth[mulstd_key]
            if prior_name != None :
               prior_id[key] = global_prior_name2id[prior_name]
      #
      row_list.append( [
         name,
         n_age,
         n_time,
         prior_id['value'],
         prior_id['dage'],
         prior_id['dtime'],
      ] )
   tbl_name = 'smooth'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_smooth_name2id = {}
   for i in range( len(smooth_table) ) :
      global_smooth_name2id[ smooth_table[i]['name'] ] = i
   # ----------------------------------------------------------------------
   # create smooth_grid table
   col_name = [
      'smooth_id',
      'age_id',
      'time_id',
      'value_prior_id',
      'dage_prior_id',
      'dtime_prior_id',
      'const_value',
   ]
   col_type = [
      'integer',  # smooth_id
      'integer',  # age_id
      'integer',  # time_id
      'integer',  # value_prior_id
      'integer',  # dage_prior_id
      'integer',  # dtime_prior_id
      'real',     # const_value
   ]
   row_list = [ ]
   for i in range( len(smooth_table) ) :
      smooth  = smooth_table[i]
      age_id  = smooth['age_id']
      time_id = smooth['time_id']
      fun     = smooth['fun']
      max_j   = 0
      for j in age_id :
         if age_list[j] > age_list[max_j] :
            max_j = j
      max_k   = 0
      for k in time_id :
         if time_list[k] > time_list[max_k] :
            max_k = k
      for j in age_id :
         for k in time_id :
            (v,da,dt) = fun(age_list[j], time_list[k])
            #
            if j == max_j :
               da = None
            elif da != None :
               da = global_prior_name2id[da]
            #
            if k == max_k :
               dt = None
            elif dt != None :
               dt = global_prior_name2id[dt]
            #
            const_value = None
            if isinstance(v, float) :
               const_value = v
               v = None
            elif v != None :
               v = global_prior_name2id[v]
            row_list.append( [ i, j, k, v, da, dt, const_value] )
   tbl_name = 'smooth_grid'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create nslist table
   col_name = [ 'nslist_name' ]
   col_type = [ 'text' ]
   row_list = list()
   for nslist_name in nslist_table :
      row_list.append( [ nslist_name ] )
   tbl_name = 'nslist'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   global_nslist_name2id = dict()
   for i in range( len( row_list ) ) :
      global_nslist_name2id[ row_list[i][0] ] = i
   # ----------------------------------------------------------------------
   # create nslist_pair table
   col_name = [ 'nslist_id', 'node_id', 'smooth_id' ]
   col_type = [ 'integer',   'integer', 'integer'   ]
   row_list = list()
   tbl_name = 'nslist_pair'
   for key in nslist_table :
      pair_list = nslist_table[key]
      nslist_id = global_nslist_name2id[key]
      for pair in pair_list :
         node_id   = global_node_name2id[ pair[0] ]
         smooth_id = global_smooth_name2id[ pair[1] ]
         row_list.append( [ nslist_id, node_id, smooth_id ] )
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create rate table
   col_name = [
      'rate_name', 'parent_smooth_id', 'child_smooth_id', 'child_nslist_id'
   ]
   col_type = [
      'text',      'integer',         'integer',           'integer'
   ]
   row_list = list()
   for rate_name in [ 'pini', 'iota', 'rho', 'chi', 'omega' ] :
      row = [ rate_name, None, None, None ]
      for i in range( len(rate_table) ) :
         rate = rate_table[i]
         if rate['name'] == rate_name :
            row = [ rate_name ]
            for key in ['parent_smooth', 'child_smooth', 'child_nslist'] :
               entry  = None
               if key in rate :
                  entry = rate[key]
               if entry != None :
                  if key == 'child_nslist' :
                     entry = global_nslist_name2id[ entry ]
                  else :
                     entry = global_smooth_name2id[ entry ]
               row.append( entry )
      row_list.append( row )
   tbl_name = 'rate'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   global_rate_name2id = {}
   for i in range( len(row_list) ) :
      global_rate_name2id[ row_list[i][0] ] = i
   # ----------------------------------------------------------------------
   # create mulcov table
   col_name = [
      'mulcov_type',
      'rate_id',
      'integrand_id',
      'covariate_id',
      'group_id',
      'group_smooth_id',
      'subgroup_smooth_id',
   ]
   col_type = [
      'text',    # mulcov_type
      'integer', # rate_id
      'integer', # integrand_id
      'integer', # covariate_id
      'integer', # group_id
      'integer', # group_smooth_id
      'integer', # subgroup_smooth_id
   ]
   row_list = []
   warning_printed = False
   for i in range( len(mulcov_table) ) :
      mulcov       = mulcov_table[i]
      mulcov_type  = mulcov['type']
      effected     = mulcov['effected']
      covariate_id = global_covariate_name2id[ mulcov['covariate'] ]
      #
      # rate_id and integrand_id
      if mulcov_type == 'rate_value' :
         rate_id      = global_rate_name2id[ effected ]
         integrand_id = None
      else :
         integrand_id = global_integrand_name2id[ effected ]
         rate_id      = None
      #
      # group_id
      if 'group' in mulcov :
         group_id = global_group_name2id[ mulcov['group'] ]
      else :
         group_id = 0
         if not warning_printed :
            msg  = 'create_database Warning: '
            msg += 'group key missing in mulcov table,\n'
            msg += 'using default value; i.e., first group '
            msg += '(you should fix this).'
            print(msg)
            warning_printed = True
      #
      # group_smooth_id
      if mulcov['smooth'] == None :
         group_smooth_id = None
      else :
         group_smooth_id    = global_smooth_name2id[ mulcov['smooth'] ]
      #
      # subgroup_smooth_id
      if not 'subsmooth' in mulcov :
         subgroup_smooth_id = None
      elif mulcov['subsmooth'] == None :
         subgroup_smooth_id = None
      else :
         subgroup_smooth_id = global_smooth_name2id[ mulcov['subsmooth'] ]
      #
      row_list.append( [
         mulcov_type,
         rate_id,
         integrand_id,
         covariate_id,
         group_id,
         group_smooth_id,
         subgroup_smooth_id,
      ] )
   tbl_name = 'mulcov'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # avgint table
   #
   # extra_name, extra_type
   extra_name = []
   extra_type = []
   if( len( avgint_table ) > 0 ) :
      extra_name = avgint_extra_columns
      row        = avgint_table[0]
      for key in extra_name :
         if isinstance(row[key], str) :
            extra_type.append('text')
         elif isinstance(row[key], int) :
            extra_type.append('integer')
         elif isinstance(row[key], float) :
            extra_type.append('real')
         else :
            msg  = 'db2csv_command: avgint_extra_columns: type error:'
            msg += '\nThe type for column ' + key
            msg += ' is not str, int, or float'
            assert False, msg
   #
   # col_name
   col_name = extra_name + [
      'integrand_id',
      'node_id',
      'subgroup_id',
      'weight_id',
      'age_lower',
      'age_upper',
      'time_lower',
      'time_upper'
   ]
   for j in range( len(covariate_table) ) :
      col_name.append( 'x_%s' % j )
   #
   # col_type
   col_type = extra_type + [
      'integer',              # integrand_id
      'integer',              # node_id
      'integer',              # subgroup_id
      'integer',              # weight_id
      'real',                 # age_lower
      'real',                 # age_upper
      'real',                 # time_lower
      'real'                  # time_upper
   ]
   for j in range( len(covariate_table) )  :
      col_type.append( 'real' )
   #
   # row_list
   row_list = [ ]
   warning_printed = False;
   for i in range( len(avgint_table) ) :
      avgint = avgint_table[i]
      #
      # subgroup column has a default value
      if 'subgroup' not in avgint :
         avgint['subgroup'] = subgroup_table[0]['subgroup']
         if not warning_printed :
            msg  = 'create_database Warning: '
            msg += 'subgroup key missing in avgint table,\n'
            msg += 'using default value; i.e., first subgroup '
            msg += '(you should fix this).'
            print(msg)
            warning_printed = True
      #
      # extra columns first
      row = list()
      for name in extra_name :
         row.append( avgint[ name ] )
      #
      avgint_id      = i
      integrand_id = global_integrand_name2id[ avgint['integrand'] ]
      node_id      = global_node_name2id[ avgint['node'] ]
      subgroup_id  = global_subgroup_name2id[ avgint['subgroup'] ]
      weight_id    = global_weight_name2id[ avgint['weight'] ]
      row = row + [
         integrand_id,
         node_id,
         subgroup_id,
         weight_id,
         avgint['age_lower'],
         avgint['age_upper'],
         avgint['time_lower'],
         avgint['time_upper']
      ]
      for j in range( len(covariate_table) ) :
         row.append( avgint[ covariate_table[j]['name'] ] )
      row_list.append(row)
   tbl_name = 'avgint'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create data table
   #
   #
   # extra_name, extra_type
   extra_name = []
   extra_type = []
   if( len( data_table ) > 0 ) :
      extra_name = data_extra_columns
      row        = data_table[0]
      for key in extra_name :
         if isinstance(row[key], str) :
            extra_type.append('text')
         elif isinstance(row[key], int) :
            extra_type.append('integer')
         elif isinstance(row[key], float) :
            extra_type.append('real')
         else :
            msg  = 'db2csv_command: data_extra_columns: type error'
            msg += '\nThe type for column ' + key
            msg += ' is not str, int, or float'
            assert False, msg
   #
   # col_name
   col_name = extra_name + [
      'integrand_id',
      'node_id',
      'subgroup_id',
      'weight_id',
      'age_lower',
      'age_upper',
      'time_lower',
      'time_upper',
      'hold_out',
      'density_id',
      'meas_value',
      'meas_std',
      'eta',
      'nu',
   ]
   for j in range( len(covariate_table) ) :
      col_name.append( 'x_%s' % j )
   #
   # col_type
   col_type = extra_type + [
      'integer',              # integrand_id
      'integer',              # node_id
      'integer',              # subgroup_id
      'integer',              # weight_id
      'real',                 # age_lower
      'real',                 # age_upper
      'real',                 # time_lower
      'real',                 # time_upper
      'integer',              # hold_out
      'integer',              # density_id
      'real',                 # meas_value
      'real',                 # meas_std
      'real',                 # eta
      'real',                 # nu
   ]
   for j in range( len(covariate_table) )  :
      col_type.append( 'real' )
   row_list = [ ]
   warning_printed = False
   for i in range( len(data_table) ) :
      data         = data_table[i]
      #
      # extra columns first
      row = list()
      for name in extra_name :
         row.append( data[name] )
      #
      # columns that have null for default value
      for key in [ 'meas_std', 'eta', 'nu' ] :
         if not key in data :
            data[key] = None
      #
      # subgroup column has a default value
      if not 'subgroup' in data :
         data['subgroup'] = subgroup_table[0]['subgroup']
         if not warning_printed :
            msg  = 'create_database Warning: '
            msg += 'subgroup key missing in data table,\n'
            msg += 'using default value; i.e., first subgroup '
            msg += '(you should fix this).'
            print(msg)
            warning_printed = True
      #
      integrand_id = global_integrand_name2id[ data['integrand'] ]
      density_id   = global_density_name2id[ data['density'] ]
      node_id      = global_node_name2id[ data['node'] ]
      subgroup_id  = global_subgroup_name2id[ data['subgroup'] ]
      weight_id    = global_weight_name2id[ data['weight'] ]
      hold_out     = int( data['hold_out'] )
      row = row + [
         integrand_id,
         node_id,
         subgroup_id,
         weight_id,
         data['age_lower'],
         data['age_upper'],
         data['time_lower'],
         data['time_upper'],
         hold_out,
         density_id,
         data['meas_value'],
         data['meas_std'],
         data['eta'],
         data['nu']
      ]
      for j in range( len(covariate_table) ) :
         row.append( data[ covariate_table[j]['name'] ] )
      row_list.append(row)
   tbl_name = 'data'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create option table
   col_name = [ 'option_name', 'option_value' ]
   col_type = [ 'text unique', 'text' ]
   row_list = []
   for row in option_table :
      name  = row['name']
      value = row['value']
      row_list.append( [ name, value ] )
   tbl_name = 'option'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # create node_cov table
   col_name = [ 'node_id', 'covariate_id', 'weight_id' ]
   col_type = [ 'integer', 'integer',      'integer'   ]
   row_list = []
   for row_in in node_cov_table :
      row_out = list()
      for key in col_name :
         row_out.append( row_in[key] )
      row_list.append( row_out )
   tbl_name = 'node_cov'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # close the connection
   connection.close()
   return
