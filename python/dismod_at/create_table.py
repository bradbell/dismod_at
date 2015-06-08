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
# $begin create_table$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
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
	import dismod_at
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
		value_tuple = dismod_at.unicode_tuple(row_cpy, quote_text)
		cmd = 'insert into ' + tbl_name + ' values ' + value_tuple
		cursor.execute(cmd)
