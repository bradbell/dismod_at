# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
'''
$begin plot_rate_fit$$
$spell
   pdf
   std
   dismod
   str
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Plot The Rates for a Fit$$

$head Syntax$$
$icode%plot_set% = plot_rate_fit(
   %database%, %pdf_file%, %plot_title%, %rate_set%
)
%$$

$head database$$
This $code str$$ is the file name for
an existing dismod_at database that contains the results of a
$cref/fit/fit_command/$$.
If there is a $cref sample_table$$ it is assumed
it was created using a $cref sample_command$$ after the $cref fit_command$$.

$head pdf_file$$
Is the location where the pdf file containing the plot will be placed.

$head plot_title$$
This $code str$$ is a title printed at the top of every plot.

$head rate_set$$
Each element of this $code set$$ an $code str$$ containing a
$cref/rate_name/rate_table/rate_name/$$
that we are plotting the fit for.

$head plot_set$$
Each element of this $code set$$ is a $code str$$ containing
the name of a rate (in $icode rate_set$$) that was plotted.
If a rate was not plotted, it is constrained to be zero by the rate table.

$head Example$$
$cref user_plot_rate_fit.py$$.

$end
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
   # tables
   new        = False
   connection = dismod_at.create_connection(database, new)
   tables     = dict()
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
      tables[name] = dismod_at.get_table_dict(connection, name)
   if check4table(connection, 'sample') :
      tables['sample'] = dismod_at.get_table_dict(connection, 'sample')
   connection.close()
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
