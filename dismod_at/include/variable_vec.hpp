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
# include "get_mulcov_table.hpp"
# include "get_rate_table.hpp"
# include "smooth_info.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
class variable_vec {
	typedef struct { size_t covariate_id; size_t smooth_id; } mulcov_pair;
private:
	// The data table
	const CppAD::vector<data_struct>& data_table_; 

	// The smoothing table
	const CppAD::vector<smooth_struct>& smooth_table_; 

	// The parent node is node_id_[0]. The child nodes are node_id_[i] for
	// i = 1 , ... , node_id_.size() - 1  (set by constructor only)
	CppAD::vector<size_t> node_id_;

	// Mapping from the data_id to the node_id_ index, so we know which
	// rates to use for each data point. Note that node_id_[0] is the 
	// parent node and the other nodes are child nodes. The value
	// size_t(-1) is used for nodes that are not below the parent.  
	// (set by constructor only)
	CppAD::vector<size_t> data_id2node_index_;

	// The smoothing standard deviation multipliers come frist in vec_
	// and there are three per smoothing; i.e., mulstd_value, mulstd_dage,
	// and mulstd_dtime in smooth_table_.

	// The rate_mean covariate multipliers come next in vec_ and
	// each rate has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<mulcov_pair> > rate_mean_mulcov_;

	// The meas_mean covariate multipliers come next in vec_ and
	// each integrand has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<mulcov_pair> > meas_mean_mulcov_;

	// The meas_std covariate multipliers come next in vec_ and
	// each integrand has its own vector of these multipliers.  
	// (set by constructor only)
	CppAD::vector< CppAD::vector<mulcov_pair> > meas_std_mulcov_;

	// The rate values for the child nodes come next in vec_. Note that
	// each of the child nodes has the same smoothing. Also note
	// that the parent node value is the average of the child node values.
	// (set by constructor only)
	CppAD::vector<size_t> rate_smooth_id_;

	// value of all the variables
	CppAD::vector<Float> vec_;
public:
	variable_vec(
		const CppAD::vector<run_struct>&      run_table         ,
		const CppAD::vector<node_struct>&     node_table        ,
		const CppAD::vector<data_struct>&     data_table        ,
		const CppAD::vector<smooth_struct>&   smooth_table      ,
		const CppAD::vector<mulcov_struct>&   mulcov_table      ,
		const CppAD::vector<rate_enum>&       rate_table        ,
		const CppAD::vector<integrand_enum>&  integrand_table
	);
};

} // END DISMOD_AT_NAMESPACE:
# endif
