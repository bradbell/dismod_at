# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
'''
{xrst_begin plot_rate_fit}
{xrst_spell
   pdf
}

Plot The Rates for a Fit
########################

Syntax
******

| *plot_set* = ``plot_rate_fit`` (
| |tab| *database* , *pdf_file* , *plot_title* , *rate_set*
| )

database
********
This ``str`` is the file name for
an existing dismod_at database that contains the results of a
:ref:`fit<fit_command-name>` .
If there is a :ref:`sample_table-name` it is assumed
it was created using a :ref:`sample_command-name` after the :ref:`fit_command-name` .

pdf_file
********
Is the location where the pdf file containing the plot will be placed.

plot_title
**********
This ``str`` is a title printed at the top of every plot.

rate_set
********
Each element of this ``set`` an ``str`` containing a
:ref:`rate_table@rate_name`
that we are plotting the fit for.

plot_set
********
Each element of this ``set`` is a ``str`` containing
the name of a rate (in *rate_set* ) that was plotted.
If a rate was not plotted, it is constrained to be zero by the rate table.

Example
*******
:ref:`user_plot_rate_fit.py-name` .

{xrst_end plot_rate_fit}
'''
# ----------------------------------------------------------------------------
import sys
import numpy
import math
import dismod_at
from matplotlib import pyplot
import matplotlib.backends.backend_pdf
# ----------------------------------------------------------------------------
def get_parent_node_id(tables) :
   parent_node_id = None
   msg = 'Cannot find parent_node_id or parent_node_name in option table'
   for option_row in tables['option'] :
      if option_row['option_name'] == 'parent_node_id' :
         parent_node_id = int( option_row['option_value'] )
      if option_row['option_name'] == 'parent_node_name' :
         parent_node_name = option_row['option_value']
         msg  = f'Cannot find parent_node_name {parent_node_name} in '
         msg += 'node table'
         for (node_id, node_row) in enumerate(tables['node']) :
            if node_row['node_name'] == parent_node_name :
               parent_node_id = node_id
   if parent_node_id is None :
      assert False, msg
   return parent_node_id
# ----------------------------------------------------------------------------
def check4table(cursor, table_name) :
   cmd     = "SELECT * FROM sqlite_master WHERE type='table' AND name="
   cmd    += "'" + table_name + "';"
   info    = cursor.execute(cmd).fetchall()
   if len(info) == 0 :
      result  = False
   else :
      assert len(info) == 1
      result = True
   return result
# ----------------------------------------------------------------------------
def plot_rate_fit(database, pdf_file, plot_title, rate_set) :
   #
   # connection
   connection = dismod_at.create_connection(
      database, new = False, readonly = True
   )
   #
   # tables
   tables     = dict()
   tables['option'] = dismod_at.get_table_dict(connection, 'option')
   #
   # other_connection, other_input_table_list
   other_connection       = None
   other_database         = None
   other_input_table_list = None
   for row in tables['option'] :
      if row['option_name'] == 'other_database' :
         other_database = row['option_value']
      if row['option_name'] == 'other_input_table' :
         other_input_table = row['option_value']
         other_input_table_list = other_input_table.split(' ')
   if other_database != None and other_input_table_list != None :
      other_connection = dismod_at.create_connection(
         other_database, new = False, readonly = True
      )
   #
   # tables
   for name in [
      'age',
      'fit_var',
      'node',
      'option',
      'rate',
      'smooth',
      'time',
      'var',

   ] :
      if name in other_input_table_list :
         tables[name] = dismod_at.get_table_dict(other_connection, name)
      else :
         tables[name] = dismod_at.get_table_dict(connection, name)
   assert 'sample' not in other_input_table_list
   if check4table(connection, 'sample') :
      tables['sample'] = dismod_at.get_table_dict(connection, 'sample')
   connection.close()
   if other_connection != None :
      other_connection.close()
   #
   # parent_node_id
   parent_node_id = get_parent_node_id(tables)
   #
   # parent_node_name
   parent_node_name = tables['node'][parent_node_id]['node_name']
   #
   # n_var, n_sample
   n_var    = len( tables['var'] )
   if 'sample' in tables :
      assert len( tables['sample'] ) % n_var == 0
      n_sample = int( len( tables['sample'] ) / n_var )
   #
   # plot_set
   plot_set = set()
   for row in tables['rate'] :
      if not row['parent_smooth_id'] is None :
         rate_name = row['rate_name']
         if rate_name in rate_set :
            plot_set.add(rate_name)
   #
   # plot_data
   plot_data = dict()
   #
   # rate_name
   for rate_name in plot_set :
      #
      # rate_id
      rate_id = None
      for (row_id, row) in enumerate(tables['rate']) :
         if row['rate_name'] == rate_name :
            rate_id = row_id
      if rate_id is None :
         msg = f'Cannot find {rate_name} in rate table for '
         msg += database
         assert False, msg
      #
      plot_std = 'sample' in tables and rate_name != 'omega'
      #
      # z_list
      z_list = list()
      #
      # var_id, var_row, is_zero_std
      is_zero_std = True
      for (var_id, var_row) in enumerate(tables['var']) :
         if var_row['var_type'] == 'rate' :
            if var_row['rate_id'] == rate_id :
               if  var_row['node_id'] == parent_node_id :
                  #
                  # age, time, value
                  age_id  = var_row['age_id']
                  time_id = var_row['time_id']
                  age     = tables['age'][age_id]['age']
                  time    = tables['time'][time_id]['time']
                  value   = tables['fit_var'][var_id]['fit_var_value']
                  #
                  # row
                  row = { 'age' : age, 'time' : time, 'value' : value }
                  if plot_std :
                     sumsq = 0.0
                     for k in range(n_sample) :
                        sample_id = k * n_var + var_id
                        sample_value  = \
                           tables['sample'][sample_id]['var_value']
                        sumsq    += (sample_value - value)**2
                     std = numpy.sqrt(sumsq / n_sample)
                     row['std']  = std
                     is_zero_std = is_zero_std and std == 0.0
                  #
                  # z_list
                  z_list.append( row )
      #
      # it std is identically zero, remove it
      if plot_std and is_zero_std :
         for row in z_list :
            del row['std']
      #
      # plot_data
      plot_data[rate_name] = z_list
   #
   # plot_limit
   age_min = min( [ row['age'] for row in tables['age'] ] )
   age_max = max( [ row['age'] for row in tables['age'] ] )
   time_min = min( [ row['time'] for row in tables['time'] ] )
   time_max = max( [ row['time'] for row in tables['time'] ] )
   plot_limit = {
      'age_min'  : age_min,
      'age_max'  : age_max,
      'time_min' : time_min,
      'time_max' : time_max,
   }
   #
   dismod_at.plot_curve(
      pdf_file   = pdf_file,
      plot_title = plot_title,
      plot_limit = plot_limit,
      plot_data  = plot_data,
   )
   return plot_set
