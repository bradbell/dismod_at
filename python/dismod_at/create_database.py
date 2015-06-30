# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin create_database$$ $newlinech #$$
# $spell
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
#	%integrand_list%,
#	%node_list%,
#	%weight_list%,
#	%covariate_list%,
#	%data_list%,
#	%prior_list%,
#	%smooth_list%,
#	%rate_list%,
#	%mulcov_list%
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
# $cref/primary key/input/Primary Key/$$ value $icode%age_id% = %i%$$.
#
# $head Name Column$$
# The $cref/name columns/input/Name Column/$$ are created with th unique
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
# $head integrand_list$$
# This is a list of $code dict$$
# that define the rows of the $cref integrand_table$$.
# The dictionary $icode%integrand_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type $pre  $$ $cnext Description            $rnext
# name    $cnext str        $cnext name for the $th i$$ integrand  $rnext
# eta     $cnext str        $cnext offset in log transform fro this integrand
# $tend
#
# $head node_list$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$.
# The dictionary $icode%node_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $pre  $$ $cnext Description       $rnext
# name    $cnext str           $cnext name for the $th i$$ node  $rnext
# parent  $cnext str           $cnext name of parent of the $th i$$ node
# $tend
# Note that if the i-th node does not have a parent, the empty string
# should be used for the parent of that node.
#
# $head weight_list$$
# This is a list of $code dict$$
# that define the rows of the $cref weight_table$$ and
# $cref weight_grid_table$$.
# The dictionary $icode%weight_list%[%i%]%$$ has the following:
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
# $head covariate_list$$
# This is a list of $code dict$$
# that define the rows of the $cref covariate_table$$.
# The dictionary $icode%covariate_list%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $pre  $$ $cnext Description            $rnext
# name      $cnext str         $cnext name for the $th i$$ covariate  $rnext
# reference $cnext float       $cnext reference value for $th i$$ covariate
# $rnext
# max_difference $cnext float $cnext maximum difference for $th i$$ covariate
# $tend
#
# $head data_list$$
# This is a list of $code dict$$
# that define the rows of the $cref data_table$$.
# The dictionary $icode%data_list%[%i%]%$$ has the following:
# $table
# Key          $cnext Value Type  $pre  $$ $cnext Description        $rnext
# integrand    $cnext str         $cnext integrand for $th i$$ data  $rnext
# density      $cnext str         $cnext density                     $rnext
# node         $cnext str         $cnext node in graph               $rnext
# weight       $cnext str         $cnext weighting function          $rnext
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
# Note that $icode%J% = len(%covariate_list%) - 1%$$ and for
# $icode%j% = 0 , %...% , %J%$$,
# $codei%
#	%c_j% = %covariate_list%[%j%]['name']
# %$$
#
# $head prior_list$$
# This is a list of $code dict$$
# that define the rows of the $cref prior_table$$.
# The dictionary $icode%prior_list%[%i%]%$$ has the following:
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
# $head smooth_list$$
# This is a list of $code dict$$
# that define the rows of the $cref smooth_table$$ and
# $cref smooth_grid_table$$.
# The dictionary $icode%smooth_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type  $pre  $$ $cnext Description       $rnext
# name    $cnext str         $cnext name of $th i$$ smoothing  $rnext
# age_id  $cnext list of int $cnext indices for age values     $rnext
# time_id $cnext list of int $cnext indices for time values    $rnext
# mulstd_value_prior_name   $cnext str   $cnext
#	name for $cref/mulstd_value_prior_id/smooth_table/mulstd_value_prior_id/$$ smoothing $rnext
# mulstd_dage_prior_name   $cnext str   $cnext
#	name corresponding to
#	$cref/mulstd_dage_prior_id/smooth_table/mulstd_dage_prior_id/$$
#	for this smoothing $rnext
# mulstd_dtime_prior_name   $cnext str   $cnext
#	name corresponding to
#	$cref/mulstd_dtime_prior_id/smooth_table/mulstd_dtime_prior_id/$$
#	for this smoothing $rnext
# fun     $cnext function    $cnext $icode%(%v%,%da%,%dt%)%=%fun%(%a%, %t%)%$$
# $tend
# The $code str$$ results $icode v$$, $icode da$$, and $icode dt$$
# are the prior names for the value, difference in age,
# and difference in time corresponding to this smoothing name.
# Note that $icode age_id$$ and $code time_id$$ must be in increasing order,
# $icode da$$ is not used when age $icode%a% = %age_id%[-1]%$$ and
# $icode dt$$ is not used when time $icode%t% = %time_id%[-1]%$$.
#
# $head rate_list$$
# This is a list of $code dict$$
# that define the rows of the $cref rate_table$$.
# The dictionary $icode%rate_list%[%i%]%$$ has the following:
# $table
# Key           $cnext Value Type  $pre  $$ $cnext Description         $rnext
# name          $cnext str     $cnext pini, iota, rho, chi, or omega   $rnext
# parent_smooth $cnext str     $cnext parent smoothing                 $rnext
# child_smooth  $cnext str     $cnext child smoothing
# $tend
# Furthermore the order of the rate names must be
# pini, iota, rho, chi, omega; i.e., the only order allowed for
# the $cref rate_table$$.
#
# $head mulcov_list$$
# This is a list of $code dict$$
# that define the rows of the $cref mulcov_table$$.
# The dictionary $icode%mulcov_list%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $pre  $$ $cnext Description       $rnext
# covariate $cnext str         $cnext is the covariate column    $rnext
# type      $cnext str  $cnext rate_mean, meas_mean, or meas_std $rnext
# effected  $cnext str         $cnext integrand or rate effected $rnext
# smooth    $cnext str         $cnext smoothing name
# $tend
#
# $end
def create_database(
	file_name,
	age_list,
	time_list,
	integrand_list,
	node_list,
	weight_list,
	covariate_list,
	data_list,
	prior_list,
	smooth_list,
	rate_list,
	mulcov_list
) :
	import dismod_at
	# -----------------------------------------------------------------------
	# create database
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	# -----------------------------------------------------------------------
	# Input Tables
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
	for row in integrand_list :
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
	for i in range( len(covariate_list) ) :
		row       = [
			covariate_list[i]['name'],
			covariate_list[i]['reference'],
			covariate_list[i]['max_difference']
		]
		row_list.append(row)
	tbl_name = 'covariate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_covariate_name2id = {}
	for i in range( len(covariate_list) ) :
		global_covariate_name2id[ covariate_list[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create node table
	global_node_name2id = {}
	for i in range( len(node_list) ) :
		global_node_name2id[ node_list[i]['name'] ] = i
	#
	col_name = [ 'node_name', 'parent' ]
	col_type = [ 'text',      'integer'   ]
	row_list = []
	for i in range( len(node_list) ) :
		node   = node_list[i]
		name   = node['name']
		parent = node['parent']
		if parent == '' :
			parent = -1
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
	for i in range( len( prior_list ) ) :
		prior         = prior_list[i]
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
	for i in range( len(weight_list) ) :
		weight = weight_list[i]
		name   = weight['name']
		n_age  = len( weight['age_id'] )
		n_time = len( weight['time_id'] )
		row_list.append( [ name, n_age, n_time ] )
	tbl_name = 'weight'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_weight_name2id = {}
	for i in range( len(weight_list) ) :
		global_weight_name2id[ weight_list[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create weight_grid table
	col_name = [  'weight_id', 'age_id',   'time_id',  'weight' ]
	col_type = [  'integer',   'integer',  'integer',  'real'   ]
	row_list = [ ]
	for i in range( len(weight_list) ) :
		weight  = weight_list[i]
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
	for i in range( len(smooth_list) ) :
		smooth        = smooth_list[i]
		name          = smooth['name']
		n_age         = len( smooth['age_id'] )
		n_time        = len( smooth['time_id'] )
		mulstd_value_prior_id = \
			global_prior_name2id[ smooth['mulstd_value_prior_name'] ]
		mulstd_dage_prior_id  = \
			global_prior_name2id[ smooth['mulstd_dage_prior_name']  ]
		mulstd_dtime_prior_id = \
			global_prior_name2id[ smooth['mulstd_dtime_prior_name'] ]
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
	for i in range( len(smooth_list) ) :
		global_smooth_name2id[ smooth_list[i]['name'] ] = i
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
	for i in range( len(smooth_list) ) :
		smooth  = smooth_list[i]
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
					da = -1
				if k == time_id[-1] :
					dt = -1
				row_list.append( [ i, j, k, v, da, dt] )
	tbl_name = 'smooth_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create rate table
	col_name = [  'rate_name', 'parent_smooth_id', 'child_smooth_id'  ]
	col_type = [  'text',      'integer',         'integer'          ]
	row_list = [ ]
	for i in range( len(rate_list) ) :
		rate             = rate_list[i]
		rate_name        = rate['name']
		parent_smooth_id = global_smooth_name2id[ rate['parent_smooth'] ]
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
	for i in range( len(mulcov_list) ) :
		mulcov          = mulcov_list[i]
		mulcov_type     = mulcov['type']
		effected        = mulcov['effected']
		if mulcov_type == 'rate_mean' :
			rate_id      = global_rate_name2id[ effected ]
			integrand_id = -1
		else :
			integrand_id = global_integrand_name2id[ effected ]
			rate_id      = -1
		covariate_id  = global_covariate_name2id[ mulcov['covariate'] ]
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
		'meas_value',
		'meas_std',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
	]
	for j in range( len(covariate_list) ) :
		col_name.append( 'x_%s' % j )
	col_type = [
		'integer',              # integrand_id
		'integer',              # density_id
		'integer',              # node_id
		'integer',              # weight_id
		'real',                 # meas_value
		'real',                 # meas_std
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
	]
	for j in range( len(covariate_list) )  :
		col_type.append( 'real' )
	row_list = [ ]
	for i in range( len(data_list) ) :
		data = data_list[i]
		data_id      = i
		integrand_id = global_integrand_name2id[ data['integrand'] ]
		density_id   = global_density_name2id[ data['density'] ]
		node_id      = global_node_name2id[ data['node'] ]
		weight_id    = global_weight_name2id[ data['weight'] ]
		row = [
			integrand_id,
			density_id,
			node_id,
			weight_id,
			data['meas_value'],
			data['meas_std'],
			data['age_lower'],
			data['age_upper'],
			data['time_lower'],
			data['time_upper']
		]
		for j in range( len(covariate_list) ) :
			row.append( data[ covariate_list[j]['name'] ] )
		row_list.append(row)
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# close the connection
	connection.commit()
	connection.close()
	return
