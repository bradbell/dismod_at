// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-22 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_HOLD_OUT_COMMAND_HPP
# define DISMOD_AT_HOLD_OUT_COMMAND_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/child_info.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void hold_out_command(
	sqlite3*                                      db                ,
	const std::string&                            integrand_name    ,
	const std::string&                            max_fit_str       ,
	const std::string&                            cov_name          ,
	const std::string&                            cov_value_1_str   ,
	const std::string&                            cov_value_2_str   ,
	const child_info&                             child_info4data   ,
	const CppAD::vector<integrand_struct>&        integrand_table   ,
	const CppAD::vector<covariate_struct>&        covariate_table   ,
	const CppAD::vector<data_struct>&             data_table        ,
	const CppAD::vector<double>&                  data_cov_value
);

} // END_DISMOD_AT_NAMESPACE

# endif
