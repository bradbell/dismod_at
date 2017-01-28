# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin db2csv_command$$ $newlinech #$$
# $spell
#	ij
#	Csv
#	py
#	dismod
#	var
#	covariate
#	covariates
#	res
#	dage
#	dtime
#	std
#	sim
#	avgint
#	dir
#	dismodat
# $$
#
# $section Summary of Database as Two Csv Files$$
#
# $head Syntax$$
#
# $subhead As Program$$
# $codei%dismodat.py %database% db2csv%$$
#
# $subhead As Python Function$$
# $codei%dismod_at.db2csv_command( %database% )%$$
#
# $head 2DO$$
# Write some automated tests for this program.
#
# $head Convention$$
# The $code null$$ value in the database corresponds
# to an empty string in the csv files.
#
# $head database$$
# is the path from the currently directory to the database.
# This must be a $cref/dismod_at/database/$$ and
# the $cref init_command$$ must have been run on the database.
#
# $subhead dir$$
# We use the notation $icode dir$$ for the directory where $icode database$$
# is located.
#
# $head option.csv$$
# The file $icode%dir%/option.csv%$$ is written by this command.
# It is a CSV file with one row for each option.
# The columns in this table are
# $cref/option_name/option_table/option_name/$$ and
# $cref/option_value/option_table/option_value/$$.
#
# $head log.csv$$
# The file $icode%dir%/log.csv%$$ is written by this command.
# It is a CSV file with one row for each message in the $cref log_table$$.
# The columns in this table are
# $cref/message_type/log_table/message_type/$$,
# $cref/table_name/log_table/table_name/$$,
# $cref/row_id/log_table/row_id/$$,
# $cref/unix_time/log_table/unix_time/$$, and
# $cref/message/log_table/message/$$.
#
# $head variable.csv$$
# The file $icode%dir%/variable.csv%$$ is written by this command.
# It is a CSV file with one row for each of the $cref model_variables$$
# and has the following columns:
#
# $subhead var_id$$
# is the $cref/var_id/var_table/var_id/$$.
#
# $subhead var_type$$
# is the $cref/var_type/var_table/var_type/$$.
#
# $subhead s_id$$
# is the $cref/smooth_id/smooth_table/smooth_id/$$ for this variable.
# If the variable is a
# $cref/smoothing standard deviation multiplier
#    /model_variables
#    /Fixed Effects, theta
#    /Smoothing Standard Deviation Multipliers, lambda
#/$$
# this is the smoothing that this multiplier effects.
# Otherwise, it is the smoothing where the prior for this variable
# comes from.
#
# $subhead age$$
# is the $cref/age/age_table/age/$$.
#
# $subhead time$$
# is the $cref/time/time_table/time/$$.
#
# $subhead rate$$
# is the $cref/rate_name/rate_table/rate_name/$$.
#
# $subhead integrand$$
# is the
# $cref/integrand_name/integrand_table/integrand_name/$$.
#
# $subhead covariate$$
# is the
# $cref/covariate_name/covariate_table/covariate_name/$$.
#
# $subhead node$$
# is the
# $cref/node_name/node_table/node_name/$$.
#
# $subhead fixed$$
# is $code true$$ if this variable is a
# $cref/fixed effect/model_variables/Fixed Effects, theta/$$,
# otherwise it is $code false$$.
#
# $subhead sam_value$$
# If the $cref sample_command$$ has been run, this is the
# $cref/var_value/sample_table/var_value/$$ corresponding to
# $cref/sample_index/sample_table/sample_index/$$ equal to zero.
#
# $subhead fit_value$$
# If the $cref fit_command$$ has been run, this is the
# $cref/variable_value/fit_var_table/variable_value/$$.
#
# $subhead res_value$$
# If the $cref fit_command$$ has been run, this is the
# $cref/residual_value/fit_var_table/residual_value/$$.
#
# $subhead res_dage$$
# If the $cref fit_command$$ has been run, this is the
# $cref/residual_dage/fit_var_table/residual_dage/$$.
#
# $subhead res_dtime$$
# If the $cref fit_command$$ has been run, this is the
# $cref/residual_dtime/fit_var_table/residual_dtime/$$.
#
# $subhead lag_value$$
# If the $cref fit_command$$ has been run, this is the
# $cref/lagrange_value/fit_var_table/lagrange_value/$$.
#
# $subhead lag_dage$$
# If the $cref fit_command$$ has been run, this is the
# $cref/lagrange_dage/fit_var_table/lagrange_dage/$$.
#
# $subhead lag_dtime$$
# If the $cref fit_command$$ has been run, this is the
# $cref/lagrange_dtime/fit_var_table/lagrange_dtime/$$.
#
# $subhead prior_info$$
# There is a column named
# $codei%
#	%field%_%character%
# %$$
# for $icode character$$ equal to $code v$$, $code a$$ and $code t$$
# and for $icode field$$ equal to
# $cref/lower/prior_table/lower/$$,
# $cref/upper/prior_table/upper/$$,
# $cref/mean/prior_table/mean/$$,
# $cref/std/prior_table/std/$$,
# $cref/eta/prior_table/eta/$$ and
# $cref/density/prior_table/density_id/$$.
# The character $code v$$ denotes this is the prior information for a value,
# $code a$$ the prior information for an age difference, and
# $code t$$ the prior information for a time difference.
# The density has been mapped to the corresponding
# $cref/density_name/density_table/density_name/$$.
#
# $head data.csv$$
# The file $icode%dir%/data.csv%$$ is written by this command.
# It is a CSV file with one row for each row in the $cref data_subset_table$$
# and has the following columns:
#
# $subhead data_id$$
# is the data table
# $cref/data_id/data_table/data_id/$$.
#
# $subhead data_name$$
# is the data table
# $cref/data_name/data_table/data_name/$$.
#
# $subhead age_lo$$
# is the data table
# $cref/age_lower/data_table/age_lower/$$.
#
# $subhead age_up$$
# is the data table
# $cref/age_upper/data_table/age_upper/$$.
#
# $subhead time_lo$$
# is the data table
# $cref/time_lower/data_table/time_lower/$$.
#
# $subhead time_up$$
# is the data table
# $cref/time_upper/data_table/time_upper/$$.
#
# $subhead integrand$$
# is the data table
# $cref/integrand_name/integrand_table/integrand_name/$$.
#
# $subhead weight$$
# is the
# $cref/weight_name/weight_table/weight_name/$$ for this row.
#
# $subhead hold_out$$
# is the data table
# $cref/hold_out/data_table/hold_out/$$.
#
# $subhead density$$
# is the
# $cref/density_name/density_table/density_name/$$ for this row.
#
# $subhead eta$$
# is the
# $cref/eta/integrand_table/eta/$$ for this row.
#
# $subhead meas_value$$
# is the data table
# $cref/meas_value/data_table/meas_value/$$.
#
# $subhead meas_std$$
# is the data table
# $cref/meas_std/data_table/meas_std/$$.
#
# $subhead sim_value$$
# If $cref/simulate_index/fit_command/simulate_index/$$
# is present in the previous fit command, it is the simulate table
# $cref/meas_value/simulate_table/meas_value/$$ for
# the specified $cref/simulate_index/simulate_table/simulate_index/$$.
#
# $subhead sim_std$$
# If $cref/simulate_index/fit_command/simulate_index/$$
# is present in the previous fit command, it is the simulate table
# $cref/meas_std/simulate_table/meas_std/$$ for
# the specified $cref/simulate_index/simulate_table/simulate_index/$$.
#
# $subhead avgint$$
# If the $cref fit_command$$ has been run, this is the
# $cref/avg_integrand/fit_data_subset_table/avg_integrand/$$ for this row.
#
# $subhead residual$$
# If the $cref fit_command$$ has been run, this is the
# $cref/weighted_residual/fit_data_subset_table/weighted_residual/$$
# for this row.
#
# $subhead node$$
# is the $cref/node_name/node_table/node_name/$$ that this
# row is associated with
# during a $code dismod_at$$ fit.
# This will correspond directly to the data table
# $cref/node_id/data_table/node_id/$$, or be an ascendant of that node.
#
# $subhead Covariates$$
# For each covariate in the $cref covariate_table$$ there is a column with
# the corresponding $icode covariate_name$$.
# For each covariate column and measurement row, the value in the
# covariate column is covariate value for this measurement minus
# the reference value for this covariate, i.e., the corresponding
# $cref/x_ij/avg_integrand/Data or Avgint Table Notation/x_ij/$$ in the model
# for the average integrand.
#
# $head predict.csv$$
# If the $cref predict_command$$ has was executed,
# the CSV file $code predict.csv$$ is written.
# For each set of $cref model_variables$$ in the $cref sample_table$$,
# and each row of $cref avgint_subset_table$$, there is a corresponding
# row in $code predict.csv$$.
#
# $subhead avgint$$
# is the $cref/average integrand/avg_integrand/Average Integrand, A_i/$$
# $latex A_i(u, \theta)$$. The model variables $latex (u, \theta)$$
# correspond to the $icode sample_index$$, and measurement subscript $latex i$$
# denotes to the $cref avgint_table$$ information
# for this row of $code predict.csv$$; i.e., $icode age_lo$$, $icode age_up$$,
# ...
#
# $subhead age_lo$$
# is the avgint table
# $cref/age_lower/avgint_table/age_lower/$$.
#
# $subhead age_up$$
# is the avgint table
# $cref/age_upper/data_table/age_upper/$$.
#
# $subhead time_lo$$
# is the avgint table
# $cref/time_lower/data_table/time_lower/$$.
#
# $subhead time_up$$
# is the avgint table
# $cref/time_upper/data_table/time_upper/$$.
#
# $subhead integrand$$
# is the avgint table
# $cref/integrand_name/integrand_table/integrand_name/$$.
#
# $subhead weight$$
# is the
# $cref/weight_name/weight_table/weight_name/$$ for this row.
#
# $subhead node$$
# is the
# $cref/node_name/node_table/node_name/$$ for this row.
#
# $subhead sample_index$$
# This identifies the set model variables in the sample table.
# To be specific, the model variables correspond to the rows on the
# sample table with the same $cref/sample_index/sample_table/sample_index/$$.
#
# $subhead Covariates$$
# For each covariate in the $cref covariate_table$$ there is a column with
# the corresponding $icode covariate_name$$.
# For each covariate column and measurement row, the value in the
# covariate column is covariate value in the $cref avgint_table$$
# minus the reference value for this covariate. i.e., the corresponding
# $cref/x_ij/avg_integrand/Data or Avgint Table Notation/x_ij/$$ in the model
# for the average integrand.
#
# $end
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
#
def db2csv_command(database_file_arg) :
	import os
	import csv
	import dismod_at
	import sys
	import copy
	#
	file_name    = database_file_arg
	database_dir = os.path.split(database_file_arg)[0]
	new          = False
	connection   = dismod_at.create_connection(file_name, new)
	cursor       = connection.cursor()
	required_table_list  = [
		'age',
		'avgint',
		'avgint_subset',
		'covariate',
		'data',
		'data_subset',
		'density',
		'integrand',
		'log',
		'option',
		'mulcov',
		'node',
		'prior',
		'rate',
		'smooth',
		'smooth_grid',
		'time',
		'var',
		'weight'
	]
	for table in required_table_list :
		if not check4table( cursor, table ):
			msg  = 'db2csv_command: the required table ' + table + '\n'
			msg += 'is missing from file ' + file_name + '\n'
			sys.exit(msg)
	#
	have_table = dict()
	have_table['sample']          = check4table(cursor, 'sample')
	have_table['simulate']        = check4table(cursor, 'simulate')
	have_table['fit_var']         = check4table(cursor, 'fit_var')
	have_table['fit_data_subset'] = check4table(cursor, 'fit_data_subset')
	have_table['predict']         = check4table(cursor, 'predict')
	if have_table['fit_var'] != have_table['fit_data_subset'] :
		msg = 'db2csv_command: '
		for table in [ 'fit_var', 'fit_data_subset' ] :
			if have_table[table] :
				msg  += 'have ' + table + ' = true\n'
			else :
				msg  += 'have ' + table + ' = false\n'
		msg += 'in ' + file_name + '\n'
		sys.exit(msg)
	#
	table_data  = dict()
	table_list  = copy.copy( required_table_list )
	for key in have_table :
		if have_table[key] :
			table_list.append(key)
	#
	for table in table_list :
		table_data[table] = dismod_at.get_table_dict(connection, table)
	# -------------------------------------------------------------------------
	# check tables that are supposed to be the same length
	pair_list = [
		[ 'var',         'fit_var'],
		[ 'data_subset', 'fit_data_subset' ]
	]
	for [left, right] in pair_list :
		if have_table[right] :
			len_left  = len( table_data[left]  )
			len_right = len( table_data[right] )
			if len_left != len_right :
				msg  = 'db2csv_command: tables should have same length:\n'
				msg += 'length ' + left + '_table = ' + str(len_left) + '\n'
				msg += 'length ' + right + '_table = ' + str(len_right) + '\n'
				sys.exit(msg)
	# -------------------------------------------------------------------------
	# parent_node_id
	parent_node_id     = None
	for row in table_data['option'] :
		if row['option_name'] == 'parent_node_id' :
			parent_node_id = int( row['option_value'] )
	# -------------------------------------------------------------------------
	# simulate_index
	simulate_index = None
	log_data       = dismod_at.get_table_dict(connection, 'log')
	# search for the last fit commmand in the log table
	for i in range( len(log_data) ) :
		log_id        = len(log_data) - i - 1
		row           = log_data[log_id]
		if simulate_index == None and row['message_type'] == 'command' :
			message = row['message']
			if message.startswith('begin fit') :
				simulate_index = message[ len('begin fit') : ].strip()
	if simulate_index == None :
		if have_table['fit_var'] :
			msg = 'Have fit_var table but cannot find '
			msg += 'fit command in the log table\n'
			sys.exit(msg)
		simulate_index = ''
	if simulate_index != '' and not have_table['simulate'] :
		msg  = 'Previous fit command used simulated data but\n'
		msg += 'cannot find simulate_table\n'
		sys.exit(msg)
	if simulate_index == '' :
		simulate_index = None
	else :
		simulate_index = int(simulate_index)
	# -------------------------------------------------------------------------
	def convert2output(value_in) :
		if value_in == None :
			value_out = ''
		elif type(value_in) is float :
			value_out = '%13.5g' % value_in
		else :
			value_out = str(value_in)
		return value_out
	# -------------------------------------------------------------------------
	def table_lookup(table_name, row_id, column_name) :
		if row_id == None :
			return ''
		value_in = table_data[table_name][row_id][column_name]
		return convert2output(value_in)
	# -------------------------------------------------------------------------
	def get_prior_info(row_out, prior_id_dict) :
		extension2name = {'_v':'value_', '_a':'dage_', '_t':'dtime_' }
		for extension in extension2name :
			name      = extension2name[extension]
			key       = name + 'prior_id'
			prior_id  = prior_id_dict[key]
			field_out = 'density' + extension
			if prior_id == None :
				row_out[field_out] = ''
			else :
				density_id   = table_data['prior'][prior_id]['density_id']
				density_name = table_data['density'][density_id]['density_name']
				row_out[field_out] = density_name
			for field_in in [ 'lower', 'upper', 'mean', 'std', 'eta' ] :
				field_out = field_in + extension
				row_out[field_out] = ''
				if prior_id != None :
					value_in = table_data['prior'][prior_id][field_in]
					if field_in == 'eta' :
						if density_name in [ 'uniform', 'gaussian', 'laplace' ] :
							if extension in [ '_a', '_t' ] :
								value_in = None
					if field_in == 'std' and density_name == 'uniform' :
							value_in = None
					row_out[field_out] = convert2output( value_in )

	# -------------------------------------------------------------------------
	def node_id2child_or_parent(node_id) :
		if node_id == parent_node_id :
			name = table_data['node'][node_id]['node_name']
			return name
		descendant_id = node_id
		while descendant_id != None :
			parent_id  = table_data['node'][descendant_id]['parent']
			if parent_id == parent_node_id :
				name = table_data['node'][descendant_id]['node_name']
				return name
			descendant_id = parent_id
		return None
	# =========================================================================
	# option.csv
	# =========================================================================
	file_name  = os.path.join(database_dir, 'option.csv')
	csv_file   = open(file_name, 'w')
	header     = [ 'option_name', 'option_value' ]
	csv_writer = csv.DictWriter(csv_file, fieldnames=header)
	csv_writer.writeheader()
	# This table must have the same values as in
	# devel/table/get_option_table.cpp
	option_list = [
		[ "accept_after_max_steps_fixed",  "5"],
		[ "accept_after_max_steps_random", "5"],
		[ "derivative_test_fixed",         "none"],
		[ "derivative_test_random",        "none"],
		[ "fixed_bound_frac",              "1e-2"],
		[ "max_num_iter_fixed",            "100"],
		[ "max_num_iter_random",           "100"],
		[ "ode_step_size",                 "10.0"],
		[ "parent_node_id",                "0"],
		[ "print_level_fixed",             "0"],
		[ "print_level_random",            "0"],
		[ "quasi_fixed",                   "true"],
		[ "random_bound",                  ""],
		[ "random_seed",                   "0"],
		[ "random_zero_sum",               "false"],
		[ "rate_case",                     "iota_pos_rho_zero"],
		[ "tolerance_fixed",               "1e-8"],
		[ "tolerance_random",              "1e-8"]
	]
	for row in table_data['option'] :
		found = False
		for choice in option_list :
			if row['option_name'] == choice[0] :
				found = True
				choice[1] = row['option_value']
		assert found
	for row in option_list :
		row_out = { 'option_name' : row[0], 'option_value' : row[1] }
		csv_writer.writerow(row_out)
	csv_file.close()
	# =========================================================================
	# log.csv
	# =========================================================================
	file_name = os.path.join(database_dir, 'log.csv')
	csv_file  = open(file_name, 'w')
	header = ['message_type', 'table_name', 'row_id', 'unix_time', 'message']
	csv_writer = csv.DictWriter(csv_file, fieldnames=header)
	csv_writer.writeheader()
	for row in table_data['log'] :
		csv_writer.writerow(row)
	csv_file.close()
	# =========================================================================
	# variable.csv
	# =========================================================================
	file_name = os.path.join(database_dir, 'variable.csv')
	csv_file  = open(file_name, 'w')
	#
	header = [
		'var_id',
		'var_type',
		's_id',
		'age',
		'time',
		'rate',
		'integrand',
		'covariate',
		'node',
		'fixed',
		'sam_value',
		'fit_value',
		'res_value',
		'res_dage',
		'res_dtime',
		'lag_value',
		'lag_dage',
		'lag_dtime',
	]
	for extension in ['_v', '_a', '_t' ] :
		for root in ['lower', 'upper', 'mean', 'std', 'eta', 'density' ] :
			header.append( root + extension )
	csv_writer = csv.DictWriter(csv_file, fieldnames=header)
	csv_writer.writeheader()
	var_id  = 0
	row_out = dict()
	for field in header :
		row_out[field] = ''
	for row_in in table_data['var'] :
		row_out['var_id']    = var_id
		row_out['var_type']  = row_in['var_type']
		row_out['s_id']      = row_in['smooth_id']
		row_out['age']       = table_lookup('age',  row_in['age_id'], 'age')
		row_out['time']      = table_lookup('time', row_in['time_id'], 'time')
		row_out['rate'] = table_lookup('rate', row_in['rate_id'], 'rate_name')
		row_out['integrand'] = table_lookup(
			'integrand', row_in['integrand_id'], 'integrand_name'
		)
		row_out['covariate'] = table_lookup(
			'covariate', row_in['covariate_id'], 'covariate_name'
		)
		row_out['node'] = table_lookup('node', row_in['node_id'], 'node_name')
		#
		# The random effects are all rate variables for the child nodes.
		row_out['fixed'] = 'true'
		if row_in['var_type'] == 'rate' :
			if row_in['node_id'] != parent_node_id :
				row_out['fixed'] = 'false'
		#
		if have_table['sample'] :
			row_out['sam_value'] = \
				 table_lookup('sample', var_id, 'var_value')
		if have_table['fit_var'] :
			row_out['fit_value'] = \
				 table_lookup('fit_var', var_id, 'variable_value')
			row_out['res_value'] = \
				 table_lookup('fit_var', var_id, 'residual_value')
			row_out['res_dage']  = \
				 table_lookup('fit_var', var_id, 'residual_dage')
			row_out['res_dtime'] = \
				 table_lookup('fit_var', var_id, 'residual_dtime')
			row_out['lag_value'] = \
				 table_lookup('fit_var', var_id, 'lagrange_value')
			row_out['lag_dage']  = \
				 table_lookup('fit_var', var_id, 'lagrange_dage')
			row_out['lag_dtime'] = \
				 table_lookup('fit_var', var_id, 'lagrange_dtime')
		#
		smooth_id = row_in['smooth_id']
		if row_in['var_type'] in ['mulstd_value','mulstd_dage','mulstd_dtime'] :
			prior_id_dict = {
			'value_prior_id':None, 'dage_prior_id':None, 'dtime_prior_id':None
			}
			smooth_id_dict = table_data['smooth'][smooth_id]
			key            = row_in['var_type'] + '_prior_id'
			prior_id       = smooth_id_dict[key]
			prior_id_dict['value_prior_id'] = prior_id
			get_prior_info(row_out, prior_id_dict)
		else :
			age_id    = row_in['age_id']
			time_id   = row_in['time_id']
			for row in table_data['smooth_grid'] :
				match = row['smooth_id'] == smooth_id
				match = match and row['age_id'] == age_id
				match = match and row['time_id'] == time_id
				if match :
					prior_id_dict = row
					get_prior_info(row_out, prior_id_dict)
		csv_writer.writerow(row_out)
		var_id += 1
	# -------------------------------------------------------------------------
	csv_file.close()
	# =========================================================================
	# data.csv
	# =========================================================================
	file_name = os.path.join(database_dir, 'data.csv')
	csv_file  = open(file_name, 'w')
	#
	header = [
		'data_id',
		'data_name',
		'age_lo',
		'age_up',
		'time_lo',
		'time_up',
		'integrand',
		'weight',
		'hold_out',
		'density',
		'eta',
		'meas_std',
		'meas_value',
		'avgint',
		'residual',
		'node'
	]
	if simulate_index != None :
		header.append('sim_value')
		header.append('sim_std')
	for row in table_data['covariate'] :
		header.append( row['covariate_name'] )
	csv_writer = csv.DictWriter(csv_file, fieldnames=header)
	csv_writer.writeheader()
	#
	row_out = dict()
	for field in header :
		row_out[field] = ''
	subset_id  = 0
	n_subset   = len( table_data['data_subset'] )
	for subset_row in table_data['data_subset'] :
		for field in header :
			row_out[field] = ''
		data_id   = subset_row['data_id']
		row_in    = table_data['data'][data_id]
		#
		row_out['data_id']     = data_id
		row_out['data_name']   = row_in['data_name']
		row_out['age_lo']      = row_in['age_lower']
		row_out['age_up']      = row_in['age_upper']
		row_out['time_lo']     = row_in['time_lower']
		row_out['time_up']     = row_in['time_upper']
		row_out['hold_out']    = row_in['hold_out']
		row_out['meas_std']    = convert2output( row_in['meas_std'] )
		row_out['meas_value']  = convert2output( row_in['meas_value'] )
		row_out['integrand'] = table_lookup(
			'integrand', row_in['integrand_id'], 'integrand_name'
		)
		row_out['weight'] = table_lookup(
			'weight', row_in['weight_id'], 'weight_name'
		)
		row_out['density'] = table_lookup(
			'density', row_in['density_id'], 'density_name'
		)
		row_out['eta']  = table_lookup(
				'integrand', row_in['integrand_id'], 'eta'
		)
		row_out['node'] = node_id2child_or_parent( row_in['node_id'] )
		#
		#
		covariate_id = 0
		for row in table_data['covariate'] :
			field_in  = 'x_' + str(covariate_id)
			field_out = row['covariate_name']
			reference = row['reference']
			if row_in[field_in] == None :
				row_out[field_out] = '0.0'
			else :
				row_out[field_out] = \
					 convert2output(row_in[field_in] - reference)
			covariate_id += 1
		#
		if have_table['fit_var'] :
			row                 = table_data['fit_data_subset'][subset_id]
			row_out['avgint']   = convert2output( row['avg_integrand'] )
			row_out['residual'] = convert2output( row['weighted_residual'] )
		if simulate_index != None :
			simulate_id =  n_subset * simulate_index + subset_id
			sim_value = table_data['simulate'][simulate_id]['meas_value']
			sim_std   = table_data['simulate'][simulate_id]['meas_std']
			row_out['sim_value'] = sim_value
			row_out['sim_std']   = sim_std
		csv_writer.writerow(row_out)
		subset_id += 1
	csv_file.close()
	# =========================================================================
	# predict.csv
	# =========================================================================
	if have_table['predict'] :
		file_name = os.path.join(database_dir, 'predict.csv')
		csv_file  = open(file_name, 'w')
		#
		header = [
			'avgint',
			'age_lo',
			'age_up',
			'time_lo',
			'time_up',
			'integrand',
			'weight',
			'node',
			'sample_index'
		]
		for row in table_data['covariate'] :
			header.append( row['covariate_name'] )
		csv_writer = csv.DictWriter(csv_file, fieldnames=header)
		csv_writer.writeheader()
		#
		for predict_row in table_data['predict'] :
			row_out = dict()
			#
			# avgint, sample_index
			row_out['avgint']  = convert2output( predict_row['avg_integrand'] )
			row_out['sample_index'] = predict_row['sample_index']
			#
			# age_lo, age_up, time_lo, time_up
			avgint_id          = int( table_lookup(
				'avgint_subset', predict_row['avgint_subset_id'], 'avgint_id'
			) )
			avgint_row         = table_data['avgint'][avgint_id]
			row_out['age_lo']  = avgint_row['age_lower']
			row_out['age_up']  = avgint_row['age_upper']
			row_out['time_lo'] = avgint_row['time_lower']
			row_out['time_up'] = avgint_row['time_upper']
			#
			# integrand
			row_out['integrand'] = table_lookup(
				'integrand', avgint_row['integrand_id'], 'integrand_name'
			)
			# weight
			row_out['weight']    = table_lookup(
				'weight', avgint_row['weight_id'], 'weight_name'
			)
			# node
			row_out['node']      = table_lookup(
				'node', avgint_row['node_id'], 'node_name'
			)
			# covariates
			covariate_id = 0
			for row in table_data['covariate'] :
				field_in  = 'x_' + str(covariate_id)
				field_out = row['covariate_name']
				if avgint_row[field_in] == None :
					row_out[field_out] = '0.0'
				else :
					row_out[field_out] = \
						convert2output(avgint_row[field_in] - reference)
				covariate_id += 1
			#
			csv_writer.writerow(row_out)
		csv_file.close()
