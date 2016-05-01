# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin create_database$$ $newlinech #$$
# $spell
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
# $$
#
# $section Create a Dismod_at Database$$
#
# $head Syntax$$
# $codei%create_database(
#	%file_name%,
#	%age_list%
#	%time_list%
#	%integrand_dict%,
#	%node_dict%,
#	%weight_dict%,
#	%covariate_dict%,
#	%data_dict%,
#	%prior_dict%,
#	%smooth_dict%,
#	%rate_dict%,
#	%mulcov_dict%,
#	%option_dict%,
#	%avgint_dict%
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
# specify the age grid values.
#
# $head time_list$$
# is a $code list$$ of $code float$$ that
# specify the time grid values.
#
# $head integrand_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref integrand_table$$.
# The dictionary $icode%integrand_dict%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type $pre  $$ $cnext Description            $rnext
# name    $cnext str        $cnext name for the $th i$$ integrand  $rnext
# eta     $cnext str        $cnext offset in log transform fro this integrand
# $tend
#
# $head node_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$.
# The dictionary $icode%node_dict%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name for the $th i$$ node  $rnext
# parent  $cnext str           $cnext name of parent of the $th i$$ node
# $tend
# Note that if the i-th node does not have a parent, the empty string
# should be used for the parent of that node.
#
# $head weight_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref weight_table$$ and
# $cref weight_grid_table$$.
# The dictionary $icode%weight_dict%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name of $th i$$ weighting  $rnext
# age_id  $cnext list of int   $cnext indices for age grid       $rnext
# time_id $cnext list of int   $cnext indices for time grid      $rnext
# fun     $cnext function      $cnext $icode%w%=%fun%(%a%, %t%)%$$
# $tend
# The float $icode w$$ is the value of this weighting a the corresponding
# float age $icode a$$ and float time $icode t$$.
#
# $head covariate_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref covariate_table$$.
# The dictionary $icode%covariate_dict%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $pre  $$ $cnext Description            $rnext
# name      $cnext str         $cnext name for the $th i$$ covariate  $rnext
# reference $cnext float       $cnext reference value for $th i$$ covariate
# $rnext
# max_difference $cnext float $cnext maximum difference for $th i$$ covariate
# $tend
#
# $head data_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref data_table$$.
# The dictionary $icode%data_dict%[%i%]%$$ has the following:
# $table
# Key          $cnext Value Type  $pre  $$ $cnext Description        $rnext
# integrand    $cnext str         $cnext integrand for $th i$$ data  $rnext
# density      $cnext str         $cnext density                     $rnext
# node         $cnext str         $cnext node in graph               $rnext
# weight       $cnext str         $cnext weighting function          $rnext
# hold_out     $cnext bool        $cnext hold out flag               $rnext
# meas_value   $cnext float       $cnext measured value              $rnext
# meas_std     $cnext float       $cnext standard deviation          $rnext
# age_lower    $cnext float       $cnext lower age limit             $rnext
# age_upper    $cnext float       $cnext upper age limit             $rnext
# time_lower   $cnext float       $cnext lower time limit            $rnext
# time_lower   $cnext float       $cnext upper time limit            $rnext
# $icode c_0$$ $cnext float       $cnext value of first covariate    $rnext
# ...          $cnext ...         $cnext  ...                        $rnext
# $icode c_J$$ $cnext float       $cnext value of last covariate
# $tend
# Note that $icode%J% = len(%covariate_dict%) - 1%$$ and for
# $icode%j% = 0 , %...% , %J%$$,
# $codei%
#	%c_j% = %covariate_dict%[%j%]['name']
# %$$
#
# $head prior_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref prior_table$$.
# The dictionary $icode%prior_dict%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name of $th i$$ prior $rnext
# lower   $cnext float         $cnext lower limit                $rnext
# upper   $cnext float         $cnext upper limit                $rnext
# std     $cnext float         $cnext standard deviation         $rnext
# density $cnext float         $cnext density function           $rnext
# eta     $cnext float         $cnext offset in log densities
# $tend
#
# $head smooth_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref smooth_table$$ and
# $cref smooth_grid_table$$.
# The dictionary $icode%smooth_dict%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type  $pre  $$ $cnext Description       $rnext
# name    $cnext $code str$$         $cnext name of $th i$$ smoothing  $rnext
# age_id  $cnext list of $code int$$ $cnext indices for age values     $rnext
# time_id $cnext list of $code int$$ $cnext indices for time values    $rnext
# mulstd_value_prior_name    $cnext $code str$$   $cnext
#	name of prior for $cref/
#		mulstd_value_prior_name/smooth_table/mulstd_value_prior_id/$$
#	$rnext
# mulstd_dage_prior_name   $cnext $code str$$   $cnext
#	name of prior for $cref/
#		mulstd_dage_prior_name/smooth_table/mulstd_dage_prior_id/$$
#	$rnext
# mulstd_dtime_prior_name   $cnext $code str$$   $cnext
#	name of prior for $cref/
#		mulstd_dtime_prior_name/smooth_table/mulstd_dtime_prior_id/$$
#	$rnext
# fun     $cnext function  $cnext $codei%(%v%,%da%,%dt%)=%fun%(%a%, %t%)%$$
# $tend
# $list number$$
# The value $code None$$ is used to represent $code null$$ for
# the smoothing multipliers names; e.g., $icode mulstd_value_prior_name$$.
# $lnext
# The $code str$$ results $icode v$$, $icode da$$, and $icode dt$$
# are the prior names for the value, difference in age,
# and difference in time corresponding to this smoothing name.
# $lnext
# The $icode age_id$$ and $icode time_id$$ must be in increasing order,
# $icode da$$ is not used when age $icode%a% = %age_id%[-1]%$$ and
# $icode dt$$ is not used when time $icode%t% = %time_id%[-1]%$$.
# $lend
#
# $head rate_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref rate_table$$.
# The dictionary $icode%rate_dict%[%i%]%$$ has the following:
# $table
# Key           $cnext Value Type  $pre  $$ $cnext Description         $rnext
# name          $cnext str     $cnext pini, iota, rho, chi, or omega   $rnext
# parent_smooth $cnext str     $cnext parent smoothing                 $rnext
# child_smooth  $cnext str     $cnext child smoothing
# $tend
# Furthermore the order of the rate names must be
# pini, iota, rho, chi, omega; i.e., the only order allowed for
# the $cref rate_table$$.
# The value $code None$$ is used to represent a $code null$$ value for
# the parent and child smoothings.
#
# $head mulcov_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref mulcov_table$$.
# The dictionary $icode%mulcov_dict%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $pre  $$ $cnext Description       $rnext
# covariate $cnext str         $cnext is the covariate column    $rnext
# type      $cnext str  $cnext rate_value, meas_value, or meas_std $rnext
# effected  $cnext str         $cnext integrand or rate effected $rnext
# smooth    $cnext str         $cnext smoothing name
# $tend
#
# $head option_dict$$
# This is a list of $code dict$$
# that define the values
# $cref/option_name/option_table/option_name/$$,
# $cref/option_value/option_table/option_value/$$ in the option table.
# The $th i$$ row of the table will have
# $codei%
#	%option_name%  = %option_dict%[%i%]['name']
#	%option_value% = %option_dict%[%i%]['value']
# %$$
# There is one exception to this rule.
# The row of the table with $icode option_name$$ equal to
# $cref/parent_node_id/option_table/parent_node_id/$$ is represented by
# $codei%
#	%option_dict%[%i%]['name']  = 'parent_node_name'
#	%option_dict%[%i%]['value'] = %value%
# %$$
# where $icode value$$ is the value of
# $cref/node_name/node_table/node_name/$$ in the node table that
# corresponds to the $icode node_id$$ equal to
# $icode parent_node_id$$ in the option table.
#
# $head avgint_dict$$
# This is a list of $code dict$$
# that define the rows of the $cref avgint_table$$.
# The dictionary $icode%avgint_dict%[%i%]%$$ has the same description as
# $cref/data_dict[i]/create_database/data_dict/$$ except that the
# following keys (and corresponding values) are not present:
# $code density$$,
# $code hold_out$$,
# $code meas_value$$,
# $code meas_std$$.
#
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
	integrand_dict,
	node_dict,
	weight_dict,
	covariate_dict,
	data_dict,
	prior_dict,
	smooth_dict,
	rate_dict,
	mulcov_dict,
	option_dict,
	avgint_dict
) :
	import sys
	import dismod_at
	# -----------------------------------------------------------------------
	# create database
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	# -----------------------------------------------------------------------
	# create age table
	col_name = [ 'age' ]
	col_type = [ 'real' ]
	row_list = []
	for age in age_list :
		row_list.append( [age] )
	tbl_name = 'age'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create time table
	col_name = [ 'time' ]
	col_type = [ 'real' ]
	row_list = []
	for time in time_list :
		row_list.append( [time] )
	tbl_name = 'time'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create integrand table
	col_name = [ 'integrand_name', 'eta' ]
	col_type = [ 'text',           'real']
	row_list = []
	for row in integrand_dict :
		row_list.append( [ row['name'], row['eta'] ]  )
	tbl_name = 'integrand'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_integrand_name2id = {}
	for i in range( len(row_list) ) :
		global_integrand_name2id[ row_list[i][0] ] = i
	# -----------------------------------------------------------------------
	# create density table
	col_name = [  'density_name'   ]
	col_type = [  'text'        ]
	row_list = [
		['uniform'],
		['gaussian'],
		['laplace'],
		['log_gaussian'],
		['log_laplace']
	]
	tbl_name = 'density'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_density_name2id = {}
	for i in range( len(row_list) ) :
		global_density_name2id[ row_list[i][0] ] = i
	# ------------------------------------------------------------------------
	# create covariate table
	col_name = [ 'covariate_name',	'reference', 'max_difference' ]
	col_type = [ 'text',             'real',     'real'           ]
	row_list = [ ]
	for i in range( len(covariate_dict) ) :
		row       = [
			covariate_dict[i]['name'],
			covariate_dict[i]['reference'],
			covariate_dict[i]['max_difference']
		]
		row_list.append(row)
	tbl_name = 'covariate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_covariate_name2id = {}
	for i in range( len(covariate_dict) ) :
		global_covariate_name2id[ covariate_dict[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create node table
	global_node_name2id = {}
	for i in range( len(node_dict) ) :
		global_node_name2id[ node_dict[i]['name'] ] = i
	#
	col_name = [ 'node_name', 'parent' ]
	col_type = [ 'text',      'integer'   ]
	row_list = []
	for i in range( len(node_dict) ) :
		node   = node_dict[i]
		name   = node['name']
		parent = node['parent']
		if parent == '' :
			parent = None
		else :
			parent = global_node_name2id[parent]
		row_list.append( [ name, parent ] )
	tbl_name = 'node'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create prior table
	col_name = [
		'prior_name', 'lower', 'upper', 'mean', 'std',  'density_id', 'eta'
	]
	col_type = [
		'text',       'real', 'real',  'real', 'real', 'integer',    'real'
	]
	row_list = [ ]
	for i in range( len( prior_dict ) ) :
		prior         = prior_dict[i]
		density_id   = global_density_name2id[ prior['density'] ]
		row  = [
			prior['name'],
			prior['lower'],
			prior['upper'],
			prior['mean'],
			prior['std'],
			density_id,
			prior['eta']
		]
		row_list.append( row )
	tbl_name = 'prior'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_prior_name2id = {}
	for i in range( len(row_list) ) :
		global_prior_name2id[ row_list[i][0] ] = i
	# ------------------------------------------------------------------------
	# create weight table
	col_name = [ 'weight_name', 'n_age',   'n_time'   ]
	col_type = [ 'text',        'integer', 'integer'  ]
	row_list = [ ]
	for i in range( len(weight_dict) ) :
		weight = weight_dict[i]
		name   = weight['name']
		n_age  = len( weight['age_id'] )
		n_time = len( weight['time_id'] )
		row_list.append( [ name, n_age, n_time ] )
	tbl_name = 'weight'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_weight_name2id = {}
	for i in range( len(weight_dict) ) :
		global_weight_name2id[ weight_dict[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create weight_grid table
	col_name = [  'weight_id', 'age_id',   'time_id',  'weight' ]
	col_type = [  'integer',   'integer',  'integer',  'real'   ]
	row_list = [ ]
	for i in range( len(weight_dict) ) :
		weight  = weight_dict[i]
		age_id  = weight['age_id']
		time_id = weight['time_id']
		fun    = weight['fun']
		for j in age_id :
			for k in time_id :
				w = fun(age_list[j], time_list[k])
				row_list.append( [ i, j, k, w] )
	tbl_name = 'weight_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
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
	for i in range( len(smooth_dict) ) :
		smooth        = smooth_dict[i]
		name          = smooth['name']
		n_age         = len( smooth['age_id'] )
		n_time        = len( smooth['time_id'] )
		#
		prior_name    = smooth['mulstd_value_prior_name']
		if prior_name == None :
			mulstd_value_prior_id = None
		else :
			mulstd_value_prior_id = global_prior_name2id[prior_name]
		#
		prior_name    = smooth['mulstd_dage_prior_name']
		if prior_name == None :
			mulstd_dage_prior_id = None
		else :
			mulstd_dage_prior_id = global_prior_name2id[prior_name]
		#
		prior_name    = smooth['mulstd_dtime_prior_name']
		if prior_name == None :
			mulstd_dtime_prior_id = None
		else :
			mulstd_dtime_prior_id = global_prior_name2id[prior_name]
		#
		row_list.append( [
			name,
			n_age,
			n_time,
			mulstd_value_prior_id,
			mulstd_dage_prior_id,
			mulstd_dtime_prior_id
		] )
	tbl_name = 'smooth'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_smooth_name2id = {}
	for i in range( len(smooth_dict) ) :
		global_smooth_name2id[ smooth_dict[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create smooth_grid table
	col_name = [
		'smooth_id',
		'age_id',
		'time_id',
		'value_prior_id',
		'dage_prior_id',
		'dtime_prior_id',
	]
	col_type = [
		'integer',  # smooth_id
		'integer',  # age_id
		'integer',  # time_id
		'integer',  # value_prior_id
		'integer',  # dage_prior_id
		'integer',  # dtime_prior_id
	]
	row_list = [ ]
	for i in range( len(smooth_dict) ) :
		smooth  = smooth_dict[i]
		age_id  = smooth['age_id']
		time_id = smooth['time_id']
		fun     = smooth['fun']
		for j in age_id :
			for k in time_id :
				(v,da,dt) = fun(age_list[j], time_list[k])
				v         = global_prior_name2id[v]
				da        = global_prior_name2id[da]
				dt        = global_prior_name2id[dt]
				if j == age_id[-1] :
					da = None
				if k == time_id[-1] :
					dt = None
				row_list.append( [ i, j, k, v, da, dt] )
	tbl_name = 'smooth_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create rate table
	col_name = [  'rate_name', 'parent_smooth_id', 'child_smooth_id'  ]
	col_type = [  'text',      'integer',         'integer'          ]
	row_list = [ ]
	for i in range( len(rate_dict) ) :
		rate             = rate_dict[i]
		rate_name        = rate['name']
		if rate['parent_smooth'] == None :
			parent_smooth_id = None
		else :
			parent_smooth_id = global_smooth_name2id[ rate['parent_smooth'] ]
		if rate['child_smooth'] == None :
			child_smooth_id = None
		else :
			child_smooth_id  = global_smooth_name2id[ rate['child_smooth'] ]
		row_list.append( [ rate_name, parent_smooth_id, child_smooth_id ] )
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	global_rate_name2id = {}
	for i in range( len(row_list) ) :
		global_rate_name2id[ row_list[i][0] ] = i
	# ------------------------------------------------------------------------
	# create mulcov table
	col_name = [
		'mulcov_type',
		'rate_id',
		'integrand_id',
		'covariate_id',
		'smooth_id'
	]
	col_type = [
		'text',    # mulcov_type
		'integer', # rate_id
		'integer', # integrand_id
		'integer', # covariate_id
		'integer'  # smooth_id'
	]
	row_list = []
	for i in range( len(mulcov_dict) ) :
		mulcov          = mulcov_dict[i]
		mulcov_type     = mulcov['type']
		effected        = mulcov['effected']
		if mulcov_type == 'rate_value' :
			rate_id      = global_rate_name2id[ effected ]
			integrand_id = None
		else :
			integrand_id = global_integrand_name2id[ effected ]
			rate_id      = None
		covariate_id  = global_covariate_name2id[ mulcov['covariate'] ]
		if mulcov['smooth'] == None :
			smooth_id = None
		else :
			smooth_id     = global_smooth_name2id[ mulcov['smooth'] ]
		row_list.append(
			[mulcov_type, rate_id, integrand_id, covariate_id, smooth_id]
		)
	tbl_name = 'mulcov'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create data table
	col_name = [
		'integrand_id',
		'density_id',
		'node_id',
		'weight_id',
		'hold_out',
		'meas_value',
		'meas_std',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
	]
	for j in range( len(covariate_dict) ) :
		col_name.append( 'x_%s' % j )
	col_type = [
		'integer',              # integrand_id
		'integer',              # density_id
		'integer',              # node_id
		'integer',              # weight_id
		'integer',              # hold_out
		'real',                 # meas_value
		'real',                 # meas_std
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
	]
	for j in range( len(covariate_dict) )  :
		col_type.append( 'real' )
	row_list = [ ]
	for i in range( len(data_dict) ) :
		data = data_dict[i]
		data_id      = i
		integrand_id = global_integrand_name2id[ data['integrand'] ]
		density_id   = global_density_name2id[ data['density'] ]
		node_id      = global_node_name2id[ data['node'] ]
		weight_id    = global_weight_name2id[ data['weight'] ]
		hold_out     = int( data['hold_out'] )
		row = [
			integrand_id,
			density_id,
			node_id,
			weight_id,
			hold_out,
			data['meas_value'],
			data['meas_std'],
			data['age_lower'],
			data['age_upper'],
			data['time_lower'],
			data['time_upper']
		]
		for j in range( len(covariate_dict) ) :
			row.append( data[ covariate_dict[j]['name'] ] )
		row_list.append(row)
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create option table
	col_name = [ 'option_name', 'option_value' ]
	col_type = [ 'text unique', 'text' ]
	row_list = []
	for row in option_dict :
		name  = row['name']
		value = row['value']
		if name == 'parent_node_id' :
			value = str(value)
			msg   = 'create_database.py: option_dict has the following row:\n'
			msg  += "\t{ 'name':'parent_node_id' , 'value':'" + value + "' }\n"
			msg  += 'This is an error and should probably be replaced by\n'
			value = node_dict[int(value)]['name']
			msg  += "\t{ 'name':'parent_node_name' , 'value':'" + value + "' }"
			sys.exit(msg)
		if name == 'parent_node_name' :
			name  = 'parent_node_id'
			value = global_node_name2id[value]
		row_list.append( [ name, value ] )
	tbl_name = 'option'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create avgint table
	col_name = [
		'integrand_id',
		'node_id',
		'weight_id',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper'
	]
	for j in range( len(covariate_dict) ) :
		col_name.append( 'x_%s' % j )
	col_type = [
		'integer',              # integrand_id
		'integer',              # node_id
		'integer',              # weight_id
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real'                  # time_upper
	]
	for j in range( len(covariate_dict) )  :
		col_type.append( 'real' )
	row_list = [ ]
	for i in range( len(avgint_dict) ) :
		avgint = avgint_dict[i]
		avgint_id      = i
		integrand_id = global_integrand_name2id[ avgint['integrand'] ]
		node_id      = global_node_name2id[ avgint['node'] ]
		weight_id    = global_weight_name2id[ avgint['weight'] ]
		row = [
			integrand_id,
			node_id,
			weight_id,
			avgint['age_lower'],
			avgint['age_upper'],
			avgint['time_lower'],
			avgint['time_upper']
		]
		for j in range( len(covariate_dict) ) :
			row.append( avgint[ covariate_dict[j]['name'] ] )
		row_list.append(row)
	tbl_name = 'avgint'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# close the connection
	connection.close()
	return
