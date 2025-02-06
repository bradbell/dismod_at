// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <gsl/gsl_randist.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/hold_out_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/balance_pair.hpp>
# include <dismod_at/null_int.hpp>
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
------------------------------------------------------------------------------
{xrst_begin hold_out_max_fit dev}

Sub-sampling Hold Outs to Keep max_fit Only Balancing Children
##############################################################

Prototype
*********
{xrst_literal ,
   // BEGIN_HOLD_OUT_MAX_FIT , // END_HOLD_OUT_MAX_FIT
   // BEGIN_RETURN_HOLD_OUT_MAX_FIT , // END_RETURN_HOLD_OUT_MAX_FIT
}

integrand_name
**************
This is the
:ref:`integrand<integrand_table@integrand_name>` that we are sub-sampling.

max_fit
*******
This is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.

max_fit_parent
**************
If this argument is DISMOD_AT_NULL_SIZE_T,
the value *max_fit* applies to the combination of the parent and child nodes.
Otherwise, *max_fit* applies to the total from all the child nodes and
*max_fit_parent* applies to the parent node.

covariate_id
************
is the covariate that will be balanced;
see :ref:`hold_out_command@Balancing` .

cov_value_1
***********
is one of the covariate values for the balancing.

cov_value_2
***********
is the opposite covariate value for balancing.

subset_hold_out
***************
is the value to use for the data_subset table hold_out column.
Only the rows for this integrand are different from the hold_out value
in data_subset_table.

{xrst_end hold_out_max_fit}
*/
// BEGIN_HOLD_OUT_MAX_FIT
CppAD::vector<int> hold_out_max_fit(
   integrand_enum                                this_integrand    ,
   size_t                                        max_fit           ,
   size_t                                        max_fit_parent    ,
   const child_info&                             child_info4data   ,
   const CppAD::vector<data_subset_struct>&      data_subset_table ,
   const CppAD::vector<integrand_struct>&        integrand_table   ,
   const CppAD::vector<covariate_struct>&        covariate_table   ,
   const CppAD::vector<data_struct>&             data_table        ,
   const CppAD::vector<double>&                  data_cov_value    )
{  // n_subset
   const size_t n_subset = data_subset_table.size();
   // END_HOLD_OUT_MAX_FIT
   //
   using std::string;
   using CppAD::vector;
   //
   // this_integrand_id
   int this_integrand_id = DISMOD_AT_NULL_INT;
   for(size_t i = 0; i < integrand_table.size(); ++i)
      if( integrand_table[i].integrand == this_integrand )
         this_integrand_id = int(i);
   //
   // rng
   // gsl random number generator
   gsl_rng* rng = CppAD::mixed::get_gsl_rng();
   //
   // subset_hold_out
   vector<int> subset_hold_out( n_subset );
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  size_t data_id             = data_subset_table[subset_id].data_id;
      int  integrand_id          = data_table[data_id].integrand_id;
      subset_hold_out[subset_id] = data_subset_table[subset_id].hold_out;
      if( integrand_id == this_integrand_id )
         subset_hold_out[subset_id] = 0;
   }
   //
   // n_child
   size_t n_child = child_info4data.child_size();
   //
   // avail
   vector< vector<size_t> > avail(n_child + 1);
   //
   // avail
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  //
      // data_id, child_id
      size_t data_id   = data_subset_table[subset_id].data_id;
      size_t child_id  = child_info4data.table_id2child(data_id);
      //
      // avail[child_id]
      int integrand_id         = data_table[data_id].integrand_id;
      int    data_hold_out     = data_table[data_id].hold_out;
      if( data_hold_out != 0 )
         assert( subset_hold_out[subset_id] == 0 );
      else
      {  if( integrand_id == this_integrand_id )
            avail[child_id].push_back( subset_id );
      }
   }
   //
   // subset_hold_out
   if( max_fit_parent != DISMOD_AT_NULL_SIZE_T )
   {  if( max_fit_parent < avail[n_child].size() )
      {  //
         // n_hold_out
         size_t n_hold_out = avail[n_child].size() - max_fit_parent;
         //
         // chosen: array of indices that are chosen
         vector<size_t> chosen(n_hold_out);
         //
         // chosen
         gsl_ran_choose(
            rng,
            chosen.data(),
            n_hold_out,
            avail[n_child].data(),
            avail[n_child].size(),
            sizeof(size_t)
         );
         //
         // subset_hold_out
         for(size_t i = 0; i < n_hold_out; ++i)
         {  size_t subset_id = chosen[i];
            assert( subset_hold_out[subset_id] == 0 );
            subset_hold_out[subset_id] = 1;
         }
      }
   }
   //
   // n_area
   size_t n_area = n_child + 1;
   if( max_fit_parent != DISMOD_AT_NULL_SIZE_T )
      n_area = n_child;
   //
   // avail_size
   vector<size_t> avail_size(n_area);
   for(size_t child_id = 0; child_id < n_area; ++child_id)
      avail_size[child_id] = avail[child_id].size();
   //
   // size_order
   vector<size_t> size_order(n_area);
   CppAD::index_sort(avail_size, size_order);
   //
   // n_fit
   size_t n_fit = 0;
   //
   for(size_t index = 0; index < n_area; ++index)
   {  //
      // child_id
      size_t child_id = size_order[index];
      //
      // n_left
      size_t n_left = n_area - index;
      //
      // this_max_fit
      // the maximum to choose for this child_id, cov_value_id
      size_t this_max_fit = (max_fit - n_fit) / n_left;
      if( this_max_fit * n_left < max_fit - n_fit )
         ++this_max_fit;
      if( this_max_fit + n_fit > max_fit )
         this_max_fit = max_fit - n_fit;
      //
      // n_fit, data_subset_table
      if( avail[child_id].size() <= this_max_fit )
      {  // include all the data correspnding to this child_id
         n_fit += avail[child_id].size();
      }
      else
      {  // include this_max_fit data corresponding to this child_id
         n_fit += this_max_fit;
         //
         // n_hold_out
         size_t n_hold_out = avail[child_id].size() - this_max_fit;
         //
         // chosen: array of indices that are chosen
         vector<size_t> chosen(n_hold_out);
         //
         // chosen
         gsl_ran_choose(
            rng,
            chosen.data(),
            n_hold_out,
            avail[child_id].data(),
            avail[child_id].size(),
            sizeof(size_t)
         );
         //
         // subset_hold_out
         for(size_t i = 0; i < n_hold_out; ++i)
         {  size_t subset_id = chosen[i];
            assert( subset_hold_out[subset_id] == 0 );
            subset_hold_out[subset_id] = 1;
         }
      }
   }
   // BEGIN_RETURN_HOLD_OUT_MAX_FIT
# ifndef NDEBUG
   assert( subset_hold_out.size() == n_subset );
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  int data_id              = data_subset_table[subset_id].data_id;
      int data_hold_out        = data_table[data_id].hold_out;
      assert( data_hold_out == 0 || subset_hold_out[subset_id] == 0 );
      int integrand_id         = data_table[data_id].integrand_id;
      if( integrand_id != this_integrand_id ) assert (
         data_subset_table[subset_id].hold_out == subset_hold_out[subset_id]
      );
   }
# endif
   return subset_hold_out;
   // END_RETURN_HOLD_OUT_MAX_FIT
}
/*
------------------------------------------------------------------------------
{xrst_begin hold_out_with_cov dev}

Sub-sampling Hold Outs To Keep Max Fit and Balance a Covariate
##############################################################

Prototype
*********
{xrst_literal ,
   // BEGIN_HOLD_OUT_WITH_COV , // END_HOLD_OUT_WITH_COV
   // BEGIN_RETURN_HOLD_OUT_WITH_COV , // END_RETURN_HOLD_OUT_WITH_COV
}

integrand_name
**************
This is the
:ref:`integrand<integrand_table@integrand_name>` that we are sub-sampling.

max_fit
*******
This is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.

covariate_id
************
is the covariate that will be balanced;
see :ref:`hold_out_command@Balancing` .

cov_value_1
***********
is one of the covariate values for the balancing.

cov_value_2
***********
is the opposite covariate value for balancing.

subset_hold_out
***************
is the value to use for the data_subset table hold_out column.
Only the rows for this integrand are different from the hold_out value
in data_subset_table.

{xrst_end hold_out_with_cov}
*/
// BEGIN_HOLD_OUT_WITH_COV
CppAD::vector<int> hold_out_with_cov(
   integrand_enum                                this_integrand    ,
   size_t                                        max_fit           ,
   const std::string&                            cov_name          ,
   double                                        cov_value_1       ,
   double                                        cov_value_2       ,
   const child_info&                             child_info4data   ,
   const CppAD::vector<data_subset_struct>&      data_subset_table ,
   const CppAD::vector<integrand_struct>&        integrand_table   ,
   const CppAD::vector<covariate_struct>&        covariate_table   ,
   const CppAD::vector<data_struct>&             data_table        ,
   const CppAD::vector<double>&                  data_cov_value    )
{   // n_subset
   const size_t n_subset = data_subset_table.size();
   // END_HOLD_OUT_WITH_COV
   //
   using std::string;
   using CppAD::vector;
   //
   // this_integrand_id
   int this_integrand_id = DISMOD_AT_NULL_INT;
   for(size_t i = 0; i < integrand_table.size(); ++i)
      if( integrand_table[i].integrand == this_integrand )
         this_integrand_id = int(i);
   //
   // rng
   // gsl random number generator
   gsl_rng* rng = CppAD::mixed::get_gsl_rng();
   //
   // subset_hold_out
   vector<int> subset_hold_out( n_subset );
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  size_t data_id             = data_subset_table[subset_id].data_id;
      int  integrand_id          = data_table[data_id].integrand_id;
      subset_hold_out[subset_id] = data_subset_table[subset_id].hold_out;
      if( integrand_id == this_integrand_id )
         subset_hold_out[subset_id] = 0;
   }
   //
   // n_covariate, covariate_id
   size_t n_covariate = covariate_table.size();
   size_t covariate_id = n_covariate;
   for(size_t i = 0; i < n_covariate; ++i)
      if( covariate_table[i].covariate_name == cov_name )
         covariate_id = i;
   if( covariate_id == n_covariate )
   {  string msg = "hold_out_command: " + cov_name;
      msg       += " is not a valid covariate name";
      error_exit(msg);
   }
   //
   // n_child
   size_t n_child = child_info4data.child_size();
   //
   // avail
   vector< vector< vector<size_t> > > avail(n_child + 1);
   for(size_t child_id = 0; child_id <= n_child; ++child_id)
      avail[child_id].resize(3);
   //
   // avail
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  //
      // data_id, child_id
      size_t data_id   = data_subset_table[subset_id].data_id;
      size_t child_id  = child_info4data.table_id2child(data_id);
      //
      // avail[child_id]
      int  integrand_id        = data_table[data_id].integrand_id;
      int    data_hold_out     = data_table[data_id].hold_out;
      if( data_hold_out != 0 )
         assert( data_subset_table[subset_id].hold_out == 0 );
      else
      {  if( integrand_id == this_integrand_id )
         {  size_t index     = data_id * n_covariate + covariate_id;
            double cov_value = data_cov_value[index];
            if( cov_value == cov_value_1 )
               avail[child_id][1].push_back( subset_id );
            else if( cov_value == cov_value_2 )
               avail[child_id][2].push_back( subset_id );
            else
               avail[child_id][0].push_back( subset_id );
         }
      }
   }
   //
   // n_index
   size_t n_index = 3 * (n_child + 1);
   //
   // avail_size
   vector<size_t> avail_size(n_index);
   for(size_t child_id = 0; child_id <= n_child; ++child_id)
   {  for(size_t cov_value_id = 0; cov_value_id < 3; ++cov_value_id)
      {  size_t index      = child_id * 3 + cov_value_id;
         avail_size[index] = avail[child_id][cov_value_id].size();
      }
   }
   //
   // size_order
   vector<size_t> size_order(n_index);
   CppAD::index_sort(avail_size, size_order);
   //
   // n_fit
   size_t n_fit = 0;
   //
   for(size_t order = 0; order < n_index; ++order)
   {  //
      // index
      size_t index = size_order[order];
      //
      // child_id, cov_value_id
      size_t cov_value_id = index % 3;
      size_t child_id     = index / 3;
      //
      // n_left
      size_t n_left = n_index - order;
      //
      // this_max_fit
      // the maximum to choose for this child_id, cov_value_id
      size_t this_max_fit = (max_fit - n_fit) / n_left;
      if( this_max_fit * n_left < max_fit - n_fit )
         ++this_max_fit;
      if( this_max_fit + n_fit > max_fit )
         this_max_fit = max_fit - n_fit;
      //
      // n_fit, data_subset_table
      if( avail_size[index] <= this_max_fit )
      {  // include all the data correspnding to this index
         n_fit += avail_size[index];
      }
      else
      {  // include this_max_fit data corresponding to this index
         n_fit += this_max_fit;
         //
         // n_hold_out
         size_t n_hold_out = avail_size[index] - this_max_fit;
         //
         // chosen: array of indices that are chosen
         vector<size_t> chosen(n_hold_out);
         //
         // chosen
         gsl_ran_choose(
            rng,
            chosen.data(),
            n_hold_out,
            avail[child_id][cov_value_id].data(),
            avail_size[index],
            sizeof(size_t)
         );
         //
         // subset_hold_out
         for(size_t i = 0; i < n_hold_out; ++i)
         {  size_t subset_id = chosen[i];
            assert( subset_hold_out[subset_id] == 0 );
            subset_hold_out[subset_id] = 1;
         }
      }
   }
   // BEGIN_RETURN_HOLD_OUT_WITH_COV
# ifndef NDEBUG
   assert( subset_hold_out.size() == n_subset );
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  int data_id              = data_subset_table[subset_id].data_id;
      int data_hold_out        = data_table[data_id].hold_out;
      assert( data_hold_out == 0 || subset_hold_out[subset_id] == 0 );
      int integrand_id         = data_table[data_id].integrand_id;
      if( integrand_id != this_integrand_id ) assert (
         data_subset_table[subset_id].hold_out == subset_hold_out[subset_id]
      );
   }
# endif
   return subset_hold_out;
   // END_RETURN_HOLD_OUT_WITH_COV
}
/*
-----------------------------------------------------------------------------
{xrst_begin hold_out_command}
{xrst_spell
  thoes
}

Hold Out Command: Randomly Sub-sample The Data
##############################################

Syntax
******

.. |first4|  replace:: ``dismod_at`` *database* ``hold_out`` *integrand_name*

| |first4| *max_fit*
| |first4| *max_fit* *max_fit_parent*
| |first4| *max_fit* *cov_name* *cov_value_1* *cov_value_2*

Purpose
*******
This command is used to set a maximum number of data values
that are included in subsequent fits.
It is intended to make the initialization and fitting faster.
The random choice of which values to include can be made repeatable using
:ref:`option_table@random_seed` .

database
********
Is an
`sqlite <https://www.sqlite.org/index.html>`_ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

integrand_name
**************
This is the
:ref:`integrand<integrand_table@integrand_name>` that we are sub-sampling.

max_fit
*******
If this argument is present,
it is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.
If for this integrand there are more than *max_fit* points with
:ref:`data_table@hold_out` zero in the data table,
points are randomly held out so that there are *max_fit*
points fit for this integrand.

max_fit_parent
**************
If this argument is present,
*max_fit* only applies to the total data from child nodes.
The value *max_fit_parent* determines the maximum number of
:ref:`option_table@Parent Node` data values to include.

cov_name
********
If this argument is present, it specifies a covariate column that
will be balanced; see covariate balancing below:

cov_value_1
***********
If this argument is present, it specifies one of the covariate values
for the balancing. This is a string representation of a ``double`` value.

cov_value_2
***********
If this argument is present, it specifies the opposite covariate value
for the balancing. This is a string representation of a ``double`` value.

Balancing
*********

Child Nodes
===========
The choice of which points to include in the fit tries to sample the
same number of data points from each of the child nodes (and the parent node).
If there are not sufficiently many data for one of these nodes, the others
make up the difference.

Covariates
==========
If *cov_name* is present, the data for each child is further split
into those with *cov_value_1*, those with *cov_value_2*,
and thoes with a different value (for the covariate specified by *cov_name* ).
The choice of which points to include tries to sample the same number
points form each of these sub-groups.


data_subset_table
*****************
Only rows of the :ref:`data_subset_table-name` that correspond to this integrand
are modified.
The :ref:`data_subset_table@hold_out` is set one (zero)
if the corresponding data is (is not) selected for hold out.
Only points that have *hold_out* zero in the data table
can have hold_out non-zero in the data_subset table.
See the fit command :ref:`fit_command@hold_out`
documentation.

Example
*******
The files :ref:`user_hold_out_1.py-name` and :ref:`user_hold_out_2.py-name`
contain examples and tests using this command.

{xrst_end hold_out_command}
*/
// ---------------------------------------------------------------------------
void hold_out_command(
   sqlite3*                                      db                ,
   const std::string&                            integrand_name    ,
   const std::string&                            max_fit_str       ,
   const std::string&                            argument_5        ,
   const std::string&                            cov_value_1_str   ,
   const std::string&                            cov_value_2_str   ,
   const child_info&                             child_info4data   ,
   const CppAD::vector<integrand_struct>&        integrand_table   ,
   const CppAD::vector<covariate_struct>&        covariate_table   ,
   const CppAD::vector<data_struct>&             data_table        ,
   const CppAD::vector<double>&                  data_cov_value    )
{  using std::string;
   using CppAD::vector;
   using CppAD::to_string;
   //
   // data_subset_table
   vector<data_subset_struct> data_subset_table = get_data_subset(db);
   //
   // n_subset
   size_t n_subset = data_subset_table.size();
   //
   // max_fit
   size_t max_fit = std::atoi( max_fit_str.c_str() );
   //
   // max_fit_parent
   size_t max_fit_parent = DISMOD_AT_NULL_SIZE_T;
   if( argument_5.size() != 0 && cov_value_1_str.size() == 0 )
      max_fit_parent = std::atoi( argument_5.c_str() );
   //
   // this_integrand
   integrand_enum this_integrand = number_integrand_enum;
   for(size_t i = 0; i < size_t(number_integrand_enum); ++i)
   {  if( integrand_enum2name[i] == integrand_name )
         this_integrand = integrand_enum(i);
   }
   if( this_integrand == number_integrand_enum )
   {  string msg = "hold_out_command: " + integrand_name;
      msg       += " is not a valid integrand name";
      error_exit(msg);
   }
   //
   // subset_hold_out
   vector<int> subset_hold_out;
   if( cov_value_1_str.size() == 0 )
   {  subset_hold_out = hold_out_max_fit(
         this_integrand    ,
         max_fit           ,
         max_fit_parent    ,
         child_info4data   ,
         data_subset_table ,
         integrand_table   ,
         covariate_table   ,
         data_table        ,
         data_cov_value
      );
   }
   else
   {
      double cov_value_1  = std::atof( cov_value_1_str.c_str() );
      double cov_value_2  = std::atof( cov_value_2_str.c_str() );
      subset_hold_out     = hold_out_with_cov(
         this_integrand    ,
         max_fit           ,
         argument_5        ,
         cov_value_1       ,
         cov_value_2       ,
         child_info4data   ,
         data_subset_table ,
         integrand_table   ,
         covariate_table   ,
         data_table        ,
         data_cov_value
      );
   }
   //
   // drop old data_subset table
   string sql_cmd    = "drop table data_subset";
   exec_sql_cmd(db, sql_cmd);
   //
   // write new data_subset table
   string table_name = "data_subset";
   size_t n_col      = 6;
   vector<string> col_name(n_col), col_type(n_col);
   vector<string> row_value(n_col * n_subset);
   vector<bool>   col_unique(n_col);
   //
   col_name[0]       = "data_id";
   col_type[0]       = "integer";
   col_unique[0]     = true;
   //
   col_name[1]       = "hold_out";
   col_type[1]       = "integer";
   col_unique[1]     = false;
   //
   col_name[2]       = "density_id";
   col_type[2]       = "integer";
   col_unique[2]     = false;
   //
   col_name[3]       = "eta";
   col_type[3]       = "real";
   col_unique[3]     = false;
   //
   col_name[4]       = "nu";
   col_type[4]       = "real";
   col_unique[4]     = false;
   //
   col_name[5]       = "sample_size";
   col_type[5]       = "integer";
   col_unique[5]     = false;
   //
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  int data_id     = data_subset_table[subset_id].data_id;
      int density_id  = data_subset_table[subset_id].density_id;
      double eta      = data_subset_table[subset_id].eta;
      double nu       = data_subset_table[subset_id].nu;
      int sample_size = data_subset_table[subset_id].sample_size;
      //
      int hold_out    = subset_hold_out[subset_id];
      //
      row_value[n_col * subset_id + 0] = to_string( data_id );
      row_value[n_col * subset_id + 1] = to_string( hold_out );
      row_value[n_col * subset_id + 2] = to_string( density_id );
      //
      if( std::isnan(eta) )
         row_value[n_col * subset_id + 3] = "";
      else
         row_value[n_col * subset_id + 3] = to_string( eta );
      //
      if( std::isnan(nu) )
         row_value[n_col * subset_id + 4] = "";
      else
         row_value[n_col * subset_id + 4] = to_string( nu );
      //
      if( sample_size == DISMOD_AT_NULL_INT )
         row_value[n_col * subset_id + 5] = "";
      else
         row_value[n_col * subset_id + 5] = to_string( sample_size );
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   return;
}
} // END_DISMOD_AT_NAMESPACE
