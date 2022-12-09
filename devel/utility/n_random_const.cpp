// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin number_random_const}

Determine Number of Random Effects that are Constant
####################################################

Syntax
******

| *n_random_const* = ``number_random_const`` (
| |tab| *bound_random* , *pack_object* , *var2prior* , *prior_table*
| )

bound_random
************
This is the value of the
:ref:`option_table@Optimize Random Only@bound_random`
in the option table.

pack_object
***********
This argument is the :ref:`pack_info-name` information corresponding to the
:ref:`model_variables-name` .

var2prior
*********
This argument is the :ref:`pack_prior-name` information corresponding to the
:ref:`model_variables-name` .

prior_table
***********
This argument is the :ref:`get_prior_table@prior_table` .

Prototype
*********
{xrst_literal
   // BEGIN PROTOTYPE
   // END PROTOTYPE
}
{xrst_toc_hidden
   example/devel/utility/n_random_const_xam.cpp
}
Example
*******
:ref:`n_random_const_xam.cpp-name`

{xrst_end number_random_const}
*/
# include <dismod_at/n_random_const.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at {
   // BEGIN PROTOTYPE
   size_t number_random_const(
      const pack_info&                     pack_object  ,
       const pack_prior&                    var2prior    ,
      const CppAD::vector<prior_struct>&   prior_table  )
   // END PROTOTYPE
   {  //
      // mapping from random effects to all variables
      CppAD::vector<size_t> pack_index = random2var_id(pack_object);
      assert( pack_index.size() == pack_object.random_size() );
      //
      // count how many random effects are constant
      size_t n_random_const = 0;
      for(size_t j = 0; j < pack_index.size(); j++)
      {  size_t var_id      = pack_index[j];
         size_t prior_id    = var2prior.value_prior_id(var_id);
         double const_value = var2prior.const_value(var_id);
         double max_abs     = var2prior.max_abs(var_id);
         if( ! std::isnan(const_value) )
         {  assert( prior_id == DISMOD_AT_NULL_SIZE_T );
            ++n_random_const;
         }
         else
         {  assert( prior_id != DISMOD_AT_NULL_SIZE_T );
            double lower = prior_table[prior_id].lower;
            double upper = prior_table[prior_id].upper;
            if( lower ==  upper )
               ++n_random_const;
            else if( max_abs == 0.0 )
               ++n_random_const;
         }
      }
      return n_random_const;
   }
}
