// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_PRIOR_HPP
# define DISMOD_AT_PACK_PRIOR_HPP
# include <dismod_at/pack_info.hpp>
# include <dismod_at/smooth_info.hpp>

namespace dismod_at {

	// diff_prior_struct
	struct diff_prior_struct {
		enum { dage_enum, dtime_enum } direction;
		size_t plus_var_id;
		size_t minus_var_id;
		size_t prior_id;
	};
	// pack_prior
	class pack_prior {
	private:
		// one_prior_struct
		struct one_prior_struct {
			size_t  n_time;
			double  const_value;
			size_t  value_prior_id;
			size_t  dage_prior_id;
			size_t  dtime_prior_id;
			bool    fixed_effect;
		};
		// prior_vec_
		CppAD::vector<one_prior_struct> prior_vec_;
		// set_prior corresponding to one use of a smoothing
		static void set_prior(
			CppAD::vector<one_prior_struct>& prior_vec  ,
			size_t                           offset     ,
			const smooth_info&               s_info
		);
	public:
		pack_prior(
			const pack_info&                     pack_object  ,
			const CppAD::vector<smooth_info>&    s_info_vec
		);
		size_t size           (void)          const;
		double const_value    (size_t var_id) const;
		size_t value_prior_id (size_t var_id) const;
		size_t dage_prior_id  (size_t var_id) const;
		size_t dtime_prior_id (size_t var_id) const;
		size_t dage_var_id    (size_t var_id) const;
		size_t dtime_var_id   (size_t var_id) const;
		bool   fixed_effect   (size_t var_id) const;
	};

}

# endif
