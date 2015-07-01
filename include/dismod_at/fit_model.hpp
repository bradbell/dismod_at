// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_FIT_MODEL_HPP
# define DISMOD_AT_FIT_MODEL_HPP

# include <dismod_at/a5_double.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>

namespace dismod_at {

	// declare fit_model class
	class fit_model : public approx_mixed {
	public:
		// type declarations
		typedef CppAD::AD<double>          a1_double;
		typedef CppAD::vector<a1_double>   a1d_vector;
		typedef CppAD::vector<a5_double>   a5d_vector;
		//
	// =======================================================================
	private:
		// const member variables
		const size_t                       n_fixed_;
		const size_t                       n_random_;
		const pack_info&                   pack_object_;
		const CppAD::vector<prior_struct>& prior_table_;
		const CppAD::vector<smooth_info>&  s_info_vec_;
		const data_model&                  data_object_;
		const prior_model&                 prior_object_;
		//
		// set during constructor and otherwise const
		CppAD::vector<size_t>              value_prior_;
		//
		// solution found by run_fit and in pack_info form
		CppAD::vector<double>              solution_;
		//
		// temporaries in joint_density
		a5d_vector                                  a5_pack_vec_tmp_;
		CppAD::vector< residual_struct<a5_double> > data_like_tmp_;
		CppAD::vector< residual_struct<a5_double> > prior_ran_tmp_;
		a5d_vector                                  joint_den_tmp_;
		//
		// temporaries in prior_density
		a1d_vector                                  random_vec_tmp_;
		a1d_vector                                  a1_pack_vec_tmp_;
		CppAD::vector< residual_struct<a1_double> > prior_fix_tmp_;
		a1d_vector                                  prior_den_tmp_;
		// ---------------------------------------------------------------
		// pass joint density to base class
		virtual a5d_vector joint_density(
			const a5d_vector& fixed_vec   ,
			const a5d_vector& random_vec
		);
		// pass prior density to base class
		virtual a1d_vector prior_density(
			const a1d_vector& fixed_vec
		);
	// =======================================================================
	public:
		// constructor
		fit_model(
			const pack_info&                   pack_object  ,
			const CppAD::vector<prior_struct>& prior_table  ,
			const CppAD::vector<smooth_info>&  s_info_vec   ,
			const data_model&                  data_object  ,
			const prior_model&                 prior_object
		);
		// run fit
		void run_fit(
			const std::string& tolerance_str    ,
			const std::string& max_num_iter_str
		);
		// get_solution
		CppAD::vector<double> get_solution(void);
	};
}

# endif
