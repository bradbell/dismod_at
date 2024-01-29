# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin perturb_command}
# {xrst_spell
#     tbl
# }
# {xrst_comment_ch #}
#
# Perturb Command: Random Change to Start or Scale Tables
# #######################################################
#
# Syntax
# ******
#
# As Program
# ==========
# ``dismodat.py`` *database* ``perturb`` *tbl_name* *sigma*
#
# As Python Function
# ==================
# ``dismod_at.perturb_command`` ( *database* , *tbl_name* , *sigma* )
#
# database
# ********
# is the path from the currently directory to the database.
# This must be a :ref:`dismod_at<database-name>` and
# the :ref:`init_command-name` must have been run on the database.
#
# tbl_name
# ********
# This is the name of an input table that we are perturbing.
# This must be one of the following choices:
#
# scale_var
# =========
# In this case the :ref:`scale_var_table-name` is perturbed.
# This can be useful when one starts near the solution where the
# derivative of the objective may be small.
#
# start_var
# =========
# In this case the :ref:`start_var_table-name` is perturbed.
# This can be useful when one wants to see if the starting point
# has an effect on the solution.
#
# sigma
# *****
# We use *multiplier* for a randomly chose multiplier,
# that is different for each table entry, and that has a log with
# mean zero and standard deviation *sigma* .
# The value chosen for each table entry is its original value
# times the multiplier.
# If this value is below (above) the lower (upper) limit
# for the table entry the lower (upper) limit is chosen.
#
# Random Seed
# ===========
# If the :ref:`option_table@random_seed` is non-zero,
# it is used to seed the random number generator that is used.
# Otherwise, the system clock is used to seed the random number generator.
#
# {xrst_end perturb_command}
# -----------------------------------------------------------------------------
import dismod_at
import random
import math
import os
# -----------------------------------------------------------------------------
def get_limit_var_table(database) :
   #
   # connection
   connection = dismod_at.create_connection(
      database, new = False, readonly = True
   )
   #
   # table['option']
   table = dict()
   table['option'] = dismod_at.get_table_dict(connection, 'option')
   #
   # other_connection, other_input_table_list
   other_connection       = None
   other_database         = None
   other_input_table_list = list()
   for row in table['option'] :
      if row['option_name'] == 'other_database' :
         other_database = row['option_value']
      if row['option_name'] == 'other_input_table' :
         other_input_table = row['option_value']
         other_input_table_list = other_input_table.split(' ')
   if other_database != None :
      if not os.path.isabs( other_database ) :
         database_dir   = os.path.dirname( database )
         other_database = os.path.join( database_dir, other_database )
      other_connection = dismod_at.create_connection(
         other_database, new = False, readonly = True
      )
   #
   for tbl_name in [
      'node',
      'option',
      'prior',
      'smooth',
      'smooth_grid',
      'var',
   ] :
      if tbl_name in other_input_table_list :
         table[tbl_name] = dismod_at.get_table_dict(other_connection, tbl_name)
      else :
         table[tbl_name] = dismod_at.get_table_dict(connection, tbl_name)
   connection.close()
   if other_connection != None :
      other_connection.close()
   #
   # parent_node_id
   parent_node_id   = None
   parent_node_name = None
   for row in table['option'] :
      if row['option_name'] == 'parent_node_id' :
         parent_node_id = int( row['option_value'] )
      if row['option_name'] == 'parent_node_name' :
         parent_node_name = row['option_value']
   assert not ( parent_node_id is None and parent_node_name is None )
   if parent_node_id is None :
      for (node_id, row) in enumerate( table['node'] ) :
         if row['node_name'] == parent_node_name :
            parent_node_id = node_id
   if parent_node_id is None :
      msg = f'parent_node_name = {parent_node_name} is not in node table'
      assert False, msg
   #
   # random_seed
   random_seed = 0
   for row in table['option'] :
      if row['option_name'] == 'random_seed' :
         random_seed = int( row['option_value'] )
   if random_seed != 0 :
      random.seed(random_seed)
   #
   # result
   limit_var_table = list()
   #
   # var_id, var_row
   for (var_id, var_row) in enumerate( table['var'] ) :
      #
      # var_type, node_id, subgroup_id, smooth_id
      var_type    = var_row['var_type']
      node_id     = var_row['node_id']
      subgoup_id  = var_row['subgroup_id']
      group_id    = var_row['group_id']
      subgroup_id = var_row['subgroup_id']
      smooth_id   = var_row['smooth_id']
      assert (subgroup_id is None) or (group_id is None)
      #
      # smooth_row
      smooth_row = table['smooth'][smooth_id]
      #
      # value_prior_id, const_value
      value_prior_id = None
      const_value    = None
      if var_type.startswith('mulstd_') :
         value_prior_id = smooth_row['mulstd_value_prior_id']
      else :
         # age_id, time_id
         age_id  = var_row['age_id']
         time_id = var_row['time_id']
         #
         # grid_row
         for grid_row in table['smooth_grid'] :
            #
            # match
            # does grid_row match this variable
            match = True
            match = match and grid_row['smooth_id'] == smooth_id
            match = match and grid_row['age_id']    == age_id
            match = match and grid_row['time_id']   == time_id
            if match :
               #
               # value_prior_id, const_value
               value_prior_id = grid_row['value_prior_id']
               const_value    = grid_row['const_value']
      #
      # lower, upper, mean
      if value_prior_id is None :
         assert const_value is not None
         lower = const_value
         upper = const_value
         mean  = const_value
      else :
         assert const_value is None
         prior_row = table['prior'][value_prior_id]
         lower     = prior_row['lower']
         upper     = prior_row['upper']
         mean      = prior_row['mean']
      #
      row = { 'lower': lower, 'mean' : mean, 'upper' : upper }
      limit_var_table.append( row )
   #
   return limit_var_table
# -----------------------------------------------------------------------------
def perturb_command(database, tbl_name, sigma_str) :
   assert type(database) == str
   assert type(tbl_name) == str
   assert type(sigma_str) == str
   #
   # key
   if tbl_name == 'scale_var' :
      key = 'scale_var_value'
   elif tbl_name == 'start_var' :
      key = 'start_var_value'
   else :
      msg  = f'perturb command: tbl_name = {tbl_name} is not '
      msg += 'start_var or scale_var'
      assert False, msg
   #
   # sigma
   sigma = float(sigma_str)
   if sigma <= 0 :
      msg  = f'perturb command: sigma = {sigma} is less than or equal zero'
      assert False, msg
   #
   # limit_var_table
   limit_var_table = get_limit_var_table(database)
   #
   # connection
   connection = dismod_at.create_connection(
      database, new = False, readonly = False
   )
   #
   # table
   table = dismod_at.get_table_dict(connection, tbl_name)
   #
   for (var_id, limit_row) in enumerate( limit_var_table ) :
      #
      # mul
      log_multiplier = random.gauss(0, sigma)
      multiplier     = math.exp(log_multiplier)
      #
      # value
      value = table[var_id][key]
      value = multiplier * value
      if limit_row['lower'] is not None :
         value = max(value, limit_row['lower'])
      if limit_row['upper'] is not None :
         value = min(value, limit_row['upper'])
      #
      # table
      table[var_id][key] = value
   #
   # replace table
   dismod_at.replace_table(connection, tbl_name, table)
   #
   return
