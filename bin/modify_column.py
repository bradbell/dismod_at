#! /usr/bin/python3
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
'''
$begin modify_column.py$$
$escape $$

$section Modify a Specific Column of an Sqlite Database$$

$head Syntax$$
$codei%bin/modify_column.py \
	%database% \
	%table_name% \
	%column_name% \
	%row_expression% \
	%value_expression% \
	%python_statement% \
	%python_statement% \
	%...%
%$$

$head database$$
Is the file name corresponding to an Sqlite database.

$head table_name$$
Is the name of the table in the database where the column appears.

$head column_name$$
Is the name of the column that is being modified.

$head row_expression$$
This expression evaluates to true or false.
Only rows where the expression is true are modified.

$head value_expresssion$$
This expression contains the value that should be placed in the
specified column when the row expression is true.

$head Expressions$$
The expressions can reference the value in any of the columns using
the corresponding column name.

$head pthon_statement$$
There may be zero or more python statements at the end of the
command line. Each one is executed at the global level
before the expressions are evaluated.
For example, one of the statements might be
$codep
	from math import exp
$$

$end
'''
import re
import os
import sys
import copy
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
if len(sys.argv) < 6 :
	msg  = 'bin/modify_column.py database'
	msg += ' table_name column_name row_expression value_expression'
	sys.exit(msg)
if sys.argv[0] != 'bin/modify_column.py' :
	msg = 'bin/modify_column.py must be executed from its parent directory'
	sys.exit(msg)
#
database_arg      = sys.argv[1]
table_name        = sys.argv[2]
column_name       = sys.argv[3]
row_expression    = sys.argv[4]
value_expression  = sys.argv[5]
i_arg = 6
while i_arg < len(sys.argv) :
	exec( sys.argv[i_arg] )
	i_arg += 1
# ---------------------------------------------------------------------------
def row_index(expression_in, var) :
	#
	pattern_in  = '([^a-zA-Z_])' + var + '([^a-zA-Z_])'
	pattern_out = r"\1row['" + var + r"']\2"
	expression_out = re.sub(pattern_in, pattern_out, expression_in)
	#
	pattern_in  = '^' + var + '([^a-zA-Z_])'
	pattern_out = r"row['" + var + r"']\1"
	expression_out = re.sub(pattern_in, pattern_out, expression_out)
	#
	pattern_in  = '([^a-zA-Z_])' + var + '$'
	pattern_out = r"\1row['" + var + r"']"
	expression_out = re.sub(pattern_in, pattern_out, expression_out)
	return expression_out
# ---------------------------------------------------------------------------
# Extract the table from the data base
new                  = False
connection           = dismod_at.create_connection(database_arg, new)
(col_name, col_type) = dismod_at.get_name_type(connection, table_name)
table_dict           = dismod_at.get_table_dict(connection, table_name)
if not column_name in col_name :
	msg  = column_name + ' is not a column in table ' + table_name + '\n'
	msg += 'of database ' + database_arg
	sys.exit(msg)
# ---------------------------------------------------------------------------
# modify the expression to use dictionary to access columns
expression  = copy.copy(value_expression)
condition   = copy.copy(row_expression)
primary_key = table_name + '_id'
count       = 0
for col in col_name :
	expression = row_index(expression, col)
	condition  = row_index(condition, col)
	if col == primary_key :
		primary_index = count
	count += 1
print(expression)
print(condition)
# ---------------------------------------------------------------------------
# modify the column in the table
count       = 0
for row in table_dict :
	row[primary_key] = count
	count += 1
	if eval(condition) :
		row[column_name] = eval(expression)
# ---------------------------------------------------------------------------
# data needed to create new table
row_list = list()
del col_name[primary_index]
del col_type[primary_index]
for row in table_dict :
	this_row = list()
	for col in col_name :
		this_row.append( row[col] )
	row_list.append(this_row)
# ---------------------------------------------------------------------------
# delete the old version of the table
cmd    = 'DROP TABLE ' + table_name
cursor = connection.cursor()
cursor.execute(cmd)
# ---------------------------------------------------------------------------
# create the new version
dismod_at.create_table(connection, table_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
print('modify_column.py: OK')
sys.exit(0)

