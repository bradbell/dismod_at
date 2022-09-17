# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin replace_table$$ $newlinech #$$
# $spell
#	dismod
#	str
#	tbl
# $$
#
# $section Replace A a Table$$
#
# $head Syntax$$
# $codei%dismod_at.replace_table(%connection%, %tbl_name%, %table_dict%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of a table that already in the
# database.
#
# $head table_dict$$
# is a $code list$$ with each element of the list is a dictionary.
# Each dictionary contains a key for every column name in the original table.
# The columns in the original table are written to the new table with the
# same order and types as in the original table. Only the values are replaced
# using the values in $icode table_dict$$.
#
# $head Primary Key$$
# This routine assumes the primary key is an integer,  corresponds
# to the first column, and has name $icode%tbl_name%_id%$$.
#
# $children%example/table/replace_table.py
# %$$
# $head Example$$
# The file $cref replace_table.py$$ is an example use of
# $code replace_table$$.
#
# $end
# ---------------------------------------------------------------------------
def replace_table(connection, tbl_name, table_dict) :
	import dismod_at
	#
	# col_name, col_type
	(col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
	#
	# remove primary key becasue it is automatically added bo create_table
	primary_key = tbl_name + '_id'
	assert col_name[0] == primary_key
	assert col_type[0] == 'integer primary key'
	del col_name[0]
	del col_type[0]
	#
	# remove the old table
	cmd    = 'DROP TABLE ' + tbl_name
	cursor = connection.cursor()
	cursor.execute(cmd)
	#
	# row_list
	row_list = list()
	for row in table_dict :
		this_row = list()
		for col in col_name :
			this_row.append( row[col] )
		row_list.append( this_row )
	#
	# write the new table
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	connection.commit()
