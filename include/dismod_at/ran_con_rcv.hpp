// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_RAN_CON_RCV_HPP
# define DISMOD_AT_RAN_CON_RCV_HPP

# include <cppad/utility/vector.hpp>
# include <cppad/mixed/typedef.hpp>
# include "pack_info.hpp"
# include "random_effect.hpp"

namespace dismod_at {
	CppAD::mixed::d_sparse_rcv ran_con_rcv(
		double                        bound_random    ,
		const CppAD::vector<bool>&    zero_sum_random ,
		const dismod_at::pack_info&   pack_object
	);
}

# endif
