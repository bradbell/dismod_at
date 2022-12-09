// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin avgint_subset dev}
{xrst_spell
   subsampled
   subsamples
}

Create a Subsampled Version of Average Integrand Case Table
###########################################################

Syntax
******

| ``avgint_subset`` (
| |tab| *avgint_table* , *avgint_cov_value* , *covariate_table* , *child_info4avgint* ,
| |tab| ``avgint_subset_obj`` , *avgint_subset_cov_value*
| )

See Also
********
:ref:`subset_data-name`

Purpose
*******
This routine subsamples the *avgint_table* , in the following way:

#. Only rows corresponding to the
   :ref:`option_table@Parent Node` ,
   or a descendant of the parent node, are included.
#. Only rows for which the covariates satisfy the
   :ref:`covariate_table@max_difference` criteria
   are included.
#. The subsampled rows are the same as the corresponding original row
   except that for each covariate, its
   :ref:`covariate_table@reference` value is subtracted
   from the value of the covariate in *avgint_table* .

avgint_table
************
This argument has prototype

   ``const CppAD::vector<avgint_struct>&`` *avgint_table*

and is the :ref:`get_avgint_table@avgint_table` .

avgint_cov_value
****************
This argument has prototype

   ``const CppAD::vector<double>&`` *avgint_cov_value*

and is the :ref:`avgint_table<get_avgint_table@avgint_cov_value>`
covariate values.

covariate_table
***************
This argument has prototype

   ``const CppAD::vector<covariate_struct>&`` *covariate_table*

and is the :ref:`get_covariate_table@covariate_table` .

child_info4avgint
*****************
This argument has prototype

   ``const child_info&`` *child_info4avgint*

avgint_subset_obj
*****************
This argument has prototype

   ``CppAD::vector<avgint_subset_struct>&`` *avgint_subset_obj*

Its input size is zero and upon return
its size is the number of rows in *avgint_table* that satisfy
the purpose above.
The structure has all the fields that are present in
:ref:`get_avgint_table@avgint_struct` .

n_subset
========
We use the notation *n_subset* = *avgint_subset_obj* . ``size`` () .

subset_id
=========
We use the notation *subset_id* for an index between
zero and *n_subset* ``-1`` ,

original_id
===========
There an extra field in ``avgint_struct`` that has
name ``original_id`` , type ``int`` .
The values in this field are equal to the
*avgint_id* for the corresponding row of :ref:`avgint_table-name` .
The value of

   *avgint_subset_obj* [ *subset_id* ]. ``original_id``

increases with *subset_id* ;
i.e., for each *subset_id* less than *n_subset* ``-2`` ,

| |tab| *avgint_subset_obj* [ *subset_id* ]. ``original_id <``
| |tab| |tab| *avgint_subset_obj* [ *subset_id* +1]. ``original_id``

avgint_subset_cov_value
***********************
This argument has prototype

   ``CppAD::vector<double>&`` *avgint_subset_cov_value*

Its input size is zero and upon return
its size is *n_subset* * *n_covariate* .
For each *subset_id* and
:ref:`covariate_table@covariate_id` ,

| *avgint_subset_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ]
| = *avgint_cov_value* [ *original_id* * *n_covariate* + *covariate_id* ]
| |tab| ``- reference`` ( *covariate_id* )

where

   *original_id* = *avgint_subset_obj* [ *subset_id* ]. ``original_id``

and ``reference`` ( *covariate_id* ) is the
:ref:`covariate_table@reference` value for the
corresponding *covariate_id* .
Note that if the
:ref:`covariate_table@max_difference`
value is ``null`` in the covariate table,
or the covariate value is ``null`` in :ref:`avgint_table-name` ,

   *avgint_subset_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ] = 0

Also note that

| | *avgint_subset_cov_value* [ *subset_id* * *n_covariate* + *covariate_id* ] |
| <= ``max_difference`` ( *covariate_id* )

where ``max_difference`` ( *covariate_id* ) is the
maximum difference for the corresponding *covariate_id* .

Contents
********
{xrst_toc_table
   example/devel/utility/avgint_subset_xam.cpp
}
Example
*******
The file :ref:`avgint_subset_xam.cpp-name` contains
and example and test of ``avgint_subset`` .
It returns true for success and false for failure.

{xrst_end avgint_subset}
*/

# include <cmath>
# include <dismod_at/avgint_subset.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void avgint_subset(
   const CppAD::vector<integrand_struct>& integrand_table         ,
   const CppAD::vector<avgint_struct>&    avgint_table            ,
   const CppAD::vector<double>&           avgint_cov_value        ,
   const CppAD::vector<covariate_struct>& covariate_table         ,
   const child_info&                      child_info4avgint         ,
   CppAD::vector<avgint_subset_struct>&   avgint_subset_obj       ,
   CppAD::vector<double>&                 avgint_subset_cov_value )
{  assert( avgint_subset_obj.size() == 0 );
   assert( avgint_subset_cov_value.size() == 0 );
   if( avgint_table.size() == 0 )
      return;
   //
   size_t n_child     = child_info4avgint.child_size();
   size_t n_avgint    = avgint_table.size();
   size_t n_covariate = covariate_table.size();
   //
   size_t n_subset = 0;
   CppAD::vector<bool> ok(n_avgint);
   for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
   {   int integrand_id         = avgint_table[avgint_id].integrand_id;
      integrand_enum integrand = integrand_table[integrand_id].integrand;
      size_t child = child_info4avgint.table_id2child(avgint_id);
      // check if this avgint is for parent or one of its descendants
      ok[avgint_id] = (child <= n_child) || (integrand == mulcov_enum);
      if( ok[avgint_id] )
      {  for(size_t j = 0; j < n_covariate; j++)
         {  size_t index          = avgint_id * n_covariate + j;
            double x_j            = avgint_cov_value[index];
            double reference      = covariate_table[j].reference;
            double max_difference = covariate_table[j].max_difference;
            double difference     = 0.0;
            if( ! std::isnan(x_j) )
               difference = x_j - reference;
            ok[avgint_id]  &= std::fabs( difference ) <= max_difference;
         }
      }
      if( ok[avgint_id] )
         n_subset++;
   }
   //
   avgint_subset_obj.resize(n_subset);
   avgint_subset_cov_value.resize(n_subset * n_covariate);
   size_t subset_id = 0;
   for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
   {  if( ok[avgint_id] )
      {  avgint_subset_struct& one_sample( avgint_subset_obj[subset_id] );
         //
         for(size_t j = 0; j < n_covariate; j++)
         {  size_t index          = avgint_id * n_covariate + j;
            double x_j            = avgint_cov_value[index];
            double reference      = covariate_table[j].reference;
            double difference     = 0.0;
            if( ! std::isnan(x_j) )
               difference = x_j - reference;
            index = subset_id * n_covariate + j;
            avgint_subset_cov_value[index] = difference;
         }
         //
         one_sample.original_id  = int( avgint_id );
         one_sample.integrand_id = avgint_table[avgint_id].integrand_id;
         one_sample.node_id      = avgint_table[avgint_id].node_id;
         one_sample.subgroup_id  = avgint_table[avgint_id].subgroup_id;
         one_sample.weight_id    = avgint_table[avgint_id].weight_id;
         one_sample.age_lower    = avgint_table[avgint_id].age_lower;
         one_sample.age_upper    = avgint_table[avgint_id].age_upper;
         one_sample.time_lower   = avgint_table[avgint_id].time_lower;
         one_sample.time_upper   = avgint_table[avgint_id].time_upper;
         //
         // advance to next sample
         subset_id++;
      }
   }
   return;
}
} // END DISMOD_AT_NAMESPACE
