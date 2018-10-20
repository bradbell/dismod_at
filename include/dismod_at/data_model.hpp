// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DATA_MODEL_HPP
# define DISMOD_AT_DATA_MODEL_HPP

# include <limits>
# include <cppad/utility/vector.hpp>
# include "data_subset.hpp"
# include "get_integrand_table.hpp"
# include "get_density_table.hpp"
# include "get_prior_table.hpp"
# include "weight_info.hpp"
# include "smooth2ode.hpp"
# include "pack_info.hpp"
# include "residual_density.hpp"
# include "get_data_sim_table.hpp"
# include "avg_integrand.hpp"

// declare test functions that are friends
extern bool sci_ode_xam(void);

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
		CppAD::vector<double> c_ode;
		bool                  bound_ran_neq;
	} data_ode_info;
private:
	// constant values
	const size_t                 n_covariate_;
	const size_t                 n_age_ode_;
	const size_t                 n_time_ode_;
	const double                 ode_step_size_;
	const size_t                 n_child_;
	const CppAD::vector<double>& subset_cov_value_;
	const pack_info&             pack_object_;
	//
	// set by constructor and not changed
	CppAD::vector<smooth2ode*>   si2ode_vec_;
	CppAD::vector<bool>          child_ran_zero_;
	CppAD::vector<data_ode_info> data_info_;
	CppAD::vector<double>        minimum_meas_cv_;
	size_t                       eigen_ode2_case_number_;
	//
	// Has replace_like been called.
	// Set false by constructor and true by replace_like.
	bool                         replace_like_called_;

	// set by consructor, except that following fields set by replace_like
	// data_subset_obj_[subset_id].density_id
	// data_subset_obj_[subset_id].hold_out
	// data_subset_obj_[subset_id].meas_value
	// data_subset_obj_[subset_id].meas_std
	CppAD::vector<data_subset_struct>         data_subset_obj_;

	// Used to compute average average integrands
	avg_integrand                avg_object_;

public:
	template <class SubsetStruct>
	data_model(
		const std::string&                       rate_case       ,
		double                                   bound_random    ,
		size_t                                   n_covariate     ,
		size_t                                   n_age_ode       ,
		size_t                                   n_time_ode      ,
		double                                   ode_step_size   ,
		const CppAD::vector<double>&             ode_age_grid    ,
		const CppAD::vector<double>&             age_table       ,
		const CppAD::vector<double>&             time_table      ,
		const CppAD::vector<integrand_struct>&   integrand_table ,
		const CppAD::vector<prior_struct>&       prior_table     ,
		const CppAD::vector<SubsetStruct>&       subset_object   ,
		const CppAD::vector<double>&             subset_cov_value,
		const CppAD::vector<weight_info>&        w_info_vec      ,
		const CppAD::vector<smooth_info>&        s_info_vec      ,
		const pack_info&                         pack_object     ,
		const child_info&                        child_object
	);
	// destructor must delete the smooth2ode objects pointed to by si2ode_vec_
	~data_model(void);
	//
	void replace_like(
		const CppAD::vector<data_subset_struct>& data_subset_obj
	);
	//
	// to compute S, C, corresponding to reference value for covariates
	template <class Float>
	CppAD::vector<Float> reference_ode(
		const  CppAD::vector<Float>&  pack_vec     ,
		bool                          parent_only
	) const;
	//
	// use ode to compute S, C, and integrand on a sub-grid
	template <class Float>
	CppAD::vector<Float> sci_ode(
		integrand_enum                integrand    ,
		size_t                        i_min        ,
		size_t                        j_min        ,
		size_t                        n_age_sub    ,
		size_t                        n_time_sub   ,
		size_t                        child        ,
		const  CppAD::vector<double>& x            ,
		const  CppAD::vector<Float>&  pack_vec     ,
		const  CppAD::vector<Float>&  reference_sc
	) const;
	friend bool ::sci_ode_xam(void);
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
		size_t                        data_id           ,
		const  CppAD::vector<Float>&  pack_vec          ,
		const  CppAD::vector<Float>&  reference_sc
	) const;
	// compute weighted residual and log-likelihood for one data points
	template <class Float>
	residual_struct<Float> like_one(
		size_t                        data_id  ,
		const  CppAD::vector<Float>&  pack_vec ,
		const  Float&                 avg      ,
		Float&                        delta
	) const;
	// compute weighted residual and log-likelihood for all data points
	template <class Float>
	CppAD::vector< residual_struct<Float> > like_all(
		bool                          hold_out ,
		bool                          parent   ,
		const  CppAD::vector<Float>&  pack_vec
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
