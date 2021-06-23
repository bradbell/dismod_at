// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DATA_DENSITY_COMMAND_HPP
# define DISMOD_AT_DATA_DENSITY_COMMAND_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void data_density_command(
	sqlite3*                                      db                ,
	std::string&                                  integrand_name    ,
	std::string&                                  density_name      ,
	std::string&                                  eta_factor_str    ,
	std::string&                                  nu_str            ,
	const CppAD::vector<integrand_struct>&        integrand_table   ,
	const CppAD::vector<density_enum>&            density_table     ,
	const CppAD::vector<data_struct>&             data_table
);

} // END_DISMOD_AT_NAMESPACE

# endif
