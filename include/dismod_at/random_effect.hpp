// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_RANDOM_EFFECT_HPP
# define DISMOD_AT_RANDOM_EFFECT_HPP

# include <cppad/cppad.hpp>
# include "pack_info.hpp"

namespace dismod_at {
	extern CppAD::vector<size_t> random2var_id(
		const pack_info&             pack_object
	);
	//
	template <class Float>
	extern void unpack_random(
		const pack_info&            pack_object  ,
		const CppAD::vector<Float>& pack_vec     ,
		CppAD::vector<Float>&       random_vec
	);
	//
	template <class Float>
	extern void pack_random(
		const pack_info&            pack_object  ,
		CppAD::vector<Float>&       pack_vec     ,
		const CppAD::vector<Float>& random_vec
	);
}

# endif
