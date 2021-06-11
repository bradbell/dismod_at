/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DEPEND_COMMAND_HPP
# define DISMOD_AT_DEPEND_COMMAND_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void depend_command(
	sqlite3*                                  db               ,
	const CppAD::vector<double>&              prior_mean       ,
	data_model&                               data_object      ,
	const CppAD::vector<subset_data_struct>&  subset_data_obj  ,
	const prior_model&                        prior_object
);

} // END_DISMOD_AT_NAMESPACE

# endif
