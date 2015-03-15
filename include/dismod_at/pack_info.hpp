// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_INFO_HPP
# define DISMOD_AT_PACK_INFO_HPP

# include <cppad/vector.hpp>
# include "get_node_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class pack_info {
public:
	// BEGIN SUBVEC_INFO
	typedef struct {
		size_t covariate_id;
		size_t smooth_id;
		size_t n_var;
		size_t offset;
	} subvec_info;
	// END SUBVEC_INFO
private:
	// number of smoothings
	const size_t n_smooth_;

	// number of integrands
	const size_t n_integrand_;

	// number of children
	const size_t n_child_;

	// mulstd variable offsets
	size_t mulstd_offset_;

	// rate variable information
	CppAD::vector< CppAD::vector<subvec_info> > rate_info_;

	// meas_mean_mulcov infromation
	CppAD::vector< CppAD::vector<subvec_info> > meas_mean_mulcov_info_;

	// meas_std_mulcov infromation
	CppAD::vector< CppAD::vector<subvec_info> > meas_std_mulcov_info_;

	// rate_mean_mulcov infromation
	CppAD::vector< CppAD::vector<subvec_info> > rate_mean_mulcov_info_;

	// total number of elements in the packed vector
	size_t size_;
public:
	pack_info(
		size_t                              n_integrand    ,
		size_t                              n_child        ,
		const CppAD::vector<smooth_struct>& smooth_table   ,
		const CppAD::vector<mulcov_struct>& mulcov_table   ,
		const CppAD::vector<rate_struct>&   rate_table
	);

	// size of entire packed vector
	size_t size(void) const;

	// number of integrands
	size_t integrand_size(void) const;

	// number of children
	size_t child_size(void) const;

	// number of smoothings
	size_t smooth_size(void) const;

	// mulstd
	size_t mulstd_offset(size_t smooth_id) const;

	// rate
	subvec_info rate_info(size_t rate_id, size_t j) const;

	// meas_mean_mulcov_
	size_t      meas_mean_mulcov_n_cov(size_t integrand_id) const;
	subvec_info meas_mean_mulcov_info(size_t integrand_id, size_t j) const;

	// meas_std_mulcov_
	size_t      meas_std_mulcov_n_cov(size_t integrand_id) const;
	subvec_info meas_std_mulcov_info(size_t integrand_id, size_t j) const;

	// rate_mean_mulcov_
	size_t      rate_mean_mulcov_n_cov(size_t integrand_id) const;
	subvec_info rate_mean_mulcov_info(size_t integrand_id, size_t j) const;
};

} // END DISMOD_AT_NAMESPACE:
# endif
