// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin fixed_effect dev}
{xrst_spell
   inverses
}

Setting and Getting the Fixed Effect Vector
###########################################

Syntax
******

| *n_fixed* = ``number_fixed`` ( *pack_object* )
| *pack_index* = ``fixed2var_id`` ( *pack_object* )
| ``pack_fixed`` ( *pack_object* , *pack_vec* , *fixed_vec* )
| ``unpack_fixed`` ( *pack_object* , *pack_vec* , *fixed_vec* )

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

Order of Fixed Effects
**********************
The order of the fixed effects is unspecified, except for the
fact that ``pack_fixed`` and ``unpack_fixed``
are inverses of each other; i.e., if you pack the fixed effects using a
*fixed_vec* , and then do a unpack, you will get that
*fixed_vec* back.

pack_object
***********
This argument has prototype

   ``const pack_info&`` *pack_object*

It is the :ref:`pack_info-name` information corresponding
to the :ref:`model_variables-name` .

n_fixed
*******
This return value has prototype

   ``size_t`` *n_fixed*

It is the number of
:ref:`fixed effects<model_variables@Fixed Effects, theta>` in the model.

pack_index
**********
This return value has prototype

   ``CppAD::vector<size_t>`` *pack_index*

It size is equal to *n_fixed* ; i.e., the number of
:ref:`fixed effects<model_variables@Fixed Effects, theta>` in the model.
For each fixed effect index
*j* = 0,..., *n_fixed* ``-1`` ,
the value *pack_index* [ *j* ] is the corresponding
index in a packed vector (with both fixed and random effects).

unpack_fixed
************
This functions copies information from *pack_vec*
to *fixed_vec* .

pack_vec
========
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and its size is *pack_object* . ``size`` () .
It specifies the value for all the :ref:`model_variables-name`
in :ref:`pack_info-name` format.

fixed_vec
=========
This argument has prototype

   ``CppAD::vector<`` *Float* >& *fixed_vec*

and its size is *n_fixed* .
It is a copy of the fixed effects in *pack_vec*
as one contiguous vector in an unspecified order.

pack_fixed
**********
This functions copies information from *fixed_vec*
to *pack_vec* .

pack_vec
========
This argument has prototype

   ``CppAD::vector<`` *Float* >& *pack_vec*

and its size is *pack_object* . ``size`` () .
It specifies the value for all the :ref:`model_variables-name`
in :ref:`pack_info-name` format.
The input value of its fixed effects does not matter.  Upon return,
the fixed effects correspond to the values in *fixed_vec* .

fixed_vec
=========
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *fixed_vec*

and its size is *n_fixed* .
It contains the fixed effects
as one contiguous vector in an unspecified order.
{xrst_toc_hidden
   example/devel/utility/fixed_effect_xam.cpp
}
Example
*******
The file :ref:`fixed_effect_xam.cpp-name`
contains an example and test that uses this routine.

{xrst_end fixed_effect}
*/

# include <dismod_at/pack_info.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// ---------------------------------------------------------------------------
// number fixed
size_t number_fixed(const pack_info&  pack_object)
{  assert( pack_object.size() > pack_object.random_size() );
   return pack_object.size() - pack_object.random_size();
}
// ---------------------------------------------------------------------------
// fixed2var_id
CppAD::vector<size_t> fixed2var_id(const pack_info& pack_object )
{
   size_t n_random = pack_object.random_size();
   size_t n_fixed  = number_fixed( pack_object );
   CppAD::vector<size_t> result( n_fixed );
   for(size_t fixed_index = 0; fixed_index < n_fixed; ++fixed_index)
      result[fixed_index] = n_random + fixed_index;

   return result;
}
// ---------------------------------------------------------------------------
// unpack_fixed
template <class Float>
void unpack_fixed(
   const pack_info&             pack_object  ,
   const CppAD::vector<Float>&  pack_vec     ,
   CppAD::vector<Float>&        fixed_vec    )
{  assert( fixed_vec.size() == number_fixed(pack_object) );
   assert( pack_vec.size()  == pack_object.size() );
   //
   size_t n_random = pack_object.random_size();
   size_t n_fixed  = number_fixed( pack_object );
   for(size_t fixed_index = 0; fixed_index < n_fixed; ++fixed_index)
      fixed_vec[fixed_index] = pack_vec[fixed_index + n_random];

   return;
}
// ---------------------------------------------------------------------------
// pack_fixed
template <class Float>
void pack_fixed(
   const pack_info&             pack_object  ,
   CppAD::vector<Float>&        pack_vec     ,
   const CppAD::vector<Float>&  fixed_vec    )
{  assert( fixed_vec.size() == number_fixed(pack_object) );
   assert( pack_vec.size()  == pack_object.size() );
   //
   size_t n_random = pack_object.random_size();
   size_t n_fixed  = number_fixed( pack_object );
   for(size_t fixed_index = 0; fixed_index < n_fixed; ++fixed_index)
      pack_vec[fixed_index + n_random] = fixed_vec[fixed_index];


   return;
}
// ---------------------------------------------------------------------------

# define DISMOD_AT_INSTANTIATE_FIXED_EFFECT(Float)        \
   template void unpack_fixed(                           \
   const pack_info&             pack_object  ,           \
   const CppAD::vector<Float>&  pack_vec     ,           \
   CppAD::vector<Float>&        fixed_vec                \
   );                                                    \
   template void pack_fixed(                             \
   const pack_info&             pack_object  ,           \
   CppAD::vector<Float>&        pack_vec     ,           \
   const CppAD::vector<Float>&  fixed_vec                \
   );

// instantiations
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( double )
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( a1_double )


} // END DISMOD_AT_NAMESPACE
