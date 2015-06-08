# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_name2type$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
#
# $section Get Column Names and Types in a Table$$
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
# $children%example/table/get_name2type.py
# %$$
# $head Example$$
# The file $cref get_name2type.py$$ is an example use of
# $code get_name2type$$.
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
