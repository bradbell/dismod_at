// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PREDICT_COMMAND_HPP
# define DISMOD_AT_PREDICT_COMMAND_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/avgint_subset.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void predict_command(
	const std::string&                                    source              ,
	sqlite3*                                              db                  ,
	const dismod_at::db_input_struct&                     db_input            ,
	size_t                                                n_var               ,
	dismod_at::data_model&                                avgint_object       ,
	const CppAD::vector<dismod_at::avgint_subset_struct>& avgint_subset_obj
);

} // END_DISMOD_AT_NAMESPACE

# endif
