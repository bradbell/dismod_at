// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_INFO_HPP
# define DISMOD_AT_PACK_INFO_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include "get_node_table.hpp"
# include "get_subgroup_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"
# include "child_info.hpp"
# include "get_integrand_table.hpp"
# include "get_covariate_table.hpp"
# include "get_nslist_pair.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class pack_info {
public:
	// BEGIN SUBVEC_INFO
	typedef struct {
		size_t covariate_id;
		size_t group_id;
		size_t smooth_id;
		size_t n_var;
		size_t offset;
	} subvec_info;
	// END SUBVEC_INFO
private:
	// total number of elements in the packed vector
	size_t size_;

	// number of smoothings
	const size_t n_smooth_;

	// number of integrands
	const size_t n_integrand_;

	// number of children
	const size_t n_child_;

	// number of random effects
	size_t n_random_;

	// number of subgroups in each group and
	// subgroup_id corresponding to first member of each group
	CppAD::vector<size_t>  subgroup_size_;
	CppAD::vector<size_t>  first_subgroup_id_;

	// mulstd variable offsets
	CppAD::vector<size_t>  mulstd_offset_;

	// node_rate_value_info_ [n_rate][n_child_ + 1]
	CppAD::vector< CppAD::vector<subvec_info> > node_rate_value_info_;

	// group_rate_value_info_ [n_rate][ncov(rate)]
	CppAD::vector< CppAD::vector<subvec_info> > group_rate_value_info_;

	// group_meas_value_info_ [n_integrand_][ncov(integrand)]
	CppAD::vector< CppAD::vector<subvec_info> > group_meas_value_info_;

	// group_meas_noise_info_ [n_integrand_][ncov(integrand)]
	CppAD::vector< CppAD::vector<subvec_info> > group_meas_noise_info_;

	// subgroup_rate_value_info_ [n_rate][ncov(rate)][n_sub(rate,j)]
	CppAD::vector< CppAD::vector< CppAD::vector<subvec_info> > >
	subgroup_rate_value_info_;

public:
	pack_info(
	size_t                                    n_integrand      ,
	const CppAD::vector<size_t>               child_id2node_id ,
	const CppAD::vector<subgroup_struct>&     subgroup_table   ,
	const CppAD::vector<smooth_struct>&       smooth_table     ,
	const CppAD::vector<mulcov_struct>&       mulcov_table     ,
	const CppAD::vector<rate_struct>&         rate_table       ,
	const CppAD::vector<nslist_pair_struct>&  nslist_pair
	);

	// size of entire packed vector
	size_t size(void) const;

	// number of integrands
	size_t integrand_size(void) const;

	// number of children
	size_t child_size(void) const;

	// number of smoothings
	size_t smooth_size(void) const;

	// number of random_effects
	size_t random_size(void) const;

	// number of groups
	size_t group_size(void) const;

	// number of subgroups in each group
	size_t subgroup_size(size_t group) const;

	// first subgroup in each group
	size_t first_subgroup_id(size_t group) const;

	// mulstd
	size_t mulstd_offset(size_t smooth_id, size_t k) const;

	// node_rate_value_info
	subvec_info node_rate_value_info(size_t rate_id, size_t j) const;

	// group_rate_value_info
	size_t      group_rate_value_n_cov(size_t rate_id) const;
	subvec_info group_rate_value_info(size_t rate_id, size_t j) const;

	// group_meas_value_info
	size_t      group_meas_value_n_cov(size_t integrand_id) const;
	subvec_info group_meas_value_info(size_t integrand_id, size_t j) const;

	// group_meas_noise_info
	size_t      group_meas_noise_n_cov(size_t integrand_id) const;
	subvec_info group_meas_noise_info(size_t integrand_id, size_t j) const;

	// subgroup_rate_value_info
	size_t      subgroup_rate_value_n_cov(size_t integrand_id) const;
	size_t      subgroup_rate_value_n_sub(size_t integrand_id, size_t j) const;
	subvec_info
	subgroup_rate_value_info(size_t integrand_id, size_t j, size_t k) const;

};

} // END DISMOD_AT_NAMESPACE:
# endif
