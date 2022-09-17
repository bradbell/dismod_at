# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin create_table$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
#	unicode
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
# The column name for the primary key, $icode%tbl_name%_id%$$ must
# not be included in the list.
#
# $subhead unique$$
# If the column name $icode%tbl_name%_name%$$ is in the list,
# the corresponding column will have the $code unique$$ constraint.
#
# $head col_type$$
# is a $code list$$ of $code str$$ where the elements are the column types
# in the same order as $icode col_name$$.
# The valid types are
# $code integer$$, $code real$$, $code text$$.
#
# $head row_list$$
# is a possibly empty $code list$$ of rows contain data that is written
# to the table.
# Each row is itself a list containing the data for one row of the
# table in the same order as $icode col_name$$.
# Note that the primary key column is not included in $icode row_list$$.
# Also note that the value in each column gets converted to unicode
# before being written to the database.
# Note that the special value
# $codei%
#	%row_list%[%i%][%j%] ==  None
# %$$
# gets converted to $code null$$ in the table.
#
# $head tbl_name_id$$
# A column with name $icode%tbl_name%_id%$$ and type
# $code integer primary key$$ is included as the first column in the table.
# Its values start with zero (for the first row) and
# increment by one for each row.
#
# $head Side Effects$$
# This routine does a
# $codei%
#	%connection%.commit()
# %$$
# to make sure the table exists before returning.
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
	primary_key = tbl_name + '_id'
	name_column = tbl_name + '_name'
	#
	cmd       = 'create table ' + tbl_name + '('
	n_col     = len( col_name )
	cmd      += '\n\t' + tbl_name + '_id integer primary key'
	for j in range(n_col) :
		assert col_name != primary_key
		cmd   += ',\n\t' + col_name[j] + ' ' + col_type[j]
		if col_name[j] == name_column :
			cmd += ' unique'
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
	connection.commit()
