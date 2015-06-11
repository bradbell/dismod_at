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
# $begin get_row_list$$ $newlinech #$$
# $spell
#	unicode
#	dismod
#	str
#	tbl
# $$
#
# $section Get Data From a Table$$
#
# $head Syntax$$
# $icode%row_list% = dismod_at.get_row_list(
#	%connection%, %tbl_name%, %col_name%, %col_type%
# )%$$
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
#
# $head col_type$$
# is a list of strings, with length $icode n_col$$,
# and containing the types for the corresponding columns.
# The possible values for the column types are
# $code integer$$, $code real$$, $code text$$, or
# $code integer primary key$$.
#
# $head row_list$$
# This is a list of lists with length $icode n_row$$.
# In addition, for each $icode i$$, $icode%row_list%[%i%]%$$ is a list
# with length $icode n_col$$.
# If follows that $icode I$$ is also the number of rows in the table and that
# The value $icode%row_list%[%i%][%j%]%$$ corresponds to
# primary key $icode%tbl_name%_id = %i%$$,
# column $icode%col_name%[%j%]%$$ and has type specified by
# $icode%col_type%[%j%]%$$.
# Note that $code integer$$ and $code integer primary key$$ values
# are represented using $code int$$,
# $code real$$ values are represented using $code float$$,
# and $code text$$ values are represented using $code unicode$$.
#
# $children%example/table/get_row_list.py
# %$$
# $head Example$$
# The file $cref get_row_list.py$$ is an example use of
# $code get_row_list$$.
#
# $end
# ---------------------------------------------------------------------------
def get_row_list(connection, tbl_name, col_name, col_type) :
	import collections
	#
	convertor = {
		'integer primary key':int,
		'integer':int,
		'real':float,
		'text':unicode
	}
	cursor    = connection.cursor()
	n_col     = len(col_name)
	columns   = ','.join(col_name)
	cmd       = 'SELECT ' + columns + ' FROM ' + tbl_name
	count     = 0
	row_list  = list()
	for row in cursor.execute(cmd) :
		row_tmp = list()
		for j in range(n_col) :
			row_tmp.append( row[j] )
		row_list.append(row_tmp)
	return row_list
