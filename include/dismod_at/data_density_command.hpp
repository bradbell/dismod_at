// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
