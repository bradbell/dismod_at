// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_FIXED_EFFECT_HPP
# define DISMOD_AT_FIXED_EFFECT_HPP

# include <cppad/cppad.hpp>
# include "pack_info.hpp"

namespace dismod_at {
   extern size_t number_fixed(const pack_info& pack_object);
   //
   extern CppAD::vector<size_t> fixed2var_id(
      const pack_info&             pack_object
   );
   //
   template <class Float>
   extern void unpack_fixed(
      const pack_info&            pack_object  ,
      const CppAD::vector<Float>& pack_vec     ,
      CppAD::vector<Float>&       fixed_vec
   );
   //
   template <class Float>
   extern void pack_fixed(
      const pack_info&            pack_object  ,
      CppAD::vector<Float>&       pack_vec     ,
      const CppAD::vector<Float>& fixed_vec
   );
}

# endif
