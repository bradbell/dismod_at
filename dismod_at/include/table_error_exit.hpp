// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_TABLE_ERROR_EXIT_HPP
# define DISMOD_AT_TABLE_ERROR_EXIT_HPP

# include <string>
namespace dismod_at {
	extern void table_error_exit(
		const std::string& table_name ,
		size_t             row_id     ,
		const std::string& message
	);
}

# endif
