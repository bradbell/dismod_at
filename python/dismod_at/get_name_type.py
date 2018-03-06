# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_name_type$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
#
# $section Get Column Names and Types in a Table$$
#
# $head Syntax$$
# $codei%(%col_name%, %col_type%)%$$
# $codei% = dismod_at.get_name_type(%connection%, %tbl_name%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name$$
# is a list of strings containing the column names in the same order as they
# appear in the table.
#
# $head col_type$$
# is a list of strings containing the column types in the same order as
# the columns appear in the table.
# The possible values for the column types are
# $code integer$$, $code real$$, $code text$$, or
# $code integer primary key$$.
#
# $head Primary Key$$
# This routine assumes the primary key is an integer,  corresponds
# to the first column, and has name $icode%tbl_name%_id%$$.
#
# $children%example/table/get_name_type.py
# %$$
# $head Example$$
# The file $cref get_name_type.py$$ is an example use of
# $code get_name_type$$.
#
# $end
# ---------------------------------------------------------------------------
def get_name_type(connection, tbl_name) :
	import collections
	import sys
	#
	cmd        = 'pragma table_info(' + tbl_name + ');'
	cursor    = connection.cursor()
	cid       = 0
	found_pk  = False
	col_name  = list()
	col_type  = list()
	for row in cursor.execute(cmd) :
		assert cid == row[0]
		#
		col_name.append(row[1])
		col_type.append(row[2])
		pk            = row[5]
		if cid == 0 :
			if pk != 1 :
				sys.exit(tbl_name + ' table: first column not primary key')
			assert found_pk == False
			assert col_type[cid].lower() == 'integer'
			assert col_name[cid] == (tbl_name + '_id')
			col_type[cid]  =  'integer primary key'
			found_ok       = True
		else :
			if pk != 0 :
				sys.exit(tbl_name + ' table: muiltiple columns in primary key')
		cid += 1
	return (col_name, col_type)
