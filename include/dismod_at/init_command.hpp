// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_INIT_COMMAND_HPP
# define DISMOD_AT_INIT_COMMAND_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/subset_data.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void init_command(
	sqlite3*                                         db                  ,
	const CppAD::vector<double>&                     prior_mean          ,
	const CppAD::vector<subset_data_struct>&         subset_data_obj     ,
	const pack_info&                                 pack_object         ,
	const db_input_struct&                           db_input            ,
	const size_t&                                    parent_node_id      ,
	const child_info&                                child_object        ,
	const CppAD::vector<smooth_info>&                s_info_vec
);

} // END_DISMOD_AT_NAMESPACE

# endif
