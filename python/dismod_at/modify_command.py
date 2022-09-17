# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin modify_command$$ $newlinech #$$
# $spell
#	Sqlite
#	dismod
#	dismodat.py
#	arg
#	exp
# $$
# $section Modify a Column of an Sqlite Database$$
#
# $head Deprecated$$
# This command was deprecated on 2021-12-05.
#
# $head Syntax$$
#
# $subhead As Program$$
# $codei%dismodat.py %database% %modify% \
#	%table_name% %column_name% %row_expression% %value_expression% \
#	%statement_one% %statement_two% %...%
# %$$
#
# $subhead As Python Function$$
# $codei%dismod_at.modify_command(%database%, %arg_list%)%$$
#
# $head See Also$$
# It is simpler to use the $cref set_command$$ when it is
# it is capable of making a change.
#
# $head Warning$$
# You can change any value in the database using the modify command.
# It is suggested that you re-run the $cref init_command$$,
# unless you are certain that the set of
# $cref model_variables$$ was not affected by this change.
#
# $head arg_list$$
# This is a list of strings containing the arguments in the
# program syntax. To be specific,
# $icode%arg_list%[0]%$$ is $icode table_name$$,
# $icode%arg_list%[1]%$$ is $icode column_name$$,
# e.t.c.
#
# $head database$$
# Is the file name corresponding to the Sqlite database.
#
# $head table_name$$
# Is the name of the table in the database that is modified.
#
# $head column_name$$
# Is the name of the column in the table that is modified.
#
# $head row_expression$$
# This is a python expression that evaluates to
# $code True$$ or $code False$$.
# Only rows where the expression is $code True$$ are modified.
#
# $head value_expression$$
# This is a python expression that specifies the value to be placed in the
# specified column when the row expression is $code True$$.
#
# $head Expressions$$
# For each row, an expression can reference the value in a column using
# the corresponding column name. In other words, for each column, a variable
# with the column name is set to the value in that column.
#
# $head statements$$
# There may be zero or more python statements at the end of the
# command line. Each one is executed before the expressions are evaluated.
# For example, one of the statements might be
# $codep
#	from math import exp
# $$
# This would make the function $codei%exp(%x%)%$$ available for use
# in $icode row_expression$$ and $icode value_expression$$.
#
# $head Log Table$$
# See $cref/dismodat.py/dismodat.py/Log Table/$$.
#
# $children%example/get_started/modify_command.py
# %$$
# $head Example$$
# The file $cref modify_command.py$$ contains an example and test
# using this command.
#
# $end
def modify_command(database, arg_list) :
	import re
	import os
	import sys
	import copy
	import dismod_at
	#
	# -------------------------------------------------------------------------
	# arguments
	assert len(arg_list) >= 4
	table_name       = arg_list[0]
	column_name      = arg_list[1]
	row_expression   = arg_list[2]
	value_expression = arg_list[3]
	i_arg = 4
	while i_arg < len(arg_list):
		exec( arg_list[i_arg] )
		i_arg += 1
	# -------------------------------------------------------------------------
	# replaces variable by _v_['variable']
	def replace_variable(expression, variable) :
		#
		ch_set = "([^a-zA-Z_'\"])"
		#
		pattern_in  = ch_set + variable + ch_set
		pattern_out = r"\1_v_['" + variable + r"']\2"
		expression_out = re.sub(pattern_in, pattern_out, expression)
		#
		pattern_in  = '^' + variable + ch_set
		pattern_out = r"_v_['" + variable + r"']\1"
		expression_out = re.sub(pattern_in, pattern_out, expression_out)
		#
		pattern_in  = ch_set + variable + '$'
		pattern_out = r"\1_v_['" + variable + r"']"
		expression_out = re.sub(pattern_in, pattern_out, expression_out)
		return expression_out
	# -------------------------------------------------------------------------
	# get the original value for the table
	new                  = False
	connection           = dismod_at.create_connection(database, new)
	(col_name, col_type) = dismod_at.get_name_type(connection, table_name)
	table_dict           = dismod_at.get_table_dict(connection, table_name)
	if not column_name in col_name :
		msg  = column_name + ' is not a column in table ' + table_name + '\n'
		msg += 'of database ' + database
		assert False, msg
	# -------------------------------------------------------------------------
	# map variable -> _v_['variable']
	primary_key   = table_name + '_id'
	primary_index = None
	count         = 0
	for col in col_name :
		row_expression   = replace_variable(row_expression,   col)
		value_expression = replace_variable(value_expression, col)
		if col == primary_key :
			primary_index = count
		count = count + 1
	# -------------------------------------------------------------------------
	# modify the values in the table
	count = 0
	for _v_ in table_dict :
		_v_[primary_key] = count
		count            = count + 1
		if eval( row_expression ) :
			_v_[column_name] = eval(value_expression)
	# -------------------------------------------------------------------------
	row_list = list()
	del col_name[primary_index]
	del col_type[primary_index]
	for row in table_dict :
		this_row = list()
		for col in col_name :
			this_row.append( row[col] )
		row_list.append(this_row)
	# -------------------------------------------------------------------------
	# delete the old version of the table
	cmd    = 'DROP TABLE ' + table_name
	cursor = connection.cursor()
	cursor.execute(cmd)
	# -------------------------------------------------------------------------
	# create the new version
	dismod_at.create_table(connection,table_name,col_name,col_type,row_list)
	# -------------------------------------------------------------------------
	connection.close()
