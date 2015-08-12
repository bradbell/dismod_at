// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_INFO_HPP
# define DISMOD_AT_PACK_INFO_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>
# include "get_node_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"
# include "child_info.hpp"
# include "get_integrand_table.hpp"
# include "get_covariate_table.hpp"

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

	// mulcov_meas_value infromation
	CppAD::vector< CppAD::vector<subvec_info> > mulcov_meas_value_info_;

	// mulcov_meas_std infromation
	CppAD::vector< CppAD::vector<subvec_info> > mulcov_meas_std_info_;

	// mulcov_rate_value infromation
	CppAD::vector< CppAD::vector<subvec_info> > mulcov_rate_value_info_;

	// total number of elements in the packed vector
	size_t size_;
public:
	pack_info(
		sqlite3*                            db             ,
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
	size_t mulstd_offset(size_t smooth_id) const; // 2DO: remove this version
	size_t mulstd_offset(size_t smooth_id, size_t k) const;

	// rate
	subvec_info rate_info(size_t rate_id, size_t j) const;

	// mulcov_meas_value_
	size_t      mulcov_meas_value_n_cov(size_t integrand_id) const;
	subvec_info mulcov_meas_value_info(size_t integrand_id, size_t j) const;

	// mulcov_meas_std_
	size_t      mulcov_meas_std_n_cov(size_t integrand_id) const;
	subvec_info mulcov_meas_std_info(size_t integrand_id, size_t j) const;

	// mulcov_rate_value_
	size_t      mulcov_rate_value_n_cov(size_t integrand_id) const;
	subvec_info mulcov_rate_value_info(size_t integrand_id, size_t j) const;

	// varialbe_name
	std::string variable_name(
		size_t                                 index           ,
		size_t                                 parent_node_id  ,
		const CppAD::vector<double>&           age_table       ,
		const CppAD::vector<covariate_struct>& covariate_table ,
		const CppAD::vector<integrand_struct>& integrand_table ,
		const CppAD::vector<mulcov_struct>&    mulcov_table    ,
		const CppAD::vector<node_struct>&      node_table      ,
		const CppAD::vector<smooth_struct>&    smooth_table    ,
		const CppAD::vector<double>&           time_table      ,
		const CppAD::vector<smooth_info>&      s_info_vec      ,
		const child_info&                      child_object
	) const;
};

} // END DISMOD_AT_NAMESPACE:
# endif
