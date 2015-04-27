// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_BILINEAR_INTERP_HPP
# define DISMOD_AT_BILINEAR_INTERP_HPP

# include <cppad/vector.hpp>

namespace dismod_at {
		double bilinear_interp(
			double                         x          ,
			double                         y          ,
			const CppAD::vector<double>&   x_grid     ,
			const CppAD::vector<double>&   y_grid     ,
			const CppAD::vector<double>&   z_grid     ,
			size_t&                        i          ,
			size_t&                        j
		);
}

# endif
