// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PRIOR_DENSITY_HPP
# define DISMOD_AT_PRIOR_DENSITY_HPP

# include <cppad/vector.hpp>
# include "pack_var.hpp"
# include "get_prior_table.hpp"
# include "get_smooth_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at {

	template <class Float>
	struct prior_density_struct {
		Float                smooth;
		CppAD::vector<Float> sub_abs;
	};

	template <class Float>
	prior_density_struct<Float> prior_density(
		const pack_var&                        pack_info        ,
		const CppAD::vector<Float>&            pack_vec         ,
		const CppAD::vector<double>&           age_table       ,
		const CppAD::vector<double>&           time_table      ,
		const CppAD::vector<prior_struct>&     prior_table     ,
		const CppAD::vector<smooth_info>&      s_info_vec
	);

}

# endif
