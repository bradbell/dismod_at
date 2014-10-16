// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
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
	Float prior_density(
		const CppAD::vector<Float>&            var_vec         ,
		const pack_var&                        var_info        ,
		const CppAD::vector<double>&           age_table       ,
		const CppAD::vector<double>&           time_table      ,
		const CppAD::vector<prior_struct>&     prior_table     ,
		const CppAD::vector<smooth_struct>&    smooth_table    ,
		const CppAD::vector<smooth_info>&      s_info_vec
	);

}

# endif
