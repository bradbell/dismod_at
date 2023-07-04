// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_RESIDUAL_DENSITY_HPP
# define DISMOD_AT_RESIDUAL_DENSITY_HPP

# include <utility>
# include "get_density_table.hpp"

namespace dismod_at {

   enum residual_enum {
      real_data_enum        ,
      simulated_data_enum   ,
      value_prior_enum      ,
      difference_prior_enum
   };

   template <class Float>
   struct residual_struct {
      Float              wres;
      Float              logden_smooth;
      Float              logden_sub_abs;
      density_enum       density;
      size_t             index;
   };

   template <class Float>
   residual_struct<Float> residual_density(
      residual_enum      residual_type  ,
      const Float&       z              ,
      const Float&       y              ,
      const Float&       mu             ,
      const Float&       delta          ,
      density_enum       d_id           ,
      const Float&       d_eta          ,
      const Float&       d_nu           ,
      const Float&       d_sample_size  ,
      size_t             index
   );
}

# endif
