// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_FIT_FIXED_HPP
# define DISMOD_AT_FIT_FIXED_HPP

# include <dismod_at/a5_double.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>

namespace dismod_at {

	// declare fit_fixed class
	class fit_fixed : public approx_mixed {
	public:
		// type declarations
		typedef CppAD::AD<double>          a1_double;
		typedef CppAD::vector<a1_double>   a1d_vector;
		typedef CppAD::vector<a5_double>   a5d_vector;
		//
	private:
		// const member variables
		const pack_info&    pack_object_;
		const data_model&   data_object_;
		const prior_model&  prior_object_;
		//
		// temporaries in joint_density
		a5d_vector                                  a5_pack_vec_tmp_;
		CppAD::vector< residual_struct<a5_double> > data_like_tmp_;
		CppAD::vector< residual_struct<a5_double> > prior_ran_tmp_;
		a5d_vector                                  joint_den_tmp_;
		// temporaries in prior_density
		a1d_vector                                  random_vec_tmp_;
		a1d_vector                                  a1_pack_vec_tmp_;
		CppAD::vector< residual_struct<a1_double> > prior_fix_tmp_;
		a1d_vector                                  prior_den_tmp_;
	public:
		// constructor
		fit_fixed(
			const pack_info&   pack_object    ,
			const data_model&  data_object    ,
			const prior_model& prior_object
		);
		// pass joint density to base class
		virtual a5d_vector joint_density(
			const a5d_vector& fixed_vec   ,
			const a5d_vector& random_vec
		);
		// pass prior density to base class
		virtual a1d_vector prior_density(
			const a1d_vector& fixed_vec
		);
	};
}

# endif
