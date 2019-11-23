# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin create_database$$ $newlinech #$$
# $spell
#	tuple
#	const
#	smoothings
#	avgint
#	num_iter
#	Ipopt
#	pini
#	mulcov
#	mulstd
#	dage
#	dtime
#	len
#	da
#	dt
#	dismod
#	covariate
#	std
#	str
#	bool
#	nslist
#	tuples
#	cv
#	covariates
#	subsmooth
# $$
#
# $section Create a Dismod_at Database$$
#
# $head Syntax$$
# $codei%create_database(
#	%file_name%,
#	%age_list%
#	%time_list%
#	%integrand_table%,
#	%node_table%,
#	%subgroup_table%,
#	%weight_table%,
#	%covariate_table%,
#	%avgint_table%,
#	%data_table%,
#	%prior_table%,
#	%smooth_table%,
#	%nslist_table%,
#	%rate_table%,
#	%mulcov_table%,
#	%option_table%
# )%$$
#
# $head Purpose$$
# This routine makes it easy to create a $code dismod_at$$ database
# with all of its $cref input$$ tables.
# This is only meant for small example and testing cases and is not efficient.
#
# $head Primary Key$$
# For each of the lists above, the order of the
# elements in the corresponding table is the same as the corresponding list.
# For example, $icode%age_list%[%i%]%$$ corresponds to the $th i$$ row
# of the $code age$$ table which has
# $cref/primary key/database/Primary Key/$$ value $icode%age_id% = %i%$$.
#
# $head Name Column$$
# The $cref/name columns/database/Name Column/$$ are created with th unique
# constraint; i.e., it will be an error to have the same value appear
# twice in a column $icode%table_name%_name%$$ in the table
# $icode table_name$$.
#
# $head file_name$$
# is as $code str$$ containing the name of the file where the data base
# is stored.
# If this file already exists, it is deleted and a database is created.
#
# $head age_list$$
# is a $code list$$ of $code float$$ that
# specify age values by indices.
#
# $head time_list$$
# is a $code list$$ of $code float$$ that
# specify time values by indices.
#
# $head integrand_table$$
# This is a list of $code dict$$
# that define the rows of the $cref integrand_table$$.
# The dictionary $icode%integrand_table%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name             $cnext str  $cnext name for the $th i$$ integrand  $rnext
# minimum_meas_cv  $cnext str  $cnext minimum measurement cv for this integrand
# $tend
# The key $code minimum_meas_cv$$ is optional.
# If it is not present, $code 0.0$$ is used for the corresponding value.
#
# $head node_table$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$.
# The dictionary $icode%node_table%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name for the $th i$$ node  $rnext
# parent  $cnext str           $cnext name of parent of the $th i$$ node
# $tend
# Note that if the i-th node does not have a parent, the empty string
# should be used for the parent of that node.
#
# $head subgroup_table$$
# This is a list of $code dict$$
# that define the rows of the $cref subgroup_table$$.
# The dictionary $icode%node_table%[%i%]%$$ has the following:
# $table
# Key      $cnext Value Type    $pre  $$ $cnext Description           $rnext
# subgroup $cnext str           $cnext name for the $th i$$ subgroup  $rnext
# group    $cnext str           $cnext name of group that subgroup is in
# $tend
#
# $head weight_table$$
# This is a list of $code dict$$
# that define the rows of the $cref weight_table$$ and
# $cref weight_grid_table$$.
# The dictionary $icode%weight_table%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name of $th i$$ weighting  $rnext
# age_id  $cnext list of int   $cnext indices for age grid       $rnext
# time_id $cnext list of int   $cnext indices for time grid      $rnext
# fun     $cnext function      $cnext $icode%w%=%fun%(%a%, %t%)%$$
# $tend
# The float $icode w$$ is the value of this weighting a the corresponding
# float age $icode a$$ and float time $icode t$$.
# Note that there is an $icode i$$, $icode j$$ such that
# $icode%a% = %age_list%[%age_id%[%i%]]%$$ and
# $icode%t% = %time_list%[%time_id%[%j%]]%$$.
#
# $head covariate_table$$
# This is a list of $code dict$$
# that define the rows of the $cref covariate_table$$.
# The dictionary $icode%covariate_table%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $pre  $$ $cnext Description            $rnext
# name      $cnext str         $cnext name for the $th i$$ covariate  $rnext
# reference $cnext float       $cnext reference value for $th i$$ covariate
# $rnext
# max_difference $cnext float $cnext maximum difference for $th i$$ covariate
# $tend
# If $icode max_difference$$ is $code None$$, the corresponding table entry
# is null and this corresponds to an infinite maximum difference.
# If $icode max_difference$$ does not appear, null is written for the
# corresponding covariate entry.
#
# $head avgint_table$$
# This is a list of $code dict$$
# that define the rows of the $cref avgint_table$$.
# The dictionary $icode%avgint_table%[%i%]%$$ has the following:
# $table
# Key          $cnext Value Type  $pre  $$ $cnext Description        $rnext
# integrand    $cnext str         $cnext integrand for $th i$$ data  $rnext
# node         $cnext str         $cnext name of node in graph       $rnext
# subgroup     $cnext str         $cnext name of subgroup            $rnext
# weight       $cnext str         $cnext weighting function name     $rnext
# age_lower    $cnext float       $cnext lower age limit             $rnext
# age_upper    $cnext float       $cnext upper age limit             $rnext
# time_lower   $cnext float       $cnext lower time limit            $rnext
# time_lower   $cnext float       $cnext upper time limit            $rnext
# $icode c_0$$ $cnext float       $cnext value of first covariate    $rnext
# ...          $cnext ...         $cnext  ...                        $rnext
# $icode c_J$$ $cnext float       $cnext value of last covariate
# $tend
# if the $code subgroup$$ key is not present, the first subgroup in
# $cref/subgroup_table/create_database/subgroup_table/$$ is used.
#
# $subhead weight$$
# The weighting function name identifies an
# entry in the $cref/weight_table/create_database/weight_table/$$
# by its $icode name$$. If $icode weight$$ is the empty string,
# the constant weighting is used.
#
# $subhead covariates$$
# Note that $icode%J% = len(%covariate_table%) - 1%$$ and for
# $icode%j% = 0 , %...% , %J%$$,
# $codei%
#	%c_j% = %covariate_table%[%j%]['name']
# %$$
# We refer to the columns above as the required columns for
# $icode avgint_table$$.
#
# $subhead avgint_extra_columns$$
# If a $icode row$$ of $icode option_table$$ has $icode%row%['name']%$$
# equal to $code 'avgint_extra_columns'$$, the corresponding
# $icode%row%['value'].split()%$$ is the list of extra avgint table columns.
# Otherwise the list of extra avgint table columns is empty.
#
# $head data_table$$
# This is a list of $code dict$$
# that define the rows of the $cref data_table$$.
# It has all the columns required for the $icode avgint_table$$.
# In addition, the dictionary $icode%data_table%[%i%]%$$ has the following:
# $table
# Key          $cnext Value Type  $pre  $$ $cnext Description        $rnext
# hold_out     $cnext bool        $cnext hold out flag               $rnext
# density      $cnext str         $cnext
# $cref/density_name/density_table/density_name/$$                   $rnext
# meas_value   $cnext float       $cnext measured value              $rnext
# meas_std     $cnext float       $cnext standard deviation          $rnext
# eta          $cnext float       $cnext offset in log-transform     $rnext
# nu           $cnext float       $cnext Student's-t degrees of freedom
# $tend
# The columns keys $code meas_std$$, $code eta$$, and $code nu$$
# are optional. If they are not present, the value $code null$$ is used
# for the corresponding row of the data table.
# if the $code subgroup$$ key is not present, the first subgroup in
# $cref/subgroup_table/create_database/subgroup_table/$$ is used.
#
# $subhead data_extra_columns$$
# If a $icode row$$ of $icode option_table$$ has $icode%row%['name']%$$
# equal to $code 'data_extra_columns'$$, the corresponding
# $icode%row%['value'].split()%$$ is the list of extra data table columns.
# Otherwise the list of extra data table columns is empty.
#
# $head prior_table$$
# This is a list of $code dict$$
# that define the rows of the $cref prior_table$$.
# The dictionary $icode%prior_table%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name of $th i$$ prior $rnext
# lower   $cnext float         $cnext lower limit                $rnext
# upper   $cnext float         $cnext upper limit                $rnext
# std     $cnext float         $cnext standard deviation         $rnext
# density      $cnext str         $cnext
# $cref/density_name/density_table/density_name/$$               $rnext
# eta     $cnext float         $cnext offset in log densities    $rnext
# nu      $cnext float         $cnext degrees of freed in Student densities
# $tend
# The columns keys $code std$$, $code eta$$, and $code nu$$
# are optional. If they are not present, the value $code null$$ is used
# for the corresponding row of the prior table.
#
# $head smooth_table$$
# This is a list of $code dict$$
# that define the rows of the $cref smooth_table$$ and
# $cref smooth_grid_table$$.
# The dictionary $icode%smooth_table%[%i%]%$$ has the following keys:
#
# $subhead name$$
# an $code str$$ specifying the name used to reference the $th i$$ smoothing.
#
# $subhead age_id$$
# a list of $code int$$ specifying the age values for this smoothing
# as indices in $cref/age_list/create_database/age_list/$$.
#
# $subhead time_id$$
# a list of $code int$$ specifying the time values for this smoothing
# as indices in $cref/time_list/create_database/time_list/$$.
#
# $subhead mulstd_value_prior_name$$
# an $code str$$ specifying the prior used for the value multiplier
# for the $th i$$ smoothing; see
# $cref/mulstd_value_prior_id/smooth_table/mulstd_value_prior_id/$$
# This key is optional and its default value is $code None$$ which corresponds
# to $code null$$ in the database.
#
# $subhead mulstd_dage_prior_name$$
# an $code str$$ specifying the prior used for the age difference multiplier
# for the $th i$$ smoothing; see
# $cref/mulstd_dage_prior_id/smooth_table/mulstd_dage_prior_id/$$
# This key is optional and its default value is $code None$$ which corresponds
# to $code null$$ in the database.
#
# $subhead mulstd_dtime_prior_name$$
# an $code str$$ specifying the prior used for the time difference multiplier
# for the $th i$$ smoothing; see
# $cref/mulstd_dtime_prior_id/smooth_table/mulstd_dtime_prior_id/$$
# This key is optional and its default value is $code None$$ which corresponds
# to $code null$$ in the database.
#
# $subhead fun$$
# This is a function with the following syntax:
# $codei%
#	(%v%, %da%, %dt%) = %fun%(%a%, %t%)
# %$$
# The $code str$$ results $icode v$$, $icode da$$, and $icode dt$$
# are the names for the value prior, age difference prior,
# and time difference prior corresponding to the $th i$$ smoothing.
# The value $icode da$$ is not used,
# when age $icode%a% = %age_id%[-1]%$$.
# The value $icode dt$$ is not used,
# when time $icode%t% = %time_id%[-1]%$$.
# Note that there is an $icode i$$, $icode j$$ such that
# $icode%a% = %age_list%[%age_id%[%i%]]%$$ and
# $icode%t% = %time_list%[%time_id%[%j%]]%$$.
#
# $subhead const_value$$
# The $icode fun$$ return value $icode v$$ may be a $code float$$.
# In this case, the value of the smoothing, at the corresponding age and time,
# is constrained to be $icode v$$ using the
# $cref/const_value/smooth_grid_table/const_value/$$ column in the
# $code smooth_grid$$ table.
#
# $head nslist_table$$
# This is a $code dict$$ that specifies the
# $cref nslist_table$$ and the $cref nslist_pair_table$$.
# For each $cref/nslist_name/nslist_table/nslist_name/$$,
# $codei%
#	%nslist_table%[%nslist_name%] = [ (%node_name%, %smooth_name%), %...% ]
# %$$
# Note that each pair above is a python $code tuple$$:
# $table
# Variable    $cnext Value Type $pre  $$ $cnext Description              $rnext
# nslist_name $cnext str $cnext name of one list of node,smoothing pairs $rnext
# node_name   $cnext str $cnext name of the node for this pair           $rnext
# smooth_name $cnext str $cnext name of the smoothing for this pair
# $tend
#
# $head rate_table$$
# This is a list of $code dict$$
# that define the rows of the $cref rate_table$$.
# The dictionary $icode%rate_table%[%i%]%$$ has the following:
# $table
# Key           $cnext Value Type  $pre  $$ $cnext Description         $rnext
# name          $cnext str     $cnext pini, iota, rho, chi, or omega   $rnext
# parent_smooth $cnext str     $cnext parent smoothing                 $rnext
# child_smooth  $cnext str     $cnext a single child smoothing         $rnext
# child_nslist  $cnext str     $cnext list of child smoothings
# $tend
# The value $code None$$ is used to represent a $code null$$ value for
# the parent and child smoothings.
# If a key name does not appear, null is used for the corresponding value.
# If a $icode name$$; e.g. $code rho$$, does not appear, the value
# null is used for the parent and child smoothings for the corresponding rate.
#
# $head mulcov_table$$
# This is a list of $code dict$$
# that define the rows of the $cref mulcov_table$$.
# The dictionary $icode%mulcov_table%[%i%]%$$ has the following:
# $table
# Key        $cnext Value Type  $pre  $$ $cnext Description       $rnext
# covariate  $cnext str         $cnext is the covariate column    $rnext
# type       $cnext str  $cnext
# $code rate_value$$, $code meas_value$$, or $code meas_noise$$ $rnext
# effected   $cnext str         $cnext integrand or rate affected $rnext
# group      $cnext str         $cnext the group that is affected $rnext
# smooth     $cnext str         $cnext smoothing at group level  $rnext
# subsmooth $cnext str          $cnext smoothing at subgroup level
# $tend
# If the $code group$$ key is not present, the first group in
# $cref/subgroup_table/create_database/subgroup_table/$$ is used.
# If the $code subsmooth$$ key is not present, the value null is used for
# the subgroup smoothing in the corresponding row.
#
# $head option_table$$
# This is a list of $code dict$$
# that define the values
# $cref/option_name/option_table/Conventions/option_name/$$,
# $cref/option_value/option_table/Conventions/option_value/$$ in the option table.
# The $th i$$ row of the table will have
# $codei%
#	%option_name%  = %option_table%[%i%]['name']
#	%option_value% = %option_table%[%i%]['value']
# %$$
#
# $childtable%example/table/create_database.py
# %$$
# $head Example$$
# The file $cref create_database.py$$ contains
# and example and test of $code create_database$$.
#
# $end
def create_database(
	file_name,
	age_list,
	time_list,
	integrand_table,
	node_table,
	subgroup_table,
	weight_table,
	covariate_table,
	avgint_table,
	data_table,
	prior_table,
	smooth_table,
	nslist_table,
	rate_table,
	mulcov_table,
	option_table
) :
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
	col_name = [ 'covariate_name',	'reference', 'max_difference' ]
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
	col_name = [ 'subgroup_name', 'group_id', 'group_name' ]
	col_type = [ 'text',          'integer',  'text'       ]
	row_list = []
	group_id   = 0
	group_name = subgroup_table[i]['group']
	for i in range( len(subgroup_table) ) :
		subgroup_name   = subgroup_table[i]['subgroup']
		if subgroup_table[i]['group'] != group_name :
			group_id   = group_id + 1
			group_name = subgroup_table[i]['group']
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
				assert False
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
	for i in range( len(avgint_table) ) :
		avgint = avgint_table[i]
		#
		# subgroup column has a default value
		if 'subgroup' not in avgint :
			avgint['subgroup'] = subgroup_table[0]['subgroup']
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
				assert False
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
	# close the connection
	connection.close()
	return
