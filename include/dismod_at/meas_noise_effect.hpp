// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
