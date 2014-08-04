// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_VARIABLE_VEC_HPP
# define DISMOD_AT_VARIABLE_VEC_HPP

# include <cppad/vector.hpp>
# include "get_node_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class pack_var {
public:
	// BEGIN MULCOV_INFO
	typedef struct { 
		size_t covariate_id;
		size_t smooth_id; 
		size_t n_var; 
		size_t offset; 
	} mulcov_info;
	// END MULCOV_INFO
private:
	// number of smoothings
	const size_t n_smooth_;

	// number of integrands
	const size_t n_integrand_;

	// number of children
	const size_t n_child_;

	// offset for mulstd variables
	size_t mulstd_offset_;

	// offset for rate variables
	CppAD::vector<size_t> rate_offset_;
	CppAD::vector<size_t> rate_n_var_;

	// meas_mean_mulcov infromation
	CppAD::vector< CppAD::vector<mulcov_info> > meas_mean_mulcov_info_;

	// meas_std_mulcov infromation
	CppAD::vector< CppAD::vector<mulcov_info> > meas_std_mulcov_info_;

	// rate_mean_mulcov infromation
	CppAD::vector< CppAD::vector<mulcov_info> > rate_mean_mulcov_info_;

	// total number of elements in the packed vector
	size_t size_;
public:
	pack_var(
		size_t                              n_integrand  ,
		size_t                              n_child      ,
		const CppAD::vector<smooth_struct>& smooth_table ,
		const CppAD::vector<mulcov_struct>& mulcov_table ,
		const CppAD::vector<rate_struct>&   rate_table 
	);

	// size of entire packed vector
	size_t size(void) const;

	// mulstd
	size_t mulstd(size_t smooth_id) const;

	// rate
	size_t rate_n_var(size_t rate_id) const;
	size_t rate_offset(size_t rate_id, size_t j) const;

	// meas_mean_mulcov_
	size_t      meas_mean_mulcov_n_cov(size_t integrand_id) const;
	mulcov_info meas_mean_mulcov_info(size_t integrand_id, size_t j) const;

	// meas_std_mulcov_
	size_t      meas_std_mulcov_n_cov(size_t integrand_id) const;
	mulcov_info meas_std_mulcov_info(size_t integrand_id, size_t j) const;

	// rate_mean_mulcov_
	size_t      rate_mean_mulcov_n_cov(size_t integrand_id) const;
	mulcov_info rate_mean_mulcov_info(size_t integrand_id, size_t j) const;
};

} // END DISMOD_AT_NAMESPACE:
# endif
