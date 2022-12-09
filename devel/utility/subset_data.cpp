// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin subset_data dev}
{xrst_spell
   subsampled
   subsamples
}

Create a Subsampled Version of Data Table
#########################################

See Also
********
:ref:`avgint_subset-name` .

Syntax
******

| ``subset_data`` (
| |tab| *option_map* , *data_table* , *integrand_table* ,
| |tab| *data_table* , *data_cov_value* , *covariate_table* , *child_info4data* ,
| |tab| *subset_data_obj* , *subset_data_cov_value*
| )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
This routine subsamples the *data_table* , in the following way:

#. Only rows corresponding to the
   :ref:`option_table@Parent Node` ,
   or a descendant of the parent node, are included.
#. Only rows for which the covariates satisfy the
   :ref:`covariate_table@max_difference` criteria
   are included.
#. For each covariate, its
   :ref:`covariate_table@reference` value is subtracted
   from the value of the covariate in *data_table* .
#. For each integrand in the
   :ref:`option_table@hold_out_integrand` list,
   the hold is set to one, no matter what
   :ref:`data_table@hold_out` is in the data table.
#. :ref:`compression intervals<option_table@compress_interval>`
   are enforced.
#. All of the
   :ref:`data_table@node_id@Child Data`
   is checked to make sure that it does not use a
   :ref:`density_table@density_name@laplace` or
   :ref:`density_table@density_name@log_laplace` density.

option_map
**********
This contains the following keys:

hold_out_integrand
==================
If this key is present in *option_map* , it is the
:ref:`option_table@hold_out_integrand` .
If it is not present, the empty string is used.

data_subset_table
*****************
is the :ref:`get_data_subset@data_subset_table` .

integrand_table
***************
is the :ref:`get_integrand_table@integrand_table` .

data_table
**********
is the :ref:`get_data_table@data_table` .

data_cov_value
**************
is the :ref:`data_table<get_data_table@data_cov_value>`
covariate values.

covariate_table
***************
is the :ref:`get_covariate_table@covariate_table` .

child_info4data
***************
is a :ref:`child_info-name` object created using the data table.

subset_data_obj
***************
The input size is zero and upon return
its size is the number of rows in *data_table* that satisfy
the purpose above.
The structure has all the fields that are present in
:ref:`get_data_table@data_table@data_struct` .

n_subset
========
We use the notation *n_subset* = *subset_data_obj* . ``size`` () .

subset_id
=========
We use the notation *subset_id* for an index between
zero and *n_subset* ``-1`` ,

original_id
===========
There an extra field in ``subset_data_struct`` that has
name ``original_id`` , type ``int`` .
The values in this field are equal to the
*original_id* for the corresponding row of :ref:`data_table-name` .
The value of

   *subset_data_obj* [ *subset_id* ]. ``original_id``

increases with *subset_id* ;
i.e., for each *subset_id* less than *n_subset* ``-2`` ,

| |tab| *subset_data_obj* [ *subset_id* ]. ``original_id <``
| |tab| |tab| *subset_data_obj* [ *subset_id* +1]. ``original_id``

data_sim_value
==============
There an extra field in ``subset_data_struct`` that has
name ``data_sim_value`` , type ``double`` .
All of these values are set to ``nan`` by this routine.
These values get replaced by simulated measurement values
when we are fitting simulated data.

subset_data_cov_value
*********************
The input size is zero and upon return
its size is *n_subset* * *n_covariate* .
For each *subset_id* and
:ref:`covariate_table@covariate_id` ,

| *subset_data_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ]
| = *data_cov_value* [ *original_id* * *n_covariate* + *covariate_id* ]
| |tab| ``- reference`` ( *covariate_id* )

where

   *original_id* = *subset_data_obj* [ *subset_id* ]. ``original_id``

and ``reference`` ( *covariate_id* ) is the
:ref:`covariate_table@reference` value for the
corresponding *covariate_id* .
Note that if the
:ref:`covariate_table@max_difference`
value is ``null`` in the covariate table,
or the covariate value is ``null`` in :ref:`data_table-name` ,

   *subset_data_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ] = 0

Also note that

| | *subset_data_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ] |
| <= ``max_difference`` ( *covariate_id* )

where ``max_difference`` ( *covariate_id* ) is the
maximum difference for the corresponding *covariate_id* .

Contents
********
{xrst_toc_table
   example/devel/utility/subset_data_xam.cpp
}
Example
*******
The file :ref:`subset_data_xam.cpp-name` contains
and example and test of ``data_subset`` .
It returns true for success and false for failure.

{xrst_end subset_data}
*/

# include <cmath>
# include <dismod_at/subset_data.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/split_space.hpp>
# include <dismod_at/get_str_map.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
void subset_data(
   const std::map<std::string, std::string>&    option_map            ,
   const CppAD::vector<data_subset_struct>&     data_subset_table     ,
   const CppAD::vector<integrand_struct>&       integrand_table       ,
   const CppAD::vector<density_enum>&           density_table         ,
   const CppAD::vector<data_struct>&            data_table            ,
   const CppAD::vector<double>&                 data_cov_value        ,
   const CppAD::vector<covariate_struct>&       covariate_table       ,
   const child_info&                            child_info4data       ,
   CppAD::vector<subset_data_struct>&           subset_data_obj       ,
   CppAD::vector<double>&                       subset_data_cov_value )
// END_PROTOTYPE
{  assert( subset_data_obj.size() == 0 );
   assert( subset_data_cov_value.size() == 0 );
   if( data_table.size() == 0 )
      return;
   //
   // sizes of const tables
   size_t n_child     = child_info4data.child_size();
   size_t n_data      = data_table.size();
   size_t n_covariate = covariate_table.size();
   size_t n_integrand = integrand_table.size();
   //
   // hold_out_integrand_list
   const std::string& hold_out_integrand = get_str_map(
      option_map, "hold_out_integrand", ""
   );
   CppAD::vector<std::string> hold_out_list = split_space(hold_out_integrand);
   //
   // age_size, time_size
   const std::string& compress_interval = get_str_map(
      option_map, "compress_interval", "0 0"
   );
   CppAD::vector<std::string> compress_interval_split = split_space(
      compress_interval
   );
   double age_size  = std::atof( compress_interval_split[0].c_str() );
   double time_size = std::atof( compress_interval_split[1].c_str() );
   //
   // hold_out_vec
   CppAD::vector<bool> hold_out_vec;
   size_t n_hold_out = hold_out_list.size();
   if( n_hold_out > 0 )
   {  hold_out_vec.resize(n_integrand);
      for(size_t integrand_id = 0; integrand_id < n_integrand; ++integrand_id)
      {  integrand_enum integrand = integrand_table[integrand_id].integrand;
         std::string    name      = integrand_enum2name[integrand];
         hold_out_vec[integrand_id] = false;
         for(size_t j = 0; j < n_hold_out; ++j)
            hold_out_vec[integrand_id] |= name == hold_out_list[j];
      }
   }
   //
   // n_subset
   // ok
   size_t n_subset = 0;
   CppAD::vector<bool> ok(n_data);
   for(size_t data_id = 0; data_id < n_data; data_id++)
   {  size_t child = child_info4data.table_id2child(data_id);
      if( child < n_child )
      {  int density_id = data_table[data_id].density_id;
         density_enum density = density_table[density_id];
         if( density == laplace_enum || density == log_laplace_enum )
         {  std::string message =
               "child data has laplace or log_lapace density";
            std::string table_name = "data";
            error_exit(message, table_name, data_id);
         }
      }
      // check if this data is for parent or one of its descendants
      ok[data_id] = child <= n_child;
      if( ok[data_id] )
      {  for(size_t j = 0; j < n_covariate; j++)
         {  size_t index          = data_id * n_covariate + j;
            double x_j            = data_cov_value[index];
            double reference      = covariate_table[j].reference;
            double max_difference = covariate_table[j].max_difference;
            double difference     = 0.0;
            if( ! std::isnan(x_j) )
               difference = x_j - reference;
            ok[data_id]  &= std::fabs( difference ) <= max_difference;
         }
      }
      if( ok[data_id] )
         n_subset++;
   }
   assert( n_subset == data_subset_table.size() );
   //
   // subset_data_obj
   // subset_data_cov_value
   subset_data_obj.resize(n_subset);
   subset_data_cov_value.resize(n_subset * n_covariate);
   size_t subset_id = 0;
   for(size_t data_id = 0; data_id < n_data; data_id++)
   {  if( ok[data_id] )
      {  subset_data_struct& one_sample( subset_data_obj[subset_id] );
         //
         for(size_t j = 0; j < n_covariate; j++)
         {  size_t index          = data_id * n_covariate + j;
            double x_j            = data_cov_value[index];
            double reference      = covariate_table[j].reference;
            double difference     = 0.0;
            if( ! std::isnan(x_j) )
               difference = x_j - reference;
            index = subset_id * n_covariate + j;
            subset_data_cov_value[index] = difference;
         }
         // values in avgint_subset_struct
         // except age_lower, age_upper, time_lower, time_upper
         assert( size_t( data_subset_table[subset_id].data_id ) == data_id );
         one_sample.original_id  = int( data_id );
         one_sample.integrand_id = data_table[data_id].integrand_id;
         one_sample.node_id      = data_table[data_id].node_id;
         one_sample.subgroup_id  = data_table[data_id].subgroup_id;
         one_sample.weight_id    = data_table[data_id].weight_id;
         // values in data_subset_table
         int density_id          = data_subset_table[subset_id].density_id;
         one_sample.density      = density_table[density_id];
         one_sample.eta          = data_subset_table[subset_id].eta;
         one_sample.nu           = data_subset_table[subset_id].nu;
         // values not in avgint_subset_struct except hold_out
         one_sample.meas_value   = data_table[data_id].meas_value;
         one_sample.meas_std     = data_table[data_id].meas_std;
         // value that depends on data_sim table
         one_sample.data_sim_value =
            std::numeric_limits<double>::quiet_NaN();
         //
         // integrand_id
         size_t integrand_id = data_table[data_id].integrand_id;
         //
         // hold_out
         int hold_out = data_table[data_id].hold_out;
         if( data_subset_table[subset_id].hold_out == 1 )
            hold_out = 1;
         if( hold_out_vec.size() != 0 )
         {  if( hold_out_vec[integrand_id] )
               hold_out = 1;
         }
         one_sample.hold_out = hold_out;
         //
         // age_lower, age_upper
         double age_lower = data_table[data_id].age_lower;
         double age_upper = data_table[data_id].age_upper;
         if( age_upper - age_lower <= age_size )
         {  double age_mid = (age_lower + age_upper) / 2.0;
            one_sample.age_lower = age_mid;
            one_sample.age_upper = age_mid;
         }
         else
         {  one_sample.age_lower = age_lower;
            one_sample.age_upper = age_upper;
         }
         //
         // time_lower, time_upper
         double time_lower = data_table[data_id].time_lower;
         double time_upper = data_table[data_id].time_upper;
         if( time_upper - time_lower <= time_size )
         {  double time_mid = (time_lower + time_upper) / 2.0;
            one_sample.time_lower = time_mid;
            one_sample.time_upper = time_mid;
         }
         else
         {  one_sample.time_lower = time_lower;
            one_sample.time_upper = time_upper;
         }
         //
         // advance to next sample
         subset_id++;
      }
   }
   return;
}

} // END DISMOD_AT_NAMESPACE
