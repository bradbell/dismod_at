# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
import sqlite3
# ============================================================================
# $begin unicode_tuple$$ $newlinech #$$
# $spell
#	iterable
#	unicode
#	tuple
#	str
#	dismod
#	sqlite
# $$
# $index convert, iterable to string$$
# $index string, version of iterable$$
#
# $section Convert an Iterable Object to a Unicode String$$
#
# $head Syntax$$
# $icode%u_tuple% = dismod_at.unicode_tuple(%iterable%, %quote_string%)%$$
#
# $head iterable$$
# is any object where we can iterate over its elements
# and convert each element to a unicode value.
#
# $head quote_string$$
# is either $code True$$ or $code False$$. 
# If it is $code True$$, and an element is a $code str$$ or $code unicode$$,
# it is surrounded by the single quote character.
#
# $head None$$
# The value $code None$$ is converted to the unicode string $code null$$
# (always without quotes).
#
# $head Infinity$$
# If $icode u_j$$ is a $code float$$ and equal to plus infinity,
# the corresponding tuple value has an exponent that is ten times the 
# exponent for the maximum float. 
# This is so that, when written by pythons $code sqlite$$ package, it 
# gets converted to infinity. 
# Minus infinity is handled in a similar fashion.
#
# $head u_tuple$$
# is a unicode string representation of the tuple containing the elements.
# To be specific it is given by
# $codei%
#	%u_tuple% = u'( %u_1%, %u_2%, %...%, %u_n% )'
# %$$
# where $icode n$$ is the number of elements 
# and $icode u_1$$ is the $th i$$ element.
# 
# $end
# --------------------------------------------------------------------------
def unicode_infinity() :
	import sys
	import math
	exponent = int( math.log( sys.float_info.max ) / math.log(10.) ) + 10
	inf      = u'10e' + unicode(exponent)
	return inf
def unicode_tuple(iterable, quote_string) :
	u_tuple = u'(' 
	count   = 0
	for element in iterable :
		if count > 0 :
			u_tuple += u','
		count += 1
		#
		if element == None :
			u_tuple += u' null'
		elif isinstance(element, float) and element == float('inf') :
			u_tuple += u' ' + unicode_infinity()
		elif isinstance(element, float) and element == - float('inf') :
			u_tuple += u' -' + unicode_infinity()
		elif quote_string and isinstance(element, basestring) :
			u_tuple += u' ' + u"'" + unicode(element) + u"'"
		else :
			u_tuple += u' ' + unicode(element)
	u_tuple     += u' )'
	return u_tuple
# ============================================================================
# $begin create_connection$$ $newlinech #$$
# $spell
#	str
#	sqlite
#	dismod
# $$
# $index create_connection, database$$
# $index connection, database create$$
# $index database, create_connection$$
#
# $section Create a Python sqlite3 Database Connection$$
#
# $head Syntax$$
# $icode%connection% = dismod_at.create_connection(%file_name%, %new%)
# %$$
#
# $head file_name$$
# is as $code str$$ containing the name of the file where the data base 
# is stored.
# 
# $head new$$
# is either $code True$$ or $code False$$.
# If it is true, and a data base with the same name already exists,
# the existing database is deleted before creating the connection.
#
# $head connection$$
# The return value is an sqlite3
# $href%https://docs.python.org/2/library/sqlite3.html#connection-objects
#	%connection object
# %$$
#
# $end
# ---------------------------------------------------------------------------
def create_connection(file_name, new) :
	import os
	if new and os.path.isfile(file_name) :
		os.remove(file_name)
	connection = sqlite3.connect(file_name)
	return connection
# ==========================================================================-
# $begin get_name2type$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
# $index table, column names and type$$
# $index name, table column$$
# $index type, table column$$
# $index column, name and type$$
#
# $section Get Column Names and Types$$
#
# $head Syntax$$
# $icode%col_name2type% = dismod_at.get_name2type(%connection%, %tbl_name%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name2type$$
# The return value is an ordered dictionary where the order is the
# same as the order of the columns in the table.
# The key is the column name and 
# the value is one of the following:
# $code integer$$, $code real$$, $code text$$, or
# $code integer primary key$$.
# 
# $end
# ---------------------------------------------------------------------------
def get_name2type(connection, tbl_name) :
	import collections
	#
	cmd        = 'pragma table_info(' + tbl_name + ');'
	cursor    = connection.cursor()
	cid       = 0
	found_pk  = False
	col_name2type = collections.OrderedDict()
	for row in cursor.execute(cmd) :
		assert cid == row[0]
		cid       += 1
		#
		key        = row[1]
		value      = row[2]
		pk         = row[5]
		if pk == 1 :
			assert found_pk == False
			assert value == 'integer'
			value           =  'integer primary key'
			found_ok        = True
		col_name2type[key]  =  value
	return col_name2type
# ==========================================================================-
# $begin create_table$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
# $index create_table, database$$
# $index table, database create$$
# $index database, create_table$$
#
# $section Create a Database Table$$
#
# $head Syntax$$
# $codei%dismod_at.create_table(
#	%connection%, %tbl_name%, %col_name%, %col_type%, %row_list% 
# )%$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head tbl_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name$$
# is a $code list$$ of $code str$$
# where the elements are the column names in the table that is created.
#
# $subhead tbl_name_id$$
# The column $icode%tbl_name%_id%$$ is added as the first column
# of the table and should not be included in $icode col_name$$.
#
# $head col_type$$
# is a $code list$$ of $code str$$ where the elements are the column types
# in the same order as $icode col_name$$.
# The valid types are
# $code integer$$, $code real$$, $code text$$.
#
# $subhead tbl_name_id$$
# The column with name $icode%tbl_name%_id%$$ will have type
# $code integer primary key$$.
#
# $head row_list$$
# is a possibly empty $code list$$ of rows contain data that is written 
# to the table.
# Each row is itself a list containing the data for one row of the
# table in the same order as $icode col_name$$.
# Note that the special value $code None$$ gets converted to $code null$$.
#
# $subhead tbl_name_id$$
# The column with name $icode%tbl_name%_id%$$ will have value starting
# with zero for the first row and incrementing by one for each row.
#
#
# $children%example/table/create_table.py 
# %$$
# $head Example$$
# The file $cref create_table.py$$ is an example use of
# $code create_table$$.
#
# $end
# ---------------------------------------------------------------------------
def create_table(connection, tbl_name, col_name, col_type, row_list) :
	import copy
	#
	cmd       = 'create table ' + tbl_name + '('
	n_col     = len( col_name )
	cmd      += '\n\t' + tbl_name + '_id integer primary key'
	for j in range(n_col) :
		cmd   += ',\n\t' + col_name[j] + ' ' + col_type[j]
	cmd += '\n\t);'
	#
	cursor  = connection.cursor()
	cursor.execute(cmd)
	#
	quote_text = True
	for i in range( len(row_list) ) :
		row_cpy     = copy.copy(row_list[i])
		row_cpy.insert(0, i)
		value_tuple = unicode_tuple(row_cpy, quote_text)
		cmd = 'insert into ' + tbl_name + ' values ' + value_tuple
		cursor.execute(cmd)
# ==========================================================================-
# $begin create_database$$ $newlinech #$$
# $spell
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
#	%mulcov_list%,
#	%run_list%
# )%$$
#
# $head Purpose$$
# This routine makes it easy to create all the 
# $cref input$$ tables in a $code dismod_at$$ database.
# This is only meant for small example and testing cases and is not efficient.
#
# $head file_name$$
# is as $code str$$ containing the name of the file where the data base 
# is stored.
# If this file already exists, it is deleted and a database is created.
#
# $head age_list$$
# is a $code list$$ of $code float$$ in increasing order that
# specify the age grid values.
#
# $head time_list$$
# is a $code list$$ of $code float$$ in increasing order that
# specify the time grid values.
#
# $head integrand_list$$
# This is a list of $code dict$$
# that define the rows of the $cref integrand_table$$.
# The dictionary $icode%integrand_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type $cnext Description                     $rnext
# name    $cnext str        $cnext name for the $th i$$ integrand  $rnext
# eta     $cnext str        $cnext offset in log transform fro this integrand
# $tend
#
# $head node_list$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$.
# The dictionary $icode%node_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $cnext Description                $rnext
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
# Key     $cnext Value Type    $cnext Description                $rnext
# name    $cnext str           $cnext name of $th i$$ weighting  $rnext
# age_id  $cnext list of int   $cnext indices for age values     $rnext
# time_id $cnext list of int   $cnext grid for time values       $rnext
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
# Key       $cnext Value Type  $cnext Description                     $rnext
# name      $cnext str         $cnext name for the $th i$$ covariate  $rnext
# reference $cnext float       $cnext reference value for $th i$$ covariate
# $tend
#
# $head data_list$$
# This is a list of $code dict$$ 
# that define the rows of the $cref data_table$$.
# The dictionary $icode%data_list%[%i%]%$$ has the following:
# $table
# Key          $cnext Value Type  $cnext Description                 $rnext 
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
# Key     $cnext Value Type    $cnext Description                $rnext 
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
# Key     $cnext Value Type  $cnext Description                $rnext
# name    $cnext str         $cnext name of $th i$$ smoothing  $rnext
# age_id  $cnext list of int $cnext indices for age values     $rnext
# time_id $cnext list of int $cnext indices for time values    $rnext
# mulstd_value   $cnext str   $cnext 
#	name for $cref/mulstd_value/smooth_table/mulstd_value/$$ smoothing $rnext
# mulstd_dage   $cnext str   $cnext 
#	name for $cref/mulstd_dage/smooth_table/mulstd_dage/$$ smoothing $rnext
# mulstd_dtime   $cnext str   $cnext 
#	name for $cref/mulstd_dtime/smooth_table/mulstd_dtime/$$ smoothing $rnext
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
# Key           $cnext Value Type  $cnext Description                  $rnext
# name          $cnext str     $cnext pini, iota, rho, chi, or omega   $rnext
# parent_smooth $cnext str     $cnext parent smoothing                 $rnext
# child_smooth  $cnext str     $cnext child smoothing
# $tend
# Furthermore the order of the rate names must be
# pini, iota, rho, chi, omega; i.e., the order in $cref rate_table$$.
#
# $head mulcov_list$$
# This is a list of $code dict$$
# that define the rows of the $cref mulcov_table$$.
# The dictionary $icode%mulcov_list%[%i%]%$$ has the following:
# $table
# Key       $cnext Value Type  $cnext Description                $rnext
# covariate $cnext str         $cnext is the covariate column    $rnext
# type      $cnext str  $cnext rate_mean, meas_mean, or meas_std $rnext
# effected  $cnext str         $cnext integrand or rate effected $rnext
# smooth    $cnext str         $cnext smoothing name
# $tend
#
# $head run_list$$
# This is a list of $code dict$$
# that define the rows of the $cref run_table$$.
# The dictionary $icode%run_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $cnext Description                $rnext
# parent_node       $cnext int $cnext name of parent for this analysis $rnext
# pini_smooth     $cnext int $cnext 
#	name of initial prevalence smoothing $rnext
# ode_step_size     $cnext double
#	$cnext used to approximation ODE solution $rnext
# n_sample          $cnext int $cnext number of posterior
#	$cref/samples/post_table/sample/$$
# $tend
# Note that if the i-th node does not have a parent, the empty string
# should be used for the parent of that node.
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
	mulcov_list,
	run_list
) :
	# -----------------------------------------------------------------------
	# create database
	new            = True
	connection     = create_connection(file_name, new)
	# -----------------------------------------------------------------------
	# create age table
	col_name = [ 'age' ]
	col_type = [ 'real' ]
	row_list = []
	for age in age_list :
		row_list.append( [age] )
	tbl_name = 'age'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create time table
	col_name = [ 'time' ]
	col_type = [ 'real' ]
	row_list = []
	for time in time_list :
		row_list.append( [time] )
	tbl_name = 'time'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create integrand table
	col_name = [ 'integrand_name', 'eta' ]
	col_type = [ 'text',           'real']
	row_list = []
	for row in integrand_list :
		row_list.append( [ row['name'], row['eta'] ]  )
	tbl_name = 'integrand'
	create_table(connection, tbl_name, col_name, col_type, row_list)
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_density_name2id = {}
	for i in range( len(row_list) ) :
		global_density_name2id[ row_list[i][0] ] = i
	# ------------------------------------------------------------------------
	# create covariate table
	col_name = [ 'covariate_name',	'reference' ]
	col_type = [ 'text',             'real'     ] 
	row_list = [ ]
	for i in range( len(covariate_list) ) :
		covariate = covariate_list[i]
		row_list.append( [ covariate['name'], covariate['reference'] ] )
	tbl_name = 'covariate'
	create_table(connection, tbl_name, col_name, col_type, row_list)
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create the prior table
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_weight_name2id = {}
	for i in range( len(weight_list) ) :
		global_weight_name2id[ weight_list[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create weight grid table
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create smooth table
	col_name = [ 'smooth_name', 'n_age',   'n_time', 'mulstd_value',
		'mulstd_dage', 'mulstd_dtime'    ]
	col_type = [ 'text',        'integer', 'integer', 'integer',
  		'integer',       'integer'           ]
	row_list = [ ]
	for i in range( len(smooth_list) ) :
		smooth        = smooth_list[i]
		name          = smooth['name']
		n_age         = len( smooth['age_id'] )
		n_time        = len( smooth['time_id'] )
		mulstd_value   = global_prior_name2id[ smooth['mulstd_value'] ]
		mulstd_dage    = global_prior_name2id[ smooth['mulstd_dage']  ]
		mulstd_dtime   = global_prior_name2id[ smooth['mulstd_dtime'] ]
		row_list.append( [ 
		name, n_age, n_time, mulstd_value, mulstd_dage, mulstd_dtime   
		] )
	tbl_name = 'smooth'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_smooth_name2id = {}
	for i in range( len(smooth_list) ) :
		global_smooth_name2id[ smooth_list[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create smooth grid table
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	global_rate_name2id = {}
	for i in range( len(row_list) ) :
		global_rate_name2id[ row_list[i][0] ] = i
	# ------------------------------------------------------------------------
	# mulcov table
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create the data table
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
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create run table
	col_name = [ 
		'parent_node_id','pini_smooth_id', 'ode_step_size','n_sample'
	]
	col_type = [ 
		'integer',       'integer',        'real',         'integer'
	]
	row_list = []
	for run in run_list :
		parent_node_id     = global_node_name2id[ run['parent_node' ] ]
		pini_smooth_id     = global_smooth_name2id[ run['pini_smooth'] ]
		ode_step_size      = run['ode_step_size']
		n_sample           = run['n_sample']
		row_list.append( [ 
			parent_node_id, pini_smooth_id, ode_step_size, n_sample 
		] )
	tbl_name = 'run'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	return
