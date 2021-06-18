// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_BND_MULCOV_COMMAND_HPP
# define DISMOD_AT_BND_MULCOV_COMMAND_HPP

# include <sqlite3.h>
# include <string>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_mulcov_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void bnd_mulcov_command(
	sqlite3*                                      db                ,
	std::string&                                  max_abs_effect    ,
	const CppAD::vector<double>&                  data_cov_value    ,
	const CppAD::vector<data_subset_struct>&      data_subset_table ,
	const CppAD::vector<covariate_struct>&        covariate_table   ,
	const CppAD::vector<mulcov_struct>&           mulcov_table
);

} // END_DISMOD_AT_NAMESPACE

# endif
