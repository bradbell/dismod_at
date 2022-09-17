// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin random_effect$$
$spell
   vec
   CppAD
   var
   cpp
   const
$$

$section Setting and Getting the Random Effect Vector$$

$head Syntax$$
$icode%pack_index% = random2var_id(%pack_object%)
%$$
$codei%pack_random(%pack_object%, %pack_vec%, %random_vec%)
%$$
$codei%unpack_random(%pack_object%, %pack_vec%, %random_vec%)
%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head Order of Random Effects$$
The order of the random effects is unspecified, except for the
fact that $code pack_random$$ and $code unpack_random$$
are inverses of each other; i.e., if you pack the random effects using a
$icode random_vec$$, and then do a unpack, you will get that
$icode random_vec$$ back.

$head pack_object$$
This argument has prototype
$codei%
   const pack_info& %pack_object%
%$$
It is the $cref pack_info$$ information corresponding
to the $cref model_variables$$.

It is the number of
$cref/random effects/model_variables/Random Effects, u/$$ in the model.

$head pack_index$$
This return value has prototype
$codei%
   CppAD::vector<size_t> %pack_index%
%$$
It size is equal to $icode n_random$$; i.e., the number of
$cref/random effects/model_variables/Random Effects, u/$$ in the model.
For each random effect index
$icode%j%  = 0%,..., n_random%-1%$$,
$icode%pack_index%[%j%]%$$ is the corresponding
index in a packed vector (with both fixed and random effects).

$head unpack_random$$
This functions copies information from $icode pack_vec$$
to $icode random_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
   const CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.

$subhead random_vec$$
This argument has prototype
$codei%
   CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode n_random$$.
It is a copy of the random effects in $icode pack_vec$$
as one contiguous vector in an unspecified order.


$head pack_random$$
This functions copies information from $icode random_vec$$
to $icode pack_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
   CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.
The input value of its random effects does not matter.  Upon return,
the random effects correspond to the values in $icode random_vec$$.

$subhead random_vec$$
This argument has prototype
$codei%
   const CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode n_random$$.
It contains the random effects
as one contiguous vector in an unspecified order.

$children%
   example/devel/utility/random_effect_xam.cpp
%$$
$head Example$$
The file $cref random_effect_xam.cpp$$
contains an example and test that uses this routine.

$end
*/

# include <dismod_at/pack_info.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE
// -------------------------------------------------------------------------
// random2var_id
CppAD::vector<size_t> random2var_id(const pack_info& pack_object )
{
   size_t n_random = pack_object.random_size();
   CppAD::vector<size_t> result( n_random );
   for(size_t random_index = 0; random_index < n_random; ++random_index)
      result[random_index] = random_index;

   return result;
}
// -------------------------------------------------------------------------
// unpack_random
template <class Float>
void unpack_random(
   const pack_info&             pack_object  ,
   const CppAD::vector<Float>&  pack_vec   ,
   CppAD::vector<Float>&        random_vec )
{
   assert( random_vec.size() == pack_object.random_size() );
   assert( pack_vec.size()   == pack_object.size() );
   //
   size_t n_random = pack_object.random_size();
   for(size_t random_index = 0; random_index < n_random; ++random_index)
      random_vec[random_index] = pack_vec[random_index];

   return;
}
// -------------------------------------------------------------------------
// pack_random
template <class Float>
void pack_random(
   const pack_info&             pack_object  ,
   CppAD::vector<Float>&        pack_vec   ,
   const CppAD::vector<Float>&  random_vec )
{
   assert( random_vec.size() == pack_object.random_size() );
   assert( pack_vec.size()   == pack_object.size() );
   //
   size_t n_random = pack_object.random_size();
   for(size_t random_index = 0; random_index < n_random; ++random_index)
         pack_vec[random_index] = random_vec[random_index];

   return;
}

# define DISMOD_AT_INSTANTIATE_RANDOM_EFFECT(Float)           \
   template void unpack_random(                              \
   const pack_info&             pack_object  ,                \
   const CppAD::vector<Float>&  pack_vec   ,                 \
   CppAD::vector<Float>&        random_vec                   \
   );                                                        \
   template void pack_random(                                \
   const pack_info&             pack_object  ,               \
   CppAD::vector<Float>&        pack_vec   ,                 \
   const CppAD::vector<Float>&  random_vec                   \
   );

// instantiations
DISMOD_AT_INSTANTIATE_RANDOM_EFFECT( double )
DISMOD_AT_INSTANTIATE_RANDOM_EFFECT( a1_double )


} // END DISMOD_AT_NAMESPACE
