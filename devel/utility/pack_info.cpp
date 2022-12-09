// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin pack_info_ctor}

Variable Pack Info: Constructor
###############################

Syntax
******

| ``pack_info`` *pack_object* (
| |tab| *n_integrand* ,
| |tab| *child_id2node_id* ,
| |tab| *subgroup_table* ,
| |tab| *smooth_table* ,
| |tab| *mulcov_table* ,
| |tab| *rate_table* ,
| |tab| *nslist_pair*
| )
| ``pack_info`` *pack_copy* ( *pack_object* )

n_integrand
***********
This argument has prototype

   ``size_t`` *n_integrand*

and is the number of integrands; i.e., the size of
:ref:`get_integrand_table@integrand_table` .
If :ref:`pack_info_ctor@mulcov_table` has size zero,
then *n_integrand* can be zero (a case used for testing purposes).

child_id2node_id
****************
This argument has prototype

   ``const CppAD::vector<size_t>`` *child2node*

and is a mapping from *child_id* to *node_id* ; see
:ref:`child_info@child_id2node_id` .
The size of this vector is the number of children; see
:ref:`child_info@child_size` .
If

   *rate_table* [ *rate_id* ]. ``child_nslist_id`` == ``DISMOD_AT_NULL_INT``

for all *rate_id* , only the size of this vector matters
(its values are not used).

subgroup_table
**************
This argument has prototype

   ``const CppAD::vector<subgroup_struct>&`` *subgroup_table*

and is the
:ref:`get_subgroup_table@subgroup_table` .

smooth_table
************
This argument has prototype

   ``const CppAD::vector<smooth_struct>&`` *smooth_table*

and is the
:ref:`get_smooth_table@smooth_table` .
Only the following fields of this table are used:
``n_age`` , ``n_time`` .

mulcov_table
************
This argument has prototype

   ``const CppAD::vector<mulcov_struct>&`` *mulcov_table*

and is the
:ref:`get_mulcov_table@mulcov_table` .

rate_table
**********
This argument has prototype

   ``const CppAD::vector<rate_struct>&`` *rate_table*

and is the
:ref:`get_rate_table@rate_table` .

nslist_pair
***********
This argument has prototype

   ``const CppAD::vector<nslist_pair_struct>&`` *nslist_pair*

and is the
:ref:`get_nslist_pair@nslist_pair` .
If

   *rate_table* [ *rate_id* ]. ``child_nslist_id`` == ``DISMOD_AT_NULL_INT``

for all *rate_id* , this table is not used and can be empty; i.e.,
have size zero.

pack_copy
*********
This object is a copy of the *pack_object* object
(and acts the same).

Age-Time Order
**************
When an function of age and time is stored with a specified *offset*
in a packed vector,
it is in time major order; i.e.,
for *i* = 0, ... , *n_age* ``-1`` ,
for *j* = 0, ... , *n_time* ``-1`` ,

   *offset* + *i* * *n_time* + *j*

is the index in the packed vector of the corresponding age-time point.

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_ctor}
*/

# include <cppad/cppad.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_info::pack_info(
   size_t                                    n_integrand      ,
   const CppAD::vector<size_t>               child_id2node_id ,
   const CppAD::vector<subgroup_struct>&     subgroup_table   ,
   const CppAD::vector<smooth_struct>&       smooth_table     ,
   const CppAD::vector<mulcov_struct>&       mulcov_table     ,
   const CppAD::vector<rate_struct>&         rate_table       ,
   const CppAD::vector<nslist_pair_struct>&  nslist_pair
) :
n_smooth_       ( smooth_table.size() )   ,
n_integrand_    ( n_integrand )           ,
n_child_        ( child_id2node_id.size() )
{  using std::string;

   // used to set all fields to null
   subvec_info null_info;
   null_info.mulcov_id    = DISMOD_AT_NULL_SIZE_T;
   null_info.covariate_id = DISMOD_AT_NULL_SIZE_T;
   null_info.smooth_id    = DISMOD_AT_NULL_SIZE_T;
   null_info.group_id     = DISMOD_AT_NULL_SIZE_T;
   null_info.n_var        = DISMOD_AT_NULL_SIZE_T;
   null_info.offset       = DISMOD_AT_NULL_SIZE_T;

   // set first_subgroup_id_, subgroup_size_
   size_t previous_group_id  = size_t( subgroup_table[0].group_id );
   assert( previous_group_id == 0 );
   size_t previous_subgroup_id = 0;
   first_subgroup_id_.push_back(previous_subgroup_id);
   size_t end_subgroup = subgroup_table.size();
   for(size_t subgroup_id = 1; subgroup_id < end_subgroup; ++subgroup_id)
   {  size_t group_id = size_t( subgroup_table[subgroup_id].group_id );
      if( group_id != previous_group_id )
      {  assert( group_id == previous_group_id + 1 );
         //
         subgroup_size_.push_back( subgroup_id - previous_subgroup_id );
         first_subgroup_id_.push_back( subgroup_id );
         previous_group_id    = group_id;
         previous_subgroup_id = subgroup_id;
      }
   }
   subgroup_size_.push_back( end_subgroup - previous_subgroup_id );

   // initialize offset
   size_t offset = 0;

   // resize by number of rates
   node_rate_value_info_.resize( number_rate_enum );
   group_rate_value_info_.resize( number_rate_enum );
   subgroup_rate_value_info_.resize( number_rate_enum );

   // resize by number of children
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
      node_rate_value_info_[rate_id].resize(n_child_ + 1);

   // resize by number of integrands
   group_meas_value_info_.resize( n_integrand );
   group_meas_noise_info_.resize( n_integrand );
   subgroup_meas_value_info_.resize( n_integrand );

   // -----------------------------------------------------------------------
   // random effects
   // -----------------------------------------------------------------------

   // node_rate_value_info_
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   for(size_t j = 0;  j < n_child_; j++)
   {  size_t child_nslist_id = rate_table[rate_id].child_nslist_id;
      size_t smooth_id = rate_table[rate_id].child_smooth_id;
      if( child_nslist_id != DISMOD_AT_NULL_SIZE_T )
      {  assert( smooth_id == DISMOD_AT_NULL_SIZE_T );
         //
         // search for the smooth_id for this child
         size_t child_node_id = child_id2node_id[j];
         for(size_t i = 0; i < nslist_pair.size(); i++)
         {  size_t nslist_id = nslist_pair[i].nslist_id;
            size_t node_id   = nslist_pair[i].node_id;
            bool   match     = nslist_id == child_nslist_id;
            match           &= node_id   == child_node_id;
            if( match )
               smooth_id = nslist_pair[i].smooth_id;
         }
         // following should have been checked previously
         assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
      }
      if( smooth_id == DISMOD_AT_NULL_SIZE_T )
         node_rate_value_info_[rate_id][j] = null_info;
      else
      {  size_t n_age  = smooth_table[smooth_id].n_age;
         size_t n_time = smooth_table[smooth_id].n_time;
         size_t n_var  = n_age * n_time;
         subvec_info& info = node_rate_value_info_[rate_id][j];
         info.mulcov_id    = DISMOD_AT_NULL_SIZE_T;
         info.covariate_id = DISMOD_AT_NULL_SIZE_T;
         info.group_id     = DISMOD_AT_NULL_SIZE_T;
         info.smooth_id    = smooth_id;
         info.n_var        = n_var;
         info.offset       = offset;
         //
         offset += n_var;
         //
         // check_rate_table should have checked this assumption
         assert( rate_id != pini_enum || n_age == 1 );
      }
   }

   // subgroup_rate_value_info_
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
   {  const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
      bool match;
      match  = mulcov_obj.mulcov_type  == rate_value_enum;
      match &= mulcov_obj.rate_id == int(rate_id);
      match &= mulcov_obj.subgroup_smooth_id != DISMOD_AT_NULL_INT;
      if( match )
      {
         size_t covariate_id = size_t(mulcov_obj.covariate_id);
         size_t group_id     = size_t(mulcov_obj.group_id);
         size_t smooth_id    = mulcov_obj.subgroup_smooth_id;
         size_t n_age        = smooth_table[smooth_id].n_age;
         size_t n_time       = smooth_table[smooth_id].n_time;
         size_t n_subgroup   = subgroup_size_[group_id];
         //
         CppAD::vector<subvec_info> info_vec(n_subgroup);
         for(size_t k = 0; k < n_subgroup; ++k)
         {  info_vec[k].mulcov_id    = mulcov_id;
            info_vec[k].covariate_id = covariate_id;
            info_vec[k].group_id     = size_t(mulcov_obj.group_id);
            info_vec[k].smooth_id    = smooth_id;
            info_vec[k].n_var        = n_age * n_time;
            info_vec[k].offset       = offset;
            offset += info_vec[k].n_var;
         }
         subgroup_rate_value_info_[rate_id].push_back( info_vec );
      }
   }

   // subgroup_meas_value_info_
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
   {  const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
      bool match;
      match  = mulcov_obj.mulcov_type  == meas_value_enum;
      match &= mulcov_obj.integrand_id == int(integrand_id);
      match &= mulcov_obj.subgroup_smooth_id != DISMOD_AT_NULL_INT;
      if( match )
      {
         size_t covariate_id = size_t(mulcov_obj.covariate_id);
         size_t group_id     = size_t(mulcov_obj.group_id);
         size_t smooth_id    = mulcov_obj.subgroup_smooth_id;
         size_t n_age        = smooth_table[smooth_id].n_age;
         size_t n_time       = smooth_table[smooth_id].n_time;
         size_t n_subgroup   = subgroup_size_[group_id];
         //
         CppAD::vector<subvec_info> info_vec(n_subgroup);
         for(size_t k = 0; k < n_subgroup; ++k)
         {  info_vec[k].mulcov_id    = mulcov_id;
            info_vec[k].covariate_id = covariate_id;
            info_vec[k].group_id     = size_t(mulcov_obj.group_id);
            info_vec[k].smooth_id    = smooth_id;
            info_vec[k].n_var        = n_age * n_time;
            info_vec[k].offset       = offset;
            offset += info_vec[k].n_var;
         }
         subgroup_meas_value_info_[integrand_id].push_back( info_vec );
      }
   }

   // n_random_
   n_random_ = offset;

   // -----------------------------------------------------------------------
   // fixed effects
   // -----------------------------------------------------------------------

   // mulstd_offset_
   mulstd_offset_.resize(3 * n_smooth_);
   for(size_t smooth_id = 0; smooth_id < n_smooth_; smooth_id++)
   {  int prior_id = smooth_table[smooth_id].mulstd_value_prior_id;
      if( prior_id == DISMOD_AT_NULL_INT )
         mulstd_offset_[smooth_id * 3 + 0] = DISMOD_AT_NULL_SIZE_T;
      else
         mulstd_offset_[smooth_id * 3 + 0] = offset++;
      //
      prior_id = smooth_table[smooth_id].mulstd_dage_prior_id;
      if( prior_id == DISMOD_AT_NULL_INT )
         mulstd_offset_[smooth_id * 3 + 1] = DISMOD_AT_NULL_SIZE_T;
      else
         mulstd_offset_[smooth_id * 3 + 1] = offset++;
      //
      prior_id = smooth_table[smooth_id].mulstd_dtime_prior_id;
      if( prior_id == DISMOD_AT_NULL_INT )
         mulstd_offset_[smooth_id * 3 + 2] = DISMOD_AT_NULL_SIZE_T;
      else
         mulstd_offset_[smooth_id * 3 + 2] = offset++;
   }

   // node_rate_value_info_
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   {  size_t smooth_id  = rate_table[rate_id].parent_smooth_id;
      subvec_info& info = node_rate_value_info_[rate_id][n_child_];

      if( smooth_id == DISMOD_AT_NULL_SIZE_T )
         info = null_info;
      else
      {  size_t n_age  = smooth_table[smooth_id].n_age;
         size_t n_time = smooth_table[smooth_id].n_time;
         size_t n_var  = n_age * n_time;
         info.mulcov_id    = DISMOD_AT_NULL_SIZE_T;
         info.covariate_id = DISMOD_AT_NULL_SIZE_T;
         info.group_id     = DISMOD_AT_NULL_SIZE_T;
         info.smooth_id    = smooth_id;
         info.n_var        = n_var;
         info.offset       = offset;
         //
         offset += n_var;
         //
         // check_rate_table should have checked this assumption
         assert( rate_id != pini_enum || n_age == 1 );
      }
   }

   // group_meas_value_info_ and group_meas_noise_info_
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
   {  const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
      bool match;
      match  = mulcov_obj.mulcov_type  == meas_value_enum;
      match |= mulcov_obj.mulcov_type  == meas_noise_enum;
      match &= mulcov_obj.integrand_id == int(integrand_id);
      match &= mulcov_obj.group_smooth_id != DISMOD_AT_NULL_INT;
      if( match )
      {  size_t covariate_id = size_t(mulcov_obj.covariate_id);
         string mulcov_type;
         CppAD::vector<subvec_info>* info_vec = DISMOD_AT_NULL_PTR;
         if( mulcov_obj.mulcov_type == meas_value_enum )
         {  info_vec    = &( group_meas_value_info_[integrand_id]) ;
            mulcov_type = "'meas_value'";
         }
         if( mulcov_obj.mulcov_type == meas_noise_enum )
         {  info_vec    = &( group_meas_noise_info_[integrand_id]) ;
            mulcov_type = "'meas_noise'";
         }
         for(size_t j = 0; j < info_vec->size(); j++)
         {  if( (*info_vec)[j].covariate_id == covariate_id )
            {  string msg = "covariate_id appears twice with "
                  "mulcov_type equal to ";
               msg += mulcov_type;
               string table_name = "mulcov";
               error_exit(msg, table_name, mulcov_id);
            }
         }
         size_t smooth_id = mulcov_obj.group_smooth_id;
         size_t n_age     = smooth_table[smooth_id].n_age;
         size_t n_time    = smooth_table[smooth_id].n_time;
         //
         subvec_info info;
         info.mulcov_id    = mulcov_id;
         info.covariate_id = covariate_id;
         info.group_id     = size_t(mulcov_obj.group_id);
         info.smooth_id    = smooth_id;
         info.n_var        = n_age * n_time;
         info.offset       = offset;
         info_vec->push_back(info);
         //
         offset           += info.n_var;
      }
   }

   // group_rate_value_info_
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
   {  const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
      bool match;
      match  = mulcov_obj.mulcov_type  == rate_value_enum;
      match &= mulcov_obj.rate_id == int(rate_id);
      match &= mulcov_obj.group_smooth_id != DISMOD_AT_NULL_INT;
      if( match )
      {  size_t covariate_id = size_t(mulcov_obj.covariate_id);
         CppAD::vector<subvec_info>& info_vec =
            group_rate_value_info_[rate_id];
         for(size_t j = 0; j < info_vec.size(); j++)
         {  if( info_vec[j].covariate_id == covariate_id )
            {  string msg = "covariate_id appears twice with "
                  "mulcov_type equal to 'rate_value'";
               string table_name = "mulcov";
               error_exit(msg, table_name, mulcov_id);
            }
         }
         size_t smooth_id = mulcov_obj.group_smooth_id;
         size_t n_age     = smooth_table[smooth_id].n_age;
         size_t n_time    = smooth_table[smooth_id].n_time;
         //
         subvec_info info;
         info.mulcov_id    = mulcov_id;
         info.covariate_id = covariate_id;
         info.group_id     = size_t(mulcov_obj.group_id);
         info.smooth_id    = smooth_id;
         info.n_var        = n_age * n_time;
         info.offset       = offset;
         info_vec.push_back(info);
         //
         offset           += info.n_var;
      }
   }

   // size is final offset
   size_ = offset;
}
// use default copy constructor
/*
-------------------------------------------------------------------------------
{xrst_begin pack_info_sizes}
{xrst_spell
   subgroups
}

Variable Pack Info: Sizes
#########################

Syntax
******

| *size* = *pack_object* . ``size`` ()
| *integrand_size* = *pack_object* . ``integrand_size`` ()
| *child_size* = *pack_object* . ``child_size`` ()
| *smooth_size* = *pack_object* . ``smooth_size`` ()
| *random_size* = *pack_object* . ``random_size`` ()
| *group_size* = *pack_object* . ``group_size`` ()
| *subgroup_size* = *pack_object* . ``subgroup_size`` ( *group_id* )
| *first_subgroup_id* = *pack_object* . ``first_subgroup_id`` ( *group_id* )

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

i.e., these member functions to not modify *pack_object* .

size
****
Its return value has prototype

   ``size_t`` *size*

and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

integrand_size
**************
This return value has prototype

   ``size_t`` *integrand_size*

and is the size of the
:ref:`get_integrand_table@integrand_table` .

child_size
**********
This return value has prototype

   ``size_t`` *child_size*

and is the number of children.

smooth_size
***********
This return value has prototype

   ``size_t`` *smooth_size*

and is the size of
:ref:`get_smooth_table@smooth_table` .

random_size
***********
This return value has prototype

   ``size_t`` *random_size*

and is the number of
:ref:`random effects<model_variables@Random Effects, u>` in the model
(counting those that are constrained with equal upper and lower limits).

group_size
**********
This return value has prototype

   ``size_t`` *group_size*

and is the number of groups in the :ref:`subgroup_table-name` .

group_id
********
This argument has prototype

   ``size_t`` *group_id*

and must be less than *group_size* .

subgroup_size
*************
This return value has prototype

   ``size_t`` *subgroup_size*

and is the number of subgroups in the specified group.

first_subgroup_id
*****************
This return value has prototype

   ``size_t`` *first_subgroup_id*

and is the index of the first subgroup in the specified group.

{xrst_end pack_info_sizes}
*/


// size
size_t pack_info::size(void) const
{  return size_; }

// integrand_size
size_t pack_info::integrand_size(void) const
{  return n_integrand_; }

// child_size
size_t pack_info::child_size(void) const
{  return n_child_; }

// smooth_size
size_t pack_info::smooth_size(void) const
{  return n_smooth_; }

// random_size
size_t pack_info::random_size(void) const
{  return n_random_; }

// group_size
size_t pack_info::group_size(void) const
{  return subgroup_size_.size(); }

// subgroup_size
size_t pack_info::subgroup_size(size_t group_id) const
{  return subgroup_size_[group_id]; }

// first_subgroup_id
size_t pack_info::first_subgroup_id(size_t group_id) const
{  return first_subgroup_id_[group_id]; }


/*
------------------------------------------------------------------------------
{xrst_begin pack_info_mulstd}

Variable Pack Info: Smoothing Standard Deviation Multipliers
############################################################

Syntax
******

   *offset* = *pack_object* . ``mulstd_offset`` ( *smooth_id* , *k* )

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

smooth_id
*********
This argument has prototype

   ``size_t`` *smooth_id*

and is the :ref:`smooth_table@smooth_id`
for the smoothing that this multiplier effects.

k
*
This argument has prototype

   ``size_t`` *k*

It specifies which type of prior standard deviations that get multiplied
by this variable as follows:

.. csv-table::
   :widths: auto

   *k*,Type of prior
   ``0``,value priors for this smoothing
   ``1``,age difference priors for this smoothing
   ``2``,time difference priors for this smoothing

offset
======
The return value has prototype

   ``size_t offset``

and is the offset (index) in the packed variable vector
where this multiplier is located.
If *offset*  has

   ``DISMOD_AT_NULL_SIZE_T``

for *k*  = 0 , then it has the same value for
*k*  = 1, 2 .
In this case, the multiplier has value one and is not a variable.

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_mulstd}

*/
size_t pack_info::mulstd_offset(size_t smooth_id, size_t k) const
{  assert( smooth_id < n_smooth_ );
   assert( k < 3 );
   return mulstd_offset_[3 * smooth_id + k];
}
/*
------------------------------------------------------------------------------
{xrst_begin pack_info_node_rate}
{xrst_spell
   subvec
}

Variable Pack Info: Node Rates
##############################

Syntax
******

   *info* = *pack_object* . ``node_rate_value_info`` ( *rate_id* , *j* )

subvec_info
***********
the type ``pack_info::subvec_info`` is defined as follows:
{xrst_literal
   include/dismod_at/pack_info.hpp
   // BEGIN SUBVEC_INFO
   // END SUBVEC_INFO
}

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

rate_id
*******
This argument has prototype

   ``size_t`` *rate_id*

and it specifies the
:ref:`rate_table@rate_id` the rate values.

j
*
This argument has prototype

   ``size_t`` *j*

and *j* <= *n_child* .

info
****
The return value  has prototype

   ``pack_info::subvec_info`` *info*

mulcov_id
=========
This field is set to null.

covariate_id
============
This field is set to null.

group_id
========
This field is set to null.

smooth_id
=========
is the :ref:`mulcov_table@group_smooth_id` for this rate.
If *j* == *n_child* ,
this smoothing corresponds to the parent rates.
Otherwise it corresponds to the child rate effects and is the same
for all children.
If

   *smooth_id* == ``DISMOD_AT_NULL_SIZE_T``

then this rate is identically zero and there are no corresponding variables.

n_var
=====
is the number of packed variables for this *rate_id* ; i.e.
the product of the number of age and number of time points in the smoothing
for this rate.

offset
======
is the offset (index) in the packed variable vector for the
specified rates:
If *j* < *n_child* ,
it is the rate vector for the *j*-th child node.
If *j* == *n_child* ,
this is the rate vector for the
:ref:`option_table@Parent Node` .
(Not specified when *smooth_id* corresponds to ``null`` ).

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_node_rate}
*/
pack_info::subvec_info pack_info::node_rate_value_info(size_t rate_id, size_t j) const
{  assert( j <= n_child_ );
   return node_rate_value_info_[rate_id][j];
}

/*
------------------------------------------------------------------------------
{xrst_begin pack_info_group_meas}
{xrst_spell
   subvec
}

Variable Pack Info: Group Measurement Covariate Multipliers
###########################################################

Syntax
******

| *n_cov* = *pack_object* . ``group_meas_value_n_cov`` ( *integrand_id* )
| *n_cov* = *pack_object* . ``group_meas_noise_n_cov`` ( *integrand_id* )
| *info* = *pack_object* . ``group_meas_value_info`` ( *integrand_id* , *j* )
| *info* = *pack_object* . ``group_meas_noise_info`` ( *integrand_id* , *j* )

meas_value
**********
The functions
``group_meas_value_n_cov`` and
``group_meas_value_info``
return information about the measurement mean covariate multipliers.

meas_noise
**********
The functions
``group_meas_noise_n_cov`` and
``group_meas_noise_info``
return information about the measurement noise covariate multipliers.

subvec_info
***********
The type ``pack_info::subvec_info`` is defined as follows:
{xrst_literal
   include/dismod_at/pack_info.hpp
   // BEGIN SUBVEC_INFO
   // END SUBVEC_INFO
}

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

integrand_id
************
This argument has prototype

   ``size_t`` *integrand_id*

and it specifies the
:ref:`integrand_table@integrand_id` for the covariate
multipliers.

n_cov
*****
This return value has prototype

   ``size_t`` *n_cov*

and is the number of covariate multipliers
(rows in :ref:`mulcov_table-name` ) for the specified *integrand_id* .
This is referred to as ``n_cov`` ( *integrand_id* ) below.

j
*
This argument has prototype

   ``size_t`` *j*

and *j* < ``n_cov`` ( *integrand_id* ) .
For each fixed *integrand_id* , the
:ref:`mulcov_table@mulcov_id` index corresponding to *j*
is monotone increasing with *j* .

info
****
this return value has prototype

   ``pack_info::subvec_info`` *info*

mulcov_id
=========
is the :ref:`mulcov_table@mulcov_id` for the
*j*-th covariate multiplier for this *integrand_id* .

covariate_id
============
is the :ref:`covariate_table@covariate_id` for the
*j*-th covariate multiplier for this *integrand_id* .

group_id
========
is the :ref:`mulcov_table@group_id` for the
*j*-th covariate multiplier for this *integrand_id* .

smooth_id
=========
is the :ref:`mulcov_table@group_smooth_id` for the
*j*-th covariate multiplier for this *integrand_id* .

n_var
=====
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this *smooth_id* .

offset
======
is the offset in the packed variable vector where the
*n_var* variables begin (for this covariate multiplier).

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_group_meas}
*/
size_t pack_info::group_meas_value_n_cov(size_t integrand_id) const
{  assert( integrand_id < n_integrand_ );
   return group_meas_value_info_[integrand_id].size();
}
size_t pack_info::group_meas_noise_n_cov(size_t integrand_id) const
{  assert( integrand_id < n_integrand_ );
   return group_meas_noise_info_[integrand_id].size();
}
//
pack_info::subvec_info
pack_info::group_meas_value_info(size_t integrand_id, size_t j) const
{  assert( integrand_id < n_integrand_ );
   return group_meas_value_info_[integrand_id][j];
}
pack_info::subvec_info
pack_info::group_meas_noise_info(size_t integrand_id, size_t j) const
{  assert( integrand_id < n_integrand_ );
   return group_meas_noise_info_[integrand_id][j];
}

/*
------------------------------------------------------------------------------
{xrst_begin pack_info_group_rate}
{xrst_spell
   subvec
}

Variable Pack Info: Group Rate Covariate Multipliers
####################################################

Syntax
******

| *n_cov* = *pack_object* . ``group_rate_value_n_cov`` ( *rate_id* )
| *info* = *pack_object* . ``group_rate_value_info`` ( *rate_id* , *j* )

subvec_info
***********
The type ``pack_info::subvec_info`` is defined as follows:
{xrst_literal
   include/dismod_at/pack_info.hpp
   // BEGIN SUBVEC_INFO
   // END SUBVEC_INFO
}

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

rate_id
*******
This argument has prototype

   ``size_t`` *rate_id*

and it specifies the
:ref:`rate_table@rate_id` for the covariate
multipliers.

n_cov
*****
This return value has prototype

   ``size_t`` *n_cov*

and is the number of covariate multipliers
(rows in :ref:`mulcov_table-name` ) for the specified *rate_id* .
This is referred to as ``n_cov`` ( *rate_id* ) below.

j
*
This argument has prototype

   ``size_t`` *j*

and *j* < ``n_cov`` ( *rate_id* ) .
For each fixed *rate_id* , the
:ref:`mulcov_table@mulcov_id` index corresponding to *j*
is monotone increasing with *j* .

info
****
this return value has prototype

   ``pack_info::subvec_info`` *info*

mulcov_id
=========
is the :ref:`mulcov_table@mulcov_id` for the
*j*-th covariate multiplier for this *integrand_id* .

covariate_id
============
is the :ref:`covariate_table@covariate_id`
for the *j*-th covariate multiplier for this *rate_id* .

group_id
========
is the :ref:`mulcov_table@group_id`
for the *j*-th covariate multiplier for this *rate_id* .

smooth_id
=========
is the :ref:`mulcov_table@group_smooth_id` for the
*j*-th covariate multiplier for this *rate_id* .

n_var
=====
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this *smooth_id* .

offset
======
is the offset in the packed variable vector where the
*n_var* variables begin (for this covariate multiplier).

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_group_rate}
*/
size_t pack_info::group_rate_value_n_cov(size_t rate_id) const
{  assert( rate_id < number_rate_enum );
   return group_rate_value_info_[rate_id].size();
}
//
pack_info::subvec_info
pack_info::group_rate_value_info(size_t rate_id, size_t j) const
{  assert( rate_id < number_rate_enum );
   return group_rate_value_info_[rate_id][j];
}
/*
------------------------------------------------------------------------------
{xrst_begin pack_info_subgroup_rate}
{xrst_spell
   subgroups
   subvec
}

Variable Pack Info: Subgroup Rate Covariate Multipliers
#######################################################

Syntax
******

| *n_cov* = *pack_object* . ``subgroup_rate_value_n_cov`` ( *rate_id* )
| *n_sub* = *pack_object* . ``subgroup_rate_value_n_sub`` ( *rate_id* , *j* )
| *info* = *pack_object* . ``subgroup_rate_value_info`` ( *rate_id* , *j* , *k* )

subvec_info
***********
The type ``pack_info::subvec_info`` is defined as follows:
{xrst_literal
   include/dismod_at/pack_info.hpp
   // BEGIN SUBVEC_INFO
   // END SUBVEC_INFO
}

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

rate_id
*******
This argument has prototype

   ``size_t`` *rate_id*

and it specifies the
:ref:`rate_table@rate_id`
for the covariate multipliers.

n_cov
*****
This return value has prototype

   ``size_t`` *n_cov*

and is the number of covariate multipliers
(rows in :ref:`mulcov_table-name` ) for the specified *rate_id* .
This is referred to as ``n_cov`` ( *rate_id* ) below.

n_sub
*****
This return value has prototype

   ``size_t`` *n_sub*

and is the number of subgroups corresponding to the
:ref:`group<mulcov_table@group_id>` for this covariate multiplier.
This is referred to as ``n_sub`` ( *rate_id* , *j* ) below.

j
*
This argument has prototype

   ``size_t`` *j*

and *j* < ``n_cov`` ( *rate_id* ) .
For each fixed *rate_id* , the
:ref:`mulcov_table@mulcov_id` index corresponding to *j*
is monotone increasing with *j* .

k
*
This argument has prototype

   ``size_t`` *k*

and *k* < ``n_sub`` ( *rate_id* , *j* ) .
For each fixed *rate_id* and *j* , the
:ref:`subgroup_table@subgroup_id` index corresponding to
*k* is monotone increasing with *k* .

info
****
this return value has prototype

   ``pack_info::subvec_info`` *info*

mulcov_id
=========
is the :ref:`mulcov_table@mulcov_id` for the
*j*-th covariate multiplier for this *integrand_id* .

covariate_id
============
is the :ref:`covariate_table@covariate_id` for the
*j*-th covariate multiplier for this *rate_id* .

group_id
========
is the :ref:`mulcov_table@group_id` for the
*j*-th covariate multiplier for this *integrand_id* .

smooth_id
=========
is the :ref:`mulcov_table@subgroup_smooth_id` for the
*j*-th covariate multiplier, and this *rate_id* .
Note that the smoothing does not depend on *k* ; see
:ref:`mulcov_table@subgroup_smooth_id` .

n_var
=====
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this *smooth_id* .

offset
======
is the offset in the packed variable vector where the
*n_var* variables begin (for this covariate multiplier and subgroup).

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_subgroup_rate}
*/
size_t pack_info::subgroup_rate_value_n_cov(size_t rate_id) const
{  assert( rate_id < number_rate_enum );
   return subgroup_rate_value_info_[rate_id].size();
}
size_t pack_info::subgroup_rate_value_n_sub(size_t rate_id, size_t j) const
{  assert( rate_id < number_rate_enum );
   return subgroup_rate_value_info_[rate_id][j].size();
}
//
pack_info::subvec_info pack_info::subgroup_rate_value_info(
   size_t rate_id, size_t j, size_t k
) const
{  assert( rate_id < number_rate_enum );
   return subgroup_rate_value_info_[rate_id][j][k];
}
/*
------------------------------------------------------------------------------
{xrst_begin pack_info_subgroup_meas}
{xrst_spell
   subgroups
   subvec
}

Variable Pack Info: Subgroup Measurement Covariate Multipliers
##############################################################

Syntax
******

| *n_cov* = *pack_object* . ``subgroup_meas_value_n_cov`` ( *integrand_id* )
| *n_sub* = *pack_object* . ``subgroup_meas_value_n_sub`` ( *integrand_id* , *j* )
| *info* = *pack_object* . ``subgroup_meas_value_info`` ( *integrand_id* , *j* , *k* )

subvec_info
***********
The type ``pack_info::subvec_info`` is defined as follows:
{xrst_literal
   include/dismod_at/pack_info.hpp
   // BEGIN SUBVEC_INFO
   // END SUBVEC_INFO
}

pack_object
***********
This object has prototype

   ``const pack_info`` *pack_object*

integrand_id
************
This argument has prototype

   ``size_t`` *integrand_id*

and it specifies the
:ref:`integrand_table@integrand_id`
for the covariate multipliers.

n_cov
*****
This return value has prototype

   ``size_t`` *n_cov*

and is the number of covariate multipliers
(rows in :ref:`mulcov_table-name` ) for the specified *integrand_id* .
This is referred to as ``n_cov`` ( *integrand_id* ) below.

n_sub
*****
This return value has prototype

   ``size_t`` *n_sub*

and is the number of subgroups corresponding to the
:ref:`group<mulcov_table@group_id>` for this covariate multiplier.
This is referred to as ``n_sub`` ( *integrand_id* , *j* ) below.

j
*
This argument has prototype

   ``size_t`` *j*

and *j* < ``n_cov`` ( *integrand_id* ) .
For each fixed *integrand_id* , the
:ref:`mulcov_table@mulcov_id` index corresponding to *j*
is monotone increasing with *j* .

k
*
This argument has prototype

   ``size_t`` *k*

and *k* < ``n_sub`` ( *integrand_id* , *j* ) .
For each fixed *integrand_id* and *j* , the
:ref:`subgroup_table@subgroup_id` index corresponding to
*k* is monotone increasing with *k* .

info
****
this return value has prototype

   ``pack_info::subvec_info`` *info*

mulcov_id
=========
is the :ref:`mulcov_table@mulcov_id` for the
*j*-th covariate multiplier for this *integrand_id* .

covariate_id
============
is the :ref:`covariate_table@covariate_id` for the
*j*-th covariate multiplier for this *integrand_id* .

group_id
========
is the :ref:`mulcov_table@group_id` for the
*j*-th covariate multiplier for this *integrand_id* .

smooth_id
=========
is the :ref:`mulcov_table@subgroup_smooth_id` for the
*j*-th covariate multiplier, and this *integrand_id* .
Note that the smoothing does not depend on *k* ; see
:ref:`mulcov_table@subgroup_smooth_id` .

n_var
=====
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this *smooth_id* .

offset
======
is the offset in the packed variable vector where the
*n_var* variables begin (for this covariate multiplier and subgroup).

Example
*******
See :ref:`pack_info Example<pack_info@Example>` .

{xrst_end pack_info_subgroup_meas}
*/
size_t pack_info::subgroup_meas_value_n_cov(size_t integrand_id) const
{  assert( integrand_id < n_integrand_ );
   return subgroup_meas_value_info_[integrand_id].size();
}
size_t pack_info::subgroup_meas_value_n_sub(size_t integrand_id, size_t j) const
{  assert( integrand_id < n_integrand_ );
   return subgroup_meas_value_info_[integrand_id][j].size();
}
//
pack_info::subvec_info pack_info::subgroup_meas_value_info(
   size_t integrand_id, size_t j, size_t k
) const
{  assert( integrand_id < n_integrand_ );
   return subgroup_meas_value_info_[integrand_id][j][k];
}

} // END DISMOD_AT_NAMESPACE
