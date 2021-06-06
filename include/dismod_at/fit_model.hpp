// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_FIT_MODEL_HPP
# define DISMOD_AT_FIT_MODEL_HPP

# include <map>
# include <sqlite3.h>
# include <dismod_at/a1_double.hpp>
# include <cppad/mixed/cppad_mixed.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/remove_const.hpp>

namespace dismod_at {

	// declare fit_model class
	class fit_model : public cppad_mixed {
	public:
		// type declarations
		typedef CppAD::vector<a1_double>   a1_vector;
		//
	// =======================================================================
	private:
		typedef CppAD::mixed::warm_start_struct warm_start_struct;
		typedef CppAD::mixed::trace_struct      trace_struct;
		// ---------------------------------------------------------------
		// private member data
		// ---------------------------------------------------------------
		//
		// const member variables
		sqlite3*                           db_;
		const int                          simulate_index_;
		const bool                         warn_on_stderr_;
		const bool                         no_scaling_;
		const size_t                       n_fixed_;
		const size_t                       n_random_;
		const pack_info&                   pack_object_;
		const pack_prior&                  var2prior_;
		const CppAD::vector<double>&       start_var_;
		const CppAD::vector<double>&       scale_var_;
		const CppAD::vector<prior_struct>& prior_table_;
		const prior_model&                 prior_object_;
		const remove_const&                random_const_;
		//
		// effectively const
		data_model&                        data_object_;
		// -------------------------------------------------------------------
		// set during constructor and otherwise const
		//
		// Prior for variable differences. Only difference for which
		// the upper limit is less than + infinity or the lower limit
		// is greater than minus infinity are included. Note that all the
		// corresponding variables should be fixed effects.
		CppAD::vector<diff_prior_struct>  diff_prior_;
		//
		// Converts a packed variable id to index in fixed vector.
		// Uses the invalid value n_fixed_ if not a fixed variable.
		CppAD::vector<size_t>             var_id2fixed_;
		//
		// Lower and upper limits for random effects and
		// number of components that have eqaul lower and upper limits.
		CppAD::vector<double>             random_lower_;
		CppAD::vector<double>             random_upper_;
		size_t                            n_random_equal_;
		//
		// Offset in log scaling for each component of the fixed vector;
		// If fixed_is_scaled_[j], fixed_vec[j] in dismod_at has value
		// log( fixed_vec[j] + fixed_scale_eta_[j] ) during optimization.
		CppAD::vector<bool>   fixed_is_scaled_;
		CppAD::vector<double> fixed_scale_eta_;
		// -------------------------------------------------------------------
		// solution found by run_fit and in pack_info order
		struct {
			CppAD::vector<double>           fit_var_value;
			CppAD::vector<double>           lagrange_value;
			CppAD::vector<double>           lagrange_dage;
			CppAD::vector<double>           lagrange_dtime;
			CppAD::vector<trace_struct>     trace_vec;
			warm_start_struct               warm_start;
		} solution_;
		// ---------------------------------------------------------------
		// private member functions
		// ---------------------------------------------------------------
		// scaling
		template <class Float>
		void scale_fixed_effect(
			const CppAD::vector<Float>& fixed_before  ,
			CppAD::vector<Float>&       fixed_after
		) const;
		//
		// unscaling
		template <class Float>
		void unscale_fixed_effect(
			const CppAD::vector<Float>& fixed_after  ,
			CppAD::vector<Float>&       fixed_before
		) const;
		// -------------------------------------------------------------------
		// virtual functions used by cppad_mixed base class
		virtual a1_vector ran_likelihood(
			const a1_vector& fixed_vec   ,
			const a1_vector& random_vec
		);
		// pass fixed negative log-likelihood to base class
		virtual a1_vector fix_likelihood(
			const a1_vector& fixed_vec
		);
		// pass constraint functions to base class
		virtual a1_vector fix_constraint(
			const a1_vector& fixed_vec_scaled
		);
		// display error message and terminate program
		virtual void fatal_error(const std::string& error_message);
		// display warning message and continue
		virtual void warning(const std::string& warning_message);
	// =======================================================================
	public:
		// constructor
		fit_model(
			sqlite3*                             db                    ,
			int                                  simulate_index        ,
			bool                                 warn_on_stderr        ,
			double                               bound_random          ,
			bool                                 no_scaling            ,
			const pack_info&                     pack_object           ,
			const pack_prior&                    var2prior             ,
			const CppAD::vector<double>&         start_var             ,
			const CppAD::vector<double>&         scale_var             ,
			const CppAD::vector<prior_struct>&   prior_table           ,
			const prior_model&                   prior_object          ,
			const remove_const&                  random_const          ,
			bool                                 quasi_fixed           ,
			const CppAD::vector<bool>&           zero_sum_child_rate   ,
			const CppAD::vector<bool>&           zero_sum_mulcov_group ,
			data_model&                          data_object
		);
		//
		// run fit
		void run_fit(
			bool                                  random_only ,
			// effectively const
			std::map<std::string, std::string>&   option_map  ,
			const warm_start_struct&              warm_start
		);
		// get_solution
		void get_solution(
			CppAD::vector<double>&           fit_var_value   ,
			CppAD::vector<double>&           lagrange_value  ,
			CppAD::vector<double>&           lagrange_dage   ,
			CppAD::vector<double>&           lagrange_dtime  ,
			warm_start_struct&               warm_start
		);
		// sample from asympotitic approximation for posterior distribution
		void sample_posterior(
			CppAD::mixed::d_sparse_rcv&         hes_fixed_obj_out  ,
			CppAD::mixed::d_sparse_rcv&         hes_random_obj_out ,
			size_t                              n_sample           ,
			CppAD::vector<double>&              sample_out         ,
			const CppAD::vector<double>&        fit_var_value      ,
			// effectively const
			std::map<std::string, std::string>& option_map
		);
	};
}
// implementation of template functions
# include <dismod_at/fit_model_scale.hpp>

# endif
