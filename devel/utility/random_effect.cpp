// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin random_effect dev}
{xrst_spell
   inverses
}

Setting and Getting the Random Effect Vector
############################################

Syntax
******

| *pack_index* = ``random2var_id`` ( *pack_object* )
| ``pack_random`` ( *pack_object* , *pack_vec* , *random_vec* )
| ``unpack_random`` ( *pack_object* , *pack_vec* , *random_vec* )

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

Order of Random Effects
***********************
The order of the random effects is unspecified, except for the
fact that ``pack_random`` and ``unpack_random``
are inverses of each other; i.e., if you pack the random effects using a
*random_vec* , and then do a unpack, you will get that
*random_vec* back.

pack_object
***********
This argument has prototype

   ``const pack_info&`` *pack_object*

It is the :ref:`pack_info-name` information corresponding
to the :ref:`model_variables-name` .

It is the number of
:ref:`random effects<model_variables@Random Effects, u>` in the model.

pack_index
**********
This return value has prototype

   ``CppAD::vector<size_t>`` *pack_index*

It size is equal to *n_random* ; i.e., the number of
:ref:`random effects<model_variables@Random Effects, u>` in the model.
For each random effect index
*j* = 0,..., *n_random* ``-1`` ,
*pack_index* [ *j* ] is the corresponding
index in a packed vector (with both fixed and random effects).

unpack_random
*************
This functions copies information from *pack_vec*
to *random_vec* .

pack_vec
========
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and its size is *pack_object* . ``size`` () .
It specifies the value for all the :ref:`model_variables-name`
in :ref:`pack_info-name` format.

random_vec
==========
This argument has prototype

   ``CppAD::vector<`` *Float* >& *random_vec*

and its size is *n_random* .
It is a copy of the random effects in *pack_vec*
as one contiguous vector in an unspecified order.

pack_random
***********
This functions copies information from *random_vec*
to *pack_vec* .

pack_vec
========
This argument has prototype

   ``CppAD::vector<`` *Float* >& *pack_vec*

and its size is *pack_object* . ``size`` () .
It specifies the value for all the :ref:`model_variables-name`
in :ref:`pack_info-name` format.
The input value of its random effects does not matter.  Upon return,
the random effects correspond to the values in *random_vec* .

random_vec
==========
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *random_vec*

and its size is *n_random* .
It contains the random effects
as one contiguous vector in an unspecified order.
{xrst_toc_hidden
   example/devel/utility/random_effect_xam.cpp
}
Example
*******
The file :ref:`random_effect_xam.cpp-name`
contains an example and test that uses this routine.

{xrst_end random_effect}
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
