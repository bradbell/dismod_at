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

# include <map>
# include <sqlite3.h>
# include <dismod_at/a5_double.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/pack_prior.hpp>

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
		sqlite3*                           db_;
		const size_t                       n_fixed_;
		const size_t                       n_random_;
		const pack_info&                   pack_object_;
		const CppAD::vector<prior_struct>& prior_table_;
		const CppAD::vector<smooth_info>&  s_info_vec_;
		const data_model&                  data_object_;
		const prior_model&                 prior_object_;
		// -------------------------------------------------------------------
		// set during constructor and otherwise const
		//
		// prior for each variable
		CppAD::vector<size_t>             value_prior_;
		//
		// Prior for variable differences. Only difference for which
		// the upper limit is less than + infinity or the lower limit
		// is greater than minus infinity are included. Note that all the
		// corresponding variables should be fixed effects.
		CppAD::vector<diff_prior_struct>  diff_prior_;
		// Converts a packed variable id to index in fixed vector.
		// Uses the invalid value n_fixed_ if not a fixed variable.
		CppAD::vector<size_t>             var_id2fixed_;
		// -------------------------------------------------------------------
		// solution found by run_fit and in pack_info form
		CppAD::vector<double>              solution_;
		// ---------------------------------------------------------------
		// pass joint negative log-likelihood to base class
		virtual a5d_vector joint_like(
			const a5d_vector& fixed_vec   ,
			const a5d_vector& random_vec
		);
		// pass prior negative log-likelihood to base class
		virtual a1d_vector fix_like(
			const a1d_vector& fixed_vec
		);
		// pass constraint functions to base class
		virtual a1d_vector constraint(
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
			sqlite3*                           db           ,
			const pack_info&                   pack_object  ,
			const CppAD::vector<double>&       start_var    ,
			const CppAD::vector<prior_struct>& prior_table  ,
			const CppAD::vector<smooth_info>&  s_info_vec   ,
			const data_model&                  data_object  ,
			const prior_model&                 prior_object
		);
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
