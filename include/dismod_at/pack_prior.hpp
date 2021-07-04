// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_PRIOR_HPP
# define DISMOD_AT_PACK_PRIOR_HPP
# include <dismod_at/pack_info.hpp>
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/get_prior_table.hpp>

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
			double  max_abs;
			double  const_value;
			size_t  n_time;
			size_t  smooth_id;
			size_t  mulcov_id;
			size_t  value_prior_id;
			size_t  dage_prior_id;
			size_t  dtime_prior_id;
			bool    fixed_effect;
		};
		// prior_vec_
		CppAD::vector<one_prior_struct> prior_vec_;
		//
		// set prior_vec_ corresponding to one use of a smoothing
		void set_prior_vec(
			double                               bound_random ,
			const CppAD::vector<prior_struct>&   prior_table  ,
			size_t                               offset       ,
			bool                                 fixed_effect ,
			size_t                               mulcov_id    ,
			size_t                               smooth_id    ,
			const CppAD::vector<smooth_info>&    s_info_vec
		);
	public:
		// ctor
		pack_prior(
			double                               bound_random        ,
			const CppAD::vector<size_t>&         n_child_data_in_fit ,
			const CppAD::vector<prior_struct>&   prior_table         ,
			const pack_info&                     pack_object         ,
			const CppAD::vector<smooth_info>&    s_info_vec
		);
		// set_bnd_mulcov
		void set_bnd_mulcov(
			const CppAD::vector<bnd_mulcov_struct>& bnd_mulcov_table
		);
		size_t size           (void)          const;
		double const_value    (size_t var_id) const;
		size_t smooth_id      (size_t var_id) const;
		size_t value_prior_id (size_t var_id) const;
		size_t dage_prior_id  (size_t var_id) const;
		size_t dtime_prior_id (size_t var_id) const;
		size_t dage_var_id    (size_t var_id) const;
		size_t dtime_var_id   (size_t var_id) const;
		bool   fixed_effect   (size_t var_id) const;
		double max_abs        (size_t var_id) const;
	};

}

# endif
