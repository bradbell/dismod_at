// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PRIOR_MODEL_HPP
# define DISMOD_AT_PRIOR_MODEL_HPP

# include <cppad/utility/vector.hpp>
# include "pack_info.hpp"
# include "get_prior_table.hpp"
# include "get_smooth_table.hpp"
# include "smooth_info.hpp"
# include "residual_density.hpp"
# include "pack_prior.hpp"

namespace dismod_at {
	class prior_model {
	private:
		// const data
		const pack_info                    pack_object_;
		const pack_prior                   var2prior_;
		const CppAD::vector<double>        age_table_;
		const CppAD::vector<double>        time_table_;
		const CppAD::vector<prior_struct>  prior_table_;

		// means used for priors, see documentation in replace_mean
		CppAD::vector<double>              prior_mean_;

		// log_prior
		template <class Float>
		residual_struct<Float> log_prior(
			const prior_struct& prior        ,
			const Float&        mulstd       ,
			const Float&        z            ,
			const Float&        y            ,
			size_t              index        ,
			bool                difference
		) const;
	public:
		// ctor
		prior_model(
			const pack_info&                       pack_object     ,
			const pack_prior&                      var2prior       ,
			const CppAD::vector<double>&           age_table       ,
			const CppAD::vector<double>&           time_table      ,
			const CppAD::vector<prior_struct>&     prior_table
		);
		// replace_mean
		void replace_mean(const CppAD::vector<double>& prior_mean);
		// fixed
		template <class Float>
		CppAD::vector< residual_struct<Float> > fixed(
			const CppAD::vector<Float>& pack_vec
		) const;
		// random
		template <class Float>
		CppAD::vector< residual_struct<Float> > random(
			const CppAD::vector<Float>& pack_vec
		) const;
	};
}

# endif
