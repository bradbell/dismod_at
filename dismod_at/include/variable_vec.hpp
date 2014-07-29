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
# include "get_run_table.hpp"
# include "get_node_table.hpp"
# include "get_data_table.hpp"
# include "get_smooth_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
class variable_vec {
private:
	// The parent and child nodes. If there is less than two children for this
	// parent, child_node_id_ will have size one and its element will be
	// set to the parent node id (set by constructor only)
	size_t parent_node_id_;
	CppAD::vector<size_t> child_node_id_;

	// Mapping from the data_id to the child node index, so we know which
	// rates to use for each data point. Note that child_node_id.size()
	// is used for parent node data and child_node_id.size() + 1 is used
	// for nodes that are not below the parent.  (set by constructor only)
	CppAD::vector<size_t> data_id2child_index_;

	// The smoothing standard deviation multipliers come frist in vec_
	// and there are three per smoothing.  (set by constructor only)
	CppAD::vector<size_t> mulstd_like_id_; 

	// The rate_mean covariate multipliers come next in vec_ and
	// each rate has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<size_t> > rate_mean_mulcov_covariate_id_;
	CppAD::vector< CppAD::vector<size_t> > rate_mean_mulcov_smooth_id_;

	// The meas_mean covariate multipliers come next in vec_ and
	// each integrand has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<size_t> > meas_mean_mulcov_covariate_id_;
	CppAD::vector< CppAD::vector<size_t> > meas_mean_mulcov_smooth_id_;

	// The meas_std covariate multipliers come next in vec_ and
	// each integrand has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<size_t> > meas_std_mulcov_covariate_id_;
	CppAD::vector< CppAD::vector<size_t> > meas_std_mulcov_smooth_id_;

	// The rate values for the child nodes come next in vec_. Note that
	// each of the child nodes has the same smoothing. Also note
	// that the parent node value is the average of the child node values.
	// (set by constructor only)
	CppAD::vector<size_t> rate_smooth_id_;

	// value of all the variables
	CppAD::vector<Float> vec_;
public:
	variable_vec(
		const CppAD::vector<run_struct>&     run_table   ,
		const CppAD::vector<node_struct>&    node_table  ,
		const CppAD::vector<data_struct>&    data_table
	);
};

} // END DISMOD_AT_NAMESPACE:
# endif
