#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin perturb_command$$ $newlinech #$$
# $spell
#	dismodat
#	py
#	dismod
#	var
#	tbl
# $$
#
# $section Perturb Command: Random Change to Start or Scale Tables$$
#
# $head Syntax$$
#
# $subhead As Program$$
# $codei%dismodat.py %database% perturb %tbl_name% %sigma%$$
#
# $subhead As Python Function$$
# $codei%dismod_at.perturb_command(%database%, %tbl_name%, %sigma%)%$$
#
# $head database$$
# is the path from the currently directory to the database.
# This must be a $cref/dismod_at/database/$$ and
# the $cref init_command$$ must have been run on the database.
#
# $head tbl_name$$
# This is the name of an input table that we are perturbing.
# This must be one of the following choices:
#
# $subhead scale_var$$
# In this case the $cref scale_var_table$$ is perturbed.
# This can be useful when one starts near the solution where the
# derivative of the objective may be small.
#
# $subhead start_var$$
# In this case the $cref start_var_table$$ is perturbed.
# This can be useful when one wants to see if the starting point
# has an effect on the solution.
#
# $head sigma$$
# We use $icode multiplier$$ for a randomly chose multiplier,
# that is different for each table entry, and that has a log with
# mean zero and standard deviation $icode sigma$$.
# The value chosen for each table entry is its original value
# times the multiplier.
# If this value is below (above) the lower (upper) limit
# for the table entry the lower (upper) limit is chosen.
#
# $subhead Random Seed$$
# If the $cref/random_seed/option_table/random_seed/$$ is non-zero,
# it is used to seed the random number generator that is used.
# Otherwise, the system clock is used to seed the random number generator.
#
# $end
# -----------------------------------------------------------------------------
import dismod_at
import random
import math
# -----------------------------------------------------------------------------
def get_limit_var_table(database) :
	#
	# table
	new        = False
	connection = dismod_at.create_connection(database, new)
	table = dict()
	for tbl_name in [
		'node',
		'option',
		'prior',
		'smooth',
		'smooth_grid',
		'var',
	] :
		table[tbl_name] = dismod_at.get_table_dict(connection, tbl_name)
	connection.close()
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
	new        = False
	connection = dismod_at.create_connection(database, new)
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
		value = max(value, limit_row['lower'])
		value = min(value, limit_row['upper'])
		#
		# table
		table[var_id][key] = value
	#
	# replace table
	dismod_at.replace_table(connection, tbl_name, table)
	#
	return
