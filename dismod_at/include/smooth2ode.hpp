// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_SMOOTH2ODE_HPP
# define DISMOD_AT_SMOOTH2ODE_HPP

# include <cppad/cppad.hpp>
# include "smooth_grid.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
class smooth2ode {
	typedef struct {
		size_t i_sg;
		size_t j_sg;
		double c_00;
		double c_10;
		double c_01;
		double c_11;
	} ode_point;
private:
	const size_t              n_age_sg_;
	const size_t              n_time_sg_;
	const size_t              n_age_ode_;
	const size_t              n_time_ode_;
	const double              ode_step_size_;
	// effectively const (computed by constructor)
	CppAD::vector<ode_point> coefficient_;
public:
	smooth2ode(
		const smooth_grid&                          sg            ,
		const CppAD::vector<double>&                age_table     ,
		const CppAD::vector<double>&                time_table    ,
		size_t                                      n_age_ode     ,
		size_t                                      n_time_ode    ,
		double                                      ode_step_size
	);
	CppAD::vector<Float> interpolate(
		const CppAD::vector<Float>&   var_sg
	) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
