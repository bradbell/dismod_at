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
#	%data_list%,
# )%$$
#
# $head Purpose$$
# This routine makes it easy to create all the 
# $cref input$$ tables in a $code dismod_at$$ database.
# It is not necessary to use this routine, nor is it necessary
# to use the same settings as this routine. For example,
# the column order and the primary key values need not be as chosen
# by this routine.
#
# $head file_name$$
# is as $code str$$ containing the name of the file where the data base 
# is stored.
# If this file already exists, it is deleted and a database is created.
#
# $head node_list$$
# This is a list of $code dict$$
# that define the rows of the $cref node_table$$. 
# If 
# $codei%
#	%name%   = node_list%[%i%]['name']
#	%parent% = node_list%[%i%]['parent']
# %$$ 
# $icode name$$ is a $code str$$ name for the $th i$$ node and
# $icode parent$$ is the $code int$$ for the corresponding parent node.
# If the $th i$$ node does not have a parent, $icode parent$$ is $code None$$. 
#
# $head weight_list$$
# This is a list of $code dict$$ that define the rows of the 
# $cref weight_grid$$ table and $cref weight_table$$. 
# If 
# $codei%
#	%name% = weight_list%[%i%]['name']
#	%age%  = weight_list%[%i%]['age']
#	%time% = weight_list%[%i%]['time']
#	%fun%  = weight_list%[%i%]['fun']
# %$$ 
# $icode name$$ is a $code str$$ name for the $th i$$ weighting function,
# $icode age$$ is a $code list$$ of $code float$$ ages,
# $icode time$$ is a $code list$$ of $code float$$ times, and
# $icode%fun%(%a%, %t%)%$$ is the weighting function.
#
# $head covariate_list$$
# This is a list of $code dict$$ 
# that define the rows of the $cref covariate_table$$.
# If 
# $codei%
#	%name%      = covariate_list%[%i%]['name']
#	%reference% = covariate_list%[%i%]['reference']
# %$$ 
# $icode name$$ is a $code str$$ name for the $th i$$ covariate and
# $icode reference$$ is the $code float$$ reference value for the covariate.
#
# $head data_list$$
# This is a list of $code dict$$ 
# that define the rows of the $cref data_table$$.
# If 
# $codei%
#	%integrand% = data_list%[%i%]['integrand']
#	%node%      = data_list%[%i%]['node']
# %$$ 
# $icode integrand$$ is $code str$$ integrand for the $th i$$ data point,
# $icode node$$ is $code str$$ node for the $th i$$ data point,
# $icode weight$$ is $code str$$ weight for the $th i$$ data point,
# $icode meas_value$$ is $code float$$ measurement for $th i$$ data point,
# 
# $end
def create_database(
	file_name,
	node_list,
	weight_list,
	covariate_list,
) :
	# -----------------------------------------------------------------------
	# primary key type
	ptype ='integer primary key'
	# -----------------------------------------------------------------------
	# create database
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
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
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create node table
	col_name = [ 'node_id', 'node_name', 'parent_id' ]
	col_type = [ ptype,     'text',      'integer'   ]
	row_list = []
	for i in range( len(node_list) ) :
		node = node_list[i]
		row_list.append( i, node['name'], node['parent'] )
	tbl_name = 'node'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create weight_grid table
	col_name = [   'weight_grid_id', 'weight_grid_name'   ]
	col_type = [   ptype,            'text'               ]
	row_list = [ ]
	for i in range( len(weight_list) ) :
		weight = weight_list[i]
		row_list.append( i, weight['name'])
	tbl_name = 'weight_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
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
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create covariate table
	col_name = [ 'covariate_id', 'covariate_name',	'reference' ]
	col_type = [ ptype,          'text',             'real'     ] 
	row_list = [ ]
	for i in range( len(covariate_list) ) :
		covariate = covariate_list[i]
		row_list.append( i, covariate['name'], covariate['reference'] )
	tbl_name = 'covariate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
