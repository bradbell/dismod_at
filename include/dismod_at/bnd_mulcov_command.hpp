// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
   const std::string&                            max_abs_effect    ,
   const std::string&                            covariate_name    ,
   const CppAD::vector<covariate_struct>&        covariate_table   ,
   const CppAD::vector<mulcov_struct>&           mulcov_table
);

} // END_DISMOD_AT_NAMESPACE

# endif
