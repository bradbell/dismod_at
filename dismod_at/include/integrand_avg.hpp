// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_INTEGRAND_AVG_HPP
# define DISMOD_AT_INTEGRAND_AVG_HPP

# include <cppad/vector.hpp>
# include "get_data_table.hpp"
# include "get_integrand_table.hpp"
# include "weight_grid.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class integrand_avg {
	typedef struct {
		integrand_enum        integrand;
		size_t                i_min;
		size_t                j_min;
		double                n_age;
		double                n_time;
		CppAD::vector<double> c;
	} ode_point;
private:
	const size_t              n_age_ode_;
	const size_t              n_time_ode_;
	const double              ode_step_size_;
	// effectively const (computed by constructor)
	CppAD::vector<ode_point> data_info_;
public:
	integrand_avg(
		const CppAD::vector<weight_grid>&    wg_vec          ,                 
		const CppAD::vector<data_struct>&    data_table      ,
		const CppAD::vector<integrand_enum>& integrand_table ,
		const CppAD::vector<double>&         age_table       ,
		const CppAD::vector<double>&         time_table      ,
		size_t                               n_age_ode       ,
		size_t                               n_time_ode      ,
		double                               ode_step_size
	);
	template <class Float>
	CppAD::vector<Float> compute(
		const CppAD::vector<size_t>&  data_id  ,
		const CppAD::vector<Float>&   iota     ,
		const CppAD::vector<Float>&   rho      ,
		const CppAD::vector<Float>&   chi      ,
		const CppAD::vector<Float>&   omega    ,
		const CppAD::vector<Float>&   S        ,
		const CppAD::vector<Float>&   C
	) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
