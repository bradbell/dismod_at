// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_FIT_MODEL_HPP
# define DISMOD_AT_FIT_MODEL_HPP

# include <map>
# include <sqlite3.h>
# include <dismod_at/a2_double.hpp>
# include <cppad/mixed/cppad_mixed.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at {

	// declare fit_model class
	class fit_model : public cppad_mixed {
	public:
		// type declarations
		typedef CppAD::AD<double>          a1_double;
		typedef CppAD::vector<a1_double>   a1d_vector;
		typedef CppAD::vector<a2_double>   a2d_vector;
		//
	// =======================================================================
	private:
		// const member variables
		sqlite3*                           db_;
		const size_t                       n_fixed_;
		const size_t                       n_random_;
		const pack_info&                   pack_object_;
		const CppAD::vector<double>&       start_var_;
		const CppAD::vector<prior_struct>& prior_table_;
		const CppAD::vector<smooth_info>&  s_info_vec_;
		const data_model&                  data_object_;
		const prior_model&                 prior_object_;
		// -------------------------------------------------------------------
		// set during constructor and otherwise const
		//
		// prior for value of each variable
		CppAD::vector<size_t>             value_prior_;
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
		// Offset in log scaling for each component of the fixed vector;
		// If fixed_is_scaled_[j], fixed_vec[j] in dismod_at has value
		// log( fixed_vec[j] + fixed_scale_eta_[j] ) during optimization.
		CppAD::vector<bool>   fixed_is_scaled_;
		CppAD::vector<double> fixed_scale_eta_;
		// -------------------------------------------------------------------
		// solution found by run_fit and in pack_info form
		CppAD::vector<double>              solution_;
		// ---------------------------------------------------------------
		// implement_ran_like
		template <class Float>
		CppAD::vector<Float> implement_ran_like(
			const CppAD::vector<Float>& fixed_vec   ,
			const CppAD::vector<Float>& random_vec
		);
		// ---------------------------------------------------------------
		// scaling
		template <class Float>
		void scale_fixed_effect(
			const CppAD::vector<Float>& fixed_in  ,
			CppAD::vector<Float>&       fixed_out
		)
		{	assert( fixed_in.size()  == n_fixed_ );
			assert( fixed_out.size() == n_fixed_ );
			for(size_t j = 0; j < n_fixed_; j++)
			{	if( fixed_is_scaled_[j] )
					fixed_out[j] = log( fixed_in[j] + fixed_scale_eta_[j] );
				else
					fixed_out[j] = fixed_in[j];
			}
		}
		// unscaling
		template <class Float>
		void unscale_fixed_effect(
			const CppAD::vector<Float>& fixed_in  ,
			CppAD::vector<Float>&       fixed_out
		)
		{	assert( fixed_in.size()  == n_fixed_ );
			assert( fixed_out.size() == n_fixed_ );
			for(size_t j = 0; j < n_fixed_; j++)
			{	if( fixed_is_scaled_[j] )
					fixed_out[j] = exp( fixed_in[j] ) - fixed_scale_eta_[j];
				else
					fixed_out[j] = fixed_in[j];
			}
		}
		// -------------------------------------------------------------------
		// virtual functions used by cppad_mixed base class
		virtual a2d_vector ran_likelihood(
			const a2d_vector& fixed_vec   ,
			const a2d_vector& random_vec
		);
		virtual a1d_vector ran_likelihood(
			const a1d_vector& fixed_vec   ,
			const a1d_vector& random_vec
		);
		// pass fixed negative log-likelihood to base class
		virtual a1d_vector fix_likelihood(
			const a1d_vector& fixed_vec
		);
		// pass constraint functions to base class
		virtual a1d_vector fix_constraint(
			const a1d_vector& fixed_vec
		);
		// display error message and terminate program
		virtual void fatal_error(const std::string& error_message);
		// display warning message and continue
		virtual void warning(const std::string& warning_message);
	// =======================================================================
	public:
		// constructor
		fit_model(
			sqlite3*                             db           ,
			const pack_info&                     pack_object  ,
			const CppAD::vector<double>&         start_var    ,
			const CppAD::vector<prior_struct>&   prior_table  ,
			const CppAD::vector<smooth_info>&    s_info_vec   ,
			const data_model&                    data_object  ,
			const prior_model&                   prior_object ,
			bool                                 quasi_fixed  ,
			const CppAD::mixed::sparse_mat_info& A_info
		);
		//
		// run fit
		void run_fit(
			// effectively const
			std::map<std::string, std::string>& option_map
		);
		// get_solution
		CppAD::vector<double> get_solution(void);
	};
}

# endif
