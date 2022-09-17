// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
	const pack_info&                                 pack_object         ,
	const db_input_struct&                           db_input            ,
	const size_t&                                    parent_node_id      ,
	const child_info&                                child_info4data     ,
	const CppAD::vector<smooth_info>&                s_info_vec
);

} // END_DISMOD_AT_NAMESPACE

# endif
