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
import os
import sys
import copy
from math import *
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
if len(sys.argv) != 6 :
	msg = 'bin/modify_column.py database table column condition expression'
	sys.exit(msg)
if sys.argv[0] != 'bin/modify_column.py' :
	msg = 'bin/modify_column.py must be executed from its parent directory'
	sys.exit(msg)
#
database_arg   = sys.argv[1]
table_arg      = sys.argv[2]
column_arg     = sys.argv[3]
condition_arg  = sys.argv[4]
expression_arg = sys.argv[5]
# ---------------------------------------------------------------------------
# Extract the table from the data base
new                  = False
connection           = dismod_at.create_connection(database_arg, new)
(col_name, col_type) = dismod_at.get_name_type(connection, table_arg)
table_dict           = dismod_at.get_table_dict(connection, table_arg)
if not column_arg in col_name :
	msg  = column_arg + ' is not a column in table ' + table_arg + '\n'
	msg += 'of database ' + database_arg
	sys.exit(msg)
# ---------------------------------------------------------------------------
# modify the expression to use dictionary to access columns
expression  = copy.copy(expression_arg)
condition   = copy.copy(condition_arg)
primary_key = table_arg + '_id'
count       = 0
for col in col_name :
	expression = expression.replace(col, "row['" + col + "']")
	condition  = condition.replace(col, "row['" + col + "']")
	if col == primary_key :
		primary_index = count
	count += 1
# ---------------------------------------------------------------------------
# modify the column in the table
count       = 0
for row in table_dict :
	row[primary_key] = count
	count += 1
	if eval(condition) :
		row[column_arg] = eval(expression)
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
cmd    = 'DROP TABLE ' + table_arg
cursor = connection.cursor()
cursor.execute(cmd)
# ---------------------------------------------------------------------------
# create the new version
dismod_at.create_table(connection, table_arg, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
print('modify_column.py: OK')
sys.exit(0)

