// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
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

| ``dismod_at`` *database* ``hold_out`` *integrand_name* *max_fit*
| ``dismod_at`` *database* ``hold_out`` *integrand_name* *max_fit* *cov_name* *cov_value_1* *cov_value_2*

Purpose
*******
This command is used to set a maximum number of data values
that are included in subsequent fits.
It is intended to make the initialization and fitting faster.
The random sample of which values to include can be made repeatable using
:ref:`option_table@random_seed` .

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

integrand_name
**************
This is the
:ref:`integrand<integrand_table@integrand_name>` that we are sub-sampling.

max_fit
*******
This is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.
If for this integrand there are more than *mas_fit* points with
:ref:`data_table@hold_out` zero in the data table,
points are randomly held out so that there are *max_fit*
points fit for this integrand.

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
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
// ---------------------------------------------------------------------------
void hold_out_command(
   sqlite3*                                      db                ,
   const std::string&                            integrand_name    ,
   const std::string&                            max_fit_str       ,
   const std::string&                            cov_name          ,
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
   // rng
   // gsl random number generator
   gsl_rng* rng = CppAD::mixed::get_gsl_rng();
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
   // n_covariate, covariate_id
   size_t n_covariate = covariate_table.size();
   size_t covariate_id = n_covariate;
   for(size_t i = 0; i < n_covariate; ++i)
      if( covariate_table[i].covariate_name == cov_name )
         covariate_id = i;
   if( covariate_id == n_covariate && 0 < cov_name.size() )
   {  string msg = "hold_out_command: " + cov_name;
      msg       += " is not a valid covariate name";
      error_exit(msg);
   }
   //
   // cov_value_1, cov_value_2
   double cov_value_1 = std::numeric_limits<double>::quiet_NaN();
   double cov_value_2 = std::numeric_limits<double>::quiet_NaN();
   if( 0 < cov_name.size() )
   {  cov_value_1 = std::atof( cov_value_1_str.c_str() );
      cov_value_2 = std::atof( cov_value_2_str.c_str() );
   }
   //
   // n_child
   size_t n_child = child_info4data.child_size();
   //
   // if cov_name.size() == 0
   // avail[child_id][0] is the available subset indices for this child_id
   //
   // if cov_name.size() > 0
   // avail[child_id][1] is the available indices for this child_id
   //                    with covariate value cov_value_1
   // avail[child_id][2] is the available indices for this child_id
   //                    with covariate value cov_value_2
   // avail[child_id][0] is the available indices for this child_id
   //                    with covariate value not cov_value_2 or cov_value_2
   vector< vector< vector<size_t> > > avail(n_child + 1);
   for(size_t child_id = 0; child_id <= n_child; ++child_id)
      avail[child_id].resize(3);
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  //
      // data_id, child_id
      size_t data_id           = data_subset_table[subset_id].data_id;
      size_t child_id          = child_info4data.table_id2child(data_id);
      //
      // data_subset_table[subset_id].hold_out
      // avail[child_id]
      size_t integrand_id      = data_table[data_id].integrand_id;
      int    hold_out          = data_table[data_id].hold_out;
      integrand_enum integrand = integrand_table[integrand_id].integrand;
      if( integrand == this_integrand )
      {  if( hold_out != 0 )
         {  assert( data_subset_table[subset_id].hold_out == 0 );
         }
         else
         {  data_subset_table[subset_id].hold_out = 0;
            if( cov_name.size() == 0 )
               avail[child_id][0].push_back( subset_id );
            else
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
   for(size_t k = 0; k < n_index; ++k)
   {  //
      // index
      size_t index = size_order[k];
      //
      // child_id, cov_value_id
      size_t cov_value_id = index % 3;
      size_t child_id     = index / 3;
      //
      // n_left
      size_t n_left = n_index - k;
      //
      // this_max_fit
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
         // choose which elements to hold out
         gsl_ran_choose(
            rng,
            chosen.data(),
            n_hold_out,
            avail[child_id][cov_value_id].data(),
            avail_size[index],
            sizeof(size_t)
         );
         //
         // data_subset_table[subset_id].hold_out
         for(size_t i = 0; i < n_hold_out; ++i)
         {  size_t subset_id = chosen[i];
            assert( data_subset_table[subset_id].hold_out == 0 );
            data_subset_table[subset_id].hold_out = 1;
         }
      }
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
      int hold_out    = data_subset_table[subset_id].hold_out;
      int density_id  = data_subset_table[subset_id].density_id;
      double eta      = data_subset_table[subset_id].eta;
      double nu       = data_subset_table[subset_id].nu;
      int sample_size = data_subset_table[subset_id].sample_size;
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
