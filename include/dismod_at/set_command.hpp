// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_SET_COMMAND_HPP
# define DISMOD_AT_SET_COMMAND_HPP

# include <sqlite3.h>
# include <string>
# include <dismod_at/get_option_table.hpp>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	void set_option_command(
		sqlite3*                                        db           ,
		const CppAD::vector<option_struct>&  option_table ,
		const std::string&                              name         ,
		const std::string&                              value
	);
	void set_command(
		const std::string&                     table_out    ,
		const std::string&                     source       ,
		const std::string&                     sample_index ,
		sqlite3*                               db           ,
		const CppAD::vector<double>&           prior_mean
	);
}

# endif
