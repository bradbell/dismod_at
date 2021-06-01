// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_BLOB_TABLE_HPP
# define DISMOD_AT_BLOB_TABLE_HPP

# include <sqlite3.h>
# include <string>

namespace dismod_at {
	void write_blob_table(
		sqlite3*                            db             ,
		const std::string&                  table_name     ,
		const std::string&                  col_name       ,
		size_t                              sizeof_data    ,
		const void*                         data
	);
	void read_blob_table(
		sqlite3*                            db             ,
		const std::string&                  table_name     ,
		const std::string&                  col_name       ,
		size_t                              sizeof_data    ,
		void*                               data
	);
}

# endif
