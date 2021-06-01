// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin blob_table_xam.cpp$$
$spell
	xam
$$

$section Writing and Reading Blobs: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cassert>
# include <dismod_at/blob_table.hpp>
# include <dismod_at/open_connection.hpp>

bool blob_table_xam(void)
{	bool   ok = true;
	//
	struct my_struct {int x; double y;};
	//
	// db
	std::string  file_name = "example.db";
	bool         new_file  = true;
	sqlite3*     db        = dismod_at::open_connection(file_name, new_file);
	//
	// one
	my_struct one;
	one.x = 1;
	one.y = 2.0;
	//
	// write from one -> blob
	std::string table_name  = "blob";
	std::string col_name    = "my_struct";
	size_t      sizeof_data = sizeof(one);
	void* data              = reinterpret_cast<void*>(&one);
	dismod_at::write_blob_table(db, table_name, col_name, sizeof_data, data);
	//
	// two
	my_struct two;
	assert( sizeof(two) == sizeof_data);
	//
	// read blob -> two
	data = reinterpret_cast<void*>(&two);
	dismod_at::read_blob_table(db, table_name, col_name, sizeof_data, data);
	//
	// check result
	ok &= one.x == two.x;
	ok &= one.y == two.y;
	//
	return ok;
}
// END C++
