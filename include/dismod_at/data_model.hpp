// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DATA_MODEL_HPP
# define DISMOD_AT_DATA_MODEL_HPP

# include <limits>
# include <cppad/vector.hpp>
# include "data_subset.hpp"
# include "get_integrand_table.hpp"
# include "get_density_table.hpp"
# include "get_node_table.hpp"
# include "weight_info.hpp"
# include "smooth2ode.hpp"
# include "pack_info.hpp"
# include "residual_density.hpp"
# include "get_simulate_table.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class data_model {
	// infromation for each data point
	typedef struct {
		integrand_enum        integrand;
		density_enum          density;
		size_t                child;
		size_t                i_min;
		size_t                j_min;
		size_t                n_age;
		size_t                n_time;
		double                eta;
		CppAD::vector<double> c_ode;
	} data_ode_info;
private:
	const size_t                              n_age_ode_;
	const size_t                              n_time_ode_;
	const double                              ode_step_size_;
	const pack_info                           pack_object_;

	// set by constructor and not changed
	size_t                       n_child_;
	CppAD::vector<smooth2ode*>   si2ode_vec_;
	CppAD::vector<data_ode_info> data_info_;

	// set to a default value by consturctor
	// modified by set_eigen_ode2_case_number
	size_t                       eigen_ode2_case_number_;

	// set by consructor as reference to object in call
	// data_subset_obj_[subset_id].meas_value is modified by change_meas_value.
	CppAD::vector<data_subset_struct>         data_subset_obj_;
public:
	data_model(
		size_t                                   parent_node_id  ,
		size_t                                   n_age_ode       ,
		size_t                                   n_time_ode      ,
		double                                   ode_step_size   ,
		const CppAD::vector<double>&             age_table       ,
		const CppAD::vector<double>&             time_table      ,
		const CppAD::vector<integrand_struct>&   integrand_table ,
		const CppAD::vector<node_struct>&        node_table      ,
		const CppAD::vector<data_subset_struct>& data_subset_obj ,
		const CppAD::vector<weight_info>&        w_info_vec      ,
		const CppAD::vector<smooth_info>&        s_info_vec      ,
		const pack_info&                         pack_object     ,
		const child_info&                        child_object
	);
	// destructor must delete the smooth2ode objects pointed to by si2ode_vec_
	~data_model(void);
	//
	void set_eigen_ode2_case_number(const std::string& rate_info_arg);
	//
	void change_meas_value(
		const size_t&                         sample_index,
		const CppAD::vector<simulate_struct>& sample_table
	);
	//
	// compute average for integrands that do not require S or C
	template <class Float>
	Float avg_no_ode(
		size_t                        data_id  ,
		const  CppAD::vector<Float>&  pack_vec
	) const;
	// compute average for integrands that require S or C
	template <class Float>
	Float avg_yes_ode(
		size_t                        data_id  ,
		const  CppAD::vector<Float>&  pack_vec
	) const;
	// compute weighted residual and log-likelihood for one data points
	template <class Float>
	residual_struct<Float> like_one(
		size_t                        data_id  ,
		const  CppAD::vector<Float>&  pack_vec  ,
		const  Float&                 avg
	) const;
	// compute weighted residual and log-likelihood for all data points
	template <class Float>
	CppAD::vector< residual_struct<Float> > like_all(
		const  CppAD::vector<Float>&  pack_vec
	) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
