// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SET_COMMAND_HPP
# define DISMOD_AT_SET_COMMAND_HPP

# include <sqlite3.h>
# include <string>
# include <dismod_at/get_option_table.hpp>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	void set_avgint_command(sqlite3* db);
	//
	void set_option_command(
		sqlite3*                                        db           ,
		const CppAD::vector<option_struct>&             option_table ,
		const std::string&                              name         ,
		const std::string&                              value
	);
	//
	void set_command(
		const std::string&                     table_out    ,
		const std::string&                     source       ,
		const std::string&                     sample_index ,
		sqlite3*                               db           ,
		const CppAD::vector<double>&           prior_mean
	);
}

# endif
