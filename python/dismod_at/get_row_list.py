# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_row_list$$ $newlinech #$$
# $spell
#	Sql
#	dismod
#	str
#	tbl
# $$
#
# $section Get Data From a Table$$
#
# $head Syntax$$
# $icode%row_list% = dismod_at.get_row_list(%$$
# $icode%connection%, %tbl_name%, %col_name%)%$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
# We use the notation $icode n_row$$ for the number of rows in
# the table.
#
# $head col_name$$
# is a list of strings containing the names of the columns that we are
# retrieving data from.
# We use the notation $icode n_col$$ for the number of columns in
# $icode col_name$$.
# You can determine the name of all the columns in the table using
# $cref get_name_type$$.
#
# $head row_list$$
# This is a list, with length $icode n_row$$,
# where each element of the list is a list.
# In addition, for each $icode i$$, $icode%row_list%[%i%]%$$ is a list
# with length $icode n_col$$.
# The value $icode%row_list%[%i%][%j%]%$$ corresponds to
# primary key $icode%tbl_name%_id = %i%$$,
# and column $icode%col_name%[%j%]%$$.
# The python type corresponding to the values in the table are
# as follows:
# $table
# Sql Table $pre  $$ $cnext  Python           $rnext
# $code integer$$    $cnext  $code int$$      $rnext
# $code real$$       $cnext  $code float$$    $rnext
# $code text$$       $cnext  $code str$$      $rnext
# $code null$$       $cnext  $code None$$
# $tend
# You can determine the type for all the columns in the table using
# $cref get_name_type$$.
# Note that the type $code integer primary key$$ corresponds to
# $code integer$$ above.
#
# $children%example/table/get_row_list.py
# %$$
# $head Example$$
# The file $cref get_row_list.py$$ is an example use of
# $code get_row_list$$.
#
# $end
# ---------------------------------------------------------------------------
def get_row_list(connection, tbl_name, col_name) :
	import collections
	#
	cursor      = connection.cursor()
	n_col       = len(col_name)
	columns     = ','.join(col_name)
	primary_key = tbl_name + '_id'
	cmd         = 'SELECT ' + columns + ' FROM ' + tbl_name
	cmd        += ' ORDER BY ' + primary_key
	row_list  = list()
	for row in cursor.execute(cmd) :
		row_tmp = list()
		for j in range(n_col) :
			row_tmp.append( row[j] )
		row_list.append(row_tmp)
	return row_list
