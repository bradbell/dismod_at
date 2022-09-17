// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin blob_table$$
$spell
	sizeof
	sqlite
	const
	std
$$

$section Writing and Reading Blobs$$

$head Syntax$$
$codei%write_blob_table(%db%, %table_name%, %col_name%, %sizeof_data%, %data%)
%$$
$codei%read_blob_table(%db%, %table_name%, %col_name%, %sizeof_data%, %data%)
%$$

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
and is the name of the table we are reading or writing.

$head col_name$$
This argument has prototype
$codei%
	const std::string& %col_name%
%$$
and is the name of the column that we are reading or writing.

$head sizeof_data$$
This argument has prototype
$codei%
	size_t& %sizeof_data%
%$$
This is the number of bytes of data that we are reading or writing.
If this is a write (read) then $icode sizeof_data$$ is (is not) $code const$$.
If this is read and the input value of $icode sizeof_data$$ is zero,
$icode sizeof_data$$ is set to the number of bytes in the data.
In this case, you should change $icode data$$ to point to the necessary space
and then call read again without changing $icode sizeof_data$$.

$head data$$
For the read operation, this argument has prototype
$codei%
	void* %data%
%$$
and is the data that is written or read from the table.
If this is a write (read) then $icode data$$ is (is not) $code const$$.

$children%example/devel/table/blob_table_xam.cpp
%$$
$head Example$$
The file $cref blob_table_xam.cpp$$ is an example use of
the write and read operations above.
$end
*/

# include <cstring>
# include <dismod_at/blob_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void write_blob_table(
	sqlite3*                db             ,
	const std::string&      table_name     ,
	const std::string&      col_name       ,
	const size_t&           sizeof_data    ,
	const void*             data           )
{	//
	std::string cmd;
	//
	// create the table
	cmd  = "create table " + table_name;
	cmd += " (" + table_name + "_id integer primary key, ";
	cmd +=  col_name + " blob);";
	dismod_at::exec_sql_cmd(db, cmd);
	//
	// prepare sqlite command
	cmd  = "insert into " + table_name + "(" + col_name + ") values(?)";
	sqlite3_stmt* p_stmt;
	int           n_byte = -1;
	const char**  pz_tail = nullptr;
	int rc = sqlite3_prepare_v2(db, cmd.c_str(), n_byte, &p_stmt, pz_tail);
	if( rc != SQLITE_OK )
	{	std::string message = "write_blob_table: following command failed:\n";
		message            += cmd;
		error_exit(message);
	}
	//
	// bind data to the statement
	int value_index = 1;
	n_byte          = int(sizeof_data);
	sqlite3_bind_blob(p_stmt, value_index, data, n_byte, SQLITE_STATIC);
	//
	// execute the statement
	rc = sqlite3_step(p_stmt);
	if( rc != SQLITE_DONE )
	{	std::string message = "write_blob_table: inserting blob in ";
		message            += table_name + " failed\n";
		error_exit(message);
	}
	//
	// delete the statement
	sqlite3_finalize(p_stmt);
}

void read_blob_table(
	sqlite3*                db             ,
	const std::string&      table_name     ,
	const std::string&      col_name       ,
	size_t&                 sizeof_data    ,
	void*                   data           )
{	//
	std::string cmd;
	//
	// prepare sqlite command
	cmd  = "select " + col_name + " from " + table_name;
	cmd += " where " + table_name + "_id = 1";
	sqlite3_stmt* p_stmt;
	int           n_byte = -1;
	const char**  pz_tail = nullptr;
	int rc = sqlite3_prepare_v2(db, cmd.c_str(), n_byte, &p_stmt, pz_tail);
	if( rc != SQLITE_OK )
	{	std::string message = "read_blob_table: following command failed:\n";
		message            += cmd;
		error_exit(message);
	}
	//
	// execute the statement
	rc = sqlite3_step(p_stmt);
	if( rc != SQLITE_ROW )
	{	std::string message = "read_blob_table: reading blob in ";
		message            += table_name + " failed\n";
		error_exit(message);
	}
	//
	// number of bytes in the blob
	int col_index = 0;
	n_byte = sqlite3_column_bytes(p_stmt, col_index);
	//
	if( sizeof_data == 0 )
	{	sizeof_data = n_byte;
		//
		// delete the statement
		sqlite3_finalize(p_stmt);
		//
		return;
	}
	if( size_t(n_byte) != sizeof_data )
	{	std::string message = "read_blob_table: unexpected size of blob in ";
		message            += table_name;
		error_exit(message);
	}
	//
	// copy to data
	void* destination  = data;
	const void* source = sqlite3_column_blob(p_stmt, col_index);
	std::memcpy(destination, source, sizeof_data),
	//
	// delete the statement
	sqlite3_finalize(p_stmt);
}


} // END_DISMOD_AT_NAMESPACE
