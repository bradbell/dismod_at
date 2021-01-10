/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_MEAS_NOISE_EFFECT_HPP
# define DISMOD_AT_MEAS_NOISE_EFFECT_HPP

namespace dismod_at {
	enum meas_noise_effect_enum {
		add_std_scale_all_enum,
		add_std_scale_none_enum,
		add_std_scale_log_enum,
		//
		add_var_scale_all_enum,
		add_var_scale_none_enum,
		add_var_scale_log_enum,
		//
		number_meas_noise_effect_enum
	};
}

# endif
