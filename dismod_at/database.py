# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2013-14 University of Washington
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
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name$$
# is a $code list$$ of $code str$$
# where the elements are the column names in the table that is created.
#
# $subhead col_type$$
# is a $code list$$ of $code str$$ where the elements are the column types
# in the same order as $icode col_name$$.
# The valid types are
# $code integer$$, $code real$$, $code text$$, and
# $code integer primary key$$.
# There must be one, and only one, column with type
# $code integer primary key$$.
#
# $subhead row_list$$
# is a possibly empty $code list$$ of rows contain data that is written 
# to the table.
# Each row is itself a list containing the data for one row of the
# table in the same order as $icode col_name$$.
# Note that the special value $code None$$ gets converted to $code null$$.
#
# $comment%example/create_table.py
# %$$
# $head Example$$
# The file $comment create_table.py$$ creates an example use of
# $code create_table$$.
#
# $end
# ---------------------------------------------------------------------------
def create_table(connection, tbl_name, col_name, col_type, row_list) :
	#
	cmd       = 'create table ' + tbl_name + '('
	n_col     = len( col_name )
	for j in range(n_col) :
		cmd   += '\n\t' + col_name[j] + ' ' + col_type[j]
		if j < n_col - 1 :
			cmd += ','
	cmd += '\n\t);'
	#
	cursor  = connection.cursor()
	cursor.execute(cmd)
	#
	quote_text = True
	for row in row_list :
		value_tuple = unicode_tuple(row, quote_text)
		cmd = 'insert into ' + tbl_name + ' values ' + value_tuple
		cursor.execute(cmd)
# ==========================================================================-
# $begin create_database$$ $newlinech #$$
# $spell
#	dismod
#	covariate
#	std
#	str
# $$
#
# $section Under Construction: Create a Dismod_at Database$$
#
# $head Syntax$$
# $codei%create_database(
#	%file_name%,
#	%node_list%,
#	%weight_list%,
#	%covariate_list%,
#	%data_list%
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
# $head node_list$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$.
# The dictionary $icode%node_list%[%i%]%$$ has the following:
# $table
# Key     $cnext Value Type    $cnext Description                $rnext
# name    $cnext str           $cnext name for the $th i$$ node  $rnext
# parent  $cnext str           $cnext name ofr parent of the $th i$$ node
# $tend
#
# $head weight_list$$
# This is a list of $code dict$$
# that define the rows of the $cref weight_grid$$ table and
# $cref weight_table$$.
# The dictionary $icode%weight_list%[%i%]%$$ has the following:
# $table
# Key    $cnext Value Type    $cnext Description                $rnext
# name   $cnext str           $cnext name of $th i$$ weighting  $rnext
# age    $cnext list of float $cnext grid for age values        $rnext
# time   $cnext list of float $cnext grid for time values       $rnext
# fun    $cnext function      $cnext %icode%w%=%fun%(%a%, %t%) 
# $tend
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
# node         $cnext str         $cnext node in graph               $rnext 
# weight       $cnext str         $cnext weighting function          $rnext 
# meas_value   $cnext float       $cnext measured value              $rnext 
# meas_std     $cnext float       $cnext standard deviation          $rnext 
# meas_density $cnext str         $cnext density function            $rnext 
# meas_eta     $cnext float       $cnext density function            $rnext 
# age_lower    $cnext float       $cnext lower age limit             $rnext 
# age_upper    $cnext float       $cnext upper age limit             $rnext 
# time_lower   $cnext float       $cnext lower time limit            $rnext
# time_lower   $cnext float       $cnext upper time limit
# $tend
# 
# $end
def create_database(
	file_name,
	node_list,
	weight_list,
	covariate_list,
	data_list
) :
	# -----------------------------------------------------------------------
	# primary key type
	ptype ='integer primary key'
	# -----------------------------------------------------------------------
	# create database
	new            = True
	connection     = create_connection(file_name, new)
	# ------------------------------------------------------------------------
	# create integrand table
	col_name = [  'integrand_id', 'integrand_name'  ]
	col_type = [   ptype,          'text'           ]
	row_list = [ 
		[ None,   'incidence'   ],
		[ None,   'remission'   ],
		[ None,   'mtexcess'    ],
		[ None,   'mtother'     ],
		[ None,   'mtwith'      ],
		[ None,   'prevalence'  ],
		[ None,   'mtspecific'  ],
		[ None,   'mtall'       ],
		[ None,   'mtstandard'  ],
		[ None,   'relrisk'     ]
	]
	tbl_name = 'intergrand'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_integrand_name2id = {}
	for i in range( len(row_list) ) :
		global_integrand_name2id[ row_list[i][1] ] = i
	# ------------------------------------------------------------------------
	# create node table
	global_node_name2id = {}
	for i in range( len(node_list) ) :
		global_node_name2id[ node_list[i]['name'] ] = i
	#
	col_name = [ 'node_id', 'node_name', 'parent_id' ]
	col_type = [ ptype,     'text',      'integer'   ]
	row_list = []
	for i in range( len(node_list) ) :
		node   = node_list[i]
		name   = node['name']
		parent = node['parent']
		if parent!= None :
			parent = global_node_name2id[parent]
		row_list.append( [ i, name, parent ] )
	tbl_name = 'node'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create weight_grid table
	col_name = [   'weight_grid_id', 'weight_grid_name'   ]
	col_type = [   ptype,            'text'               ]
	row_list = [ ]
	for i in range( len(weight_list) ) :
		weight = weight_list[i]
		row_list.append( [ i, weight['name'] ] )
	tbl_name = 'weight_grid'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	global_weight_grid_name2id = {}
	for i in range( len(weight_list) ) :
		global_weight_grid_name2id[ weight_list[i]['name'] ] = i
	# ------------------------------------------------------------------------
	# create weight table
	col_name = [  'weight_id', 'weight_grid_id', 'age',   'time',  'weight' ]
	col_type = [  ptype,       'integer',        'real',  'real',  'real'   ]
	row_list = [ ]
	for i in range( len(weight_list) ) :
		weight = weight_list[i]
		age    = weight['age']
		time   = weight['time']
		fun    = weight['fun']
		for a in age :
			for t in time :
				w = fun(a, t)
				row_list.append( [ None, i, a, t, w] )
	tbl_name = 'weight'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create covariate table
	col_name = [ 'covariate_id', 'covariate_name',	'reference' ]
	col_type = [ ptype,          'text',             'real'     ] 
	row_list = [ ]
	for i in range( len(covariate_list) ) :
		covariate = covariate_list[i]
		row_list.append( [ i, covariate['name'], covariate['reference'] ] )
	tbl_name = 'covariate'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create the data table
	col_name = [
		'data_id',
		'integrand_id',
		'node_id',
		'weight_grid_id',
		'meas_value',
		'meas_std',
		'meas_density',
		'meas_eta',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
	]
	col_type = [
		ptype,                  # data_id
		'integer',              # integrand_id
		'integer',              # node_id
		'integer',              # weight_grid_id
		'real',                 # meas_value
		'real',                 # meas_std
		'text',                 # meas_density
		'real',                 # meas_eta
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
	]
	row_list = [ ]
	for i in range( len(data_list) ) :
		data = data_list[i]
		data_id      = i
		integrand_id = global_integrand_name2id[ data['integrand'] ]
		node_id      = global_node_name2id[ data['node'] ]
		weight_grid_id = global_weight_grid_name2id[ data['weight'] ]
		row = [ 
			data_id,
			integrand_id,
			node_id,
			weight_grid_id,
			data['meas_value'],
			data['meas_std'],
			data['meas_density'],
			data['meas_eta'],
			data['age_lower'],
			data['age_upper'],
			data['time_lower'],
			data['time_upper']
		]
		row_list.append(row)
	tbl_name = 'data'
	create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	return
