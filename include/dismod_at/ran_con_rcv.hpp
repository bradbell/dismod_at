// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_RAN_CON_RCV_HPP
# define DISMOD_AT_RAN_CON_RCV_HPP

# include <cppad/utility/vector.hpp>
# include <cppad/mixed/typedef.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/remove_const.hpp>

namespace dismod_at {
	CppAD::mixed::d_sparse_rcv ran_con_rcv(
		double                        bound_random           ,
		const CppAD::vector<bool>&    zero_sum_child_rate    ,
		const CppAD::vector<bool>&    zero_sum_mulcov_group  ,
		const pack_info&              pack_object            ,
		const remove_const&           random_const
	);
}

# endif
