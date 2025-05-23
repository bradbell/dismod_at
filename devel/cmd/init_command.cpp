// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/init_command.hpp>
# include <dismod_at/set_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/get_data_subset.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin init_command}

The Initialize Command
######################

Syntax
******
``dismod_at`` *database* ``init``

Purpose
*******
This command initializes the data flow.
To be specific, it begins by deleting any existing output tables,
except for the log table,
and then creates new versions of the following tables:

.. csv-table::
   :widths: auto

   age_avg_table,:ref:`age_avg_table-title`
   bnd_mulcov_table,:ref:`bnd_mulcov_table-title`
   data_subset_table,:ref:`data_subset_table-title`
   start_var_table,:ref:`start_var_table-title`
   scale_var_table,:ref:`scale_var_table-title`
   var_table,:ref:`var_table-title`

Deleted Tables
**************
This routine begins by deleting any existing
:ref:`output tables<data_flow@Output Tables by Table Name>` ,
except for the :ref:`log_table-name` .

Changing Values
***************
You must re-run this command when ever any of the
:ref:`input-name` tables is changed.
The option table is an exception to this rule; see
:ref:`option_table@Changing Values` .

database
********
Is an
`sqlite <https://sqlite.org/index.html>`_ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

age_avg_table
*************
A new :ref:`age_avg_table-name` is created when this command is run.
In fact, this table is recreated when any command is run except for the
python commands and the :ref:`set_command-name` .

var_table
*********
A new :ref:`var_table-name` is created with the information
that maps a :ref:`var_table@var_id`
to its meaning in terms of the
:ref:`model_variables-name` .

data_subset_table
*****************
A new :ref:`data_subset_table-name` is created.
This makes explicit exactly which rows of the data table are used.

hold_out
========
This column is set to zero.

density_id
==========
This column is set to the corresponding
:ref:`data_table@density_id` in the data table.

eta
===
This column is set to the corresponding
:ref:`data_table@eta` in the data table.

nu
==
This column is set to the corresponding
:ref:`data_table@nu` in the data table.

start_var_table
***************
A new :ref:`start_var_table-name` is created using the
means of the priors for the model variables.

scale_var_table
***************
A new :ref:`scale_var_table-name` is created using the
means of the priors for the model variables.

bnd_mulcov_table
****************
A new bnd_mulcov table is created using null for
:ref:`bnd_mulcov_table@max_mulcov` ; i.e., plus infinity.
{xrst_toc_hidden
   example/get_started/init_command.py
}
Example
*******
The file :ref:`init_command.py-name` contains an example and test
using this command.

{xrst_end init_command}
*/

namespace { // BEGIN_EMPTY_NAMESPACE
CppAD::vector<data_subset_struct> make_data_subset_table(
   const child_info&                      child_info4data       ,
   const CppAD::vector<covariate_struct>& covariate_table       ,
   const CppAD::vector<data_struct>&      data_table            ,
   const CppAD::vector<double>&           data_cov_value        )
{
   // n_data
   size_t n_data = data_table.size();
   //
   // n_child
   size_t n_child = child_info4data.child_size();
   //
   // n_covariate
   size_t n_covariate = covariate_table.size();
   //
   // data_subset_table
   CppAD::vector<data_subset_struct> data_subset_table;
   for(size_t data_id = 0; data_id < n_data; data_id++)
   {  size_t child = child_info4data.table_id2child(data_id);
      //
      // check if this data is for parent or one of its descendants
      bool in_subset = child <= n_child;
      if( in_subset )
      {  for(size_t j = 0; j < n_covariate; j++)
         {  size_t index          = data_id * n_covariate + j;
            double x_j            = data_cov_value[index];
            double reference      = covariate_table[j].reference;
            double max_difference = covariate_table[j].max_difference;
            double difference     = 0.0;
            if( ! std::isnan(x_j) )
               difference = x_j - reference;
            in_subset &= std::fabs(difference) <= max_difference;
         }
      }
      if( in_subset )
      {  data_subset_struct row;
         row.data_id     = int( data_id );
         row.hold_out    = 0;
         row.density_id  = data_table[data_id].density_id;
         row.eta         = data_table[data_id].eta;
         row.nu          = data_table[data_id].nu;
         row.sample_size = data_table[data_id].sample_size;
         data_subset_table.push_back(row);
      }
   }
   return data_subset_table;
}

} // END_EMPTY_NAMESPACE

// ----------------------------------------------------------------------------
void init_command(
   sqlite3*                                         db                  ,
   const CppAD::vector<double>&                     prior_mean          ,
   const pack_info&                                 pack_object         ,
   const db_input_struct&                           db_input            ,
   const size_t&                                    parent_node_id      ,
   const child_info&                                child_info4data     ,
   const CppAD::vector<smooth_info>&                s_info_vec          )
{  using std::string;
   using CppAD::to_string;
   using CppAD::vector;

   // -----------------------------------------------------------------------
   // Should be same as list of Output Tables by Table Name in data_flow.omh
   // except for that age_avg and log tables are not included.
   // BEGIN_SORT_THIS_LINE_PLUS_2
   const char* drop_list[] = {
      "bnd_mulcov",
      "data_sim",
      "data_subset",
      "depend_var",
      "fit_data_subset",
      "fit_var",
      "hes_fixed",
      "hes_random",
      "ipopt_info",
      "mixed_info",
      "predict",
      "prior_sim",
      "sample",
      "scale_var",
      "start_var",
      "trace_fixed",
      "truth_var",
      "var",
   };
   // END_SORT_THIS_LINE_MINUS_2
   size_t n_drop = sizeof( drop_list ) / sizeof( drop_list[0] );
   string sql_cmd;
   for(size_t i = 0; i < n_drop; i++)
   {  sql_cmd = "drop table if exists ";
      sql_cmd += drop_list[i];
      exec_sql_cmd(db, sql_cmd);
   }
   // -----------------------------------------------------------------------
   // start_var table
   string table_out    = "start_var";
   string source       = "prior_mean";
   string sample_index = "";
   set_command(table_out, source, sample_index, db, prior_mean);
   // -----------------------------------------------------------------------
   // scale_var table
   table_out    = "scale_var";
   source       = "prior_mean";
   sample_index = "";
   set_command(table_out, source, sample_index, db, prior_mean);
   //
   // declare some variables used for all tables
   string table_name;
   vector<string> col_name, col_type;
   vector<string> row_value;
   vector<bool>   col_unique;
   size_t n_col;
   // -----------------------------------------------------------------------
   // data_subset table
   table_name = "data_subset";
   vector<data_subset_struct> data_subset_table = make_data_subset_table(
      child_info4data,
      db_input.covariate_table,
      db_input.data_table,
      db_input.data_cov_value
   );
   size_t n_subset   = data_subset_table.size();
   n_col             = 6;
   col_name.resize(n_col);
   col_type.resize(n_col);
   row_value.resize(n_col * n_subset);
   col_unique.resize(n_col);
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
   // -----------------------------------------------------------------------
   // bnd_mulcov_table
   //
   // max_abs_diff
   size_t n_integrand = db_input.integrand_table.size();
   size_t n_covariate = db_input.covariate_table.size();
   vector<double> max_abs_diff(n_integrand * n_covariate);
   for(size_t i = 0; i < n_integrand * n_covariate; ++i)
      max_abs_diff[i] = 0.0;
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  int data_id         = data_subset_table[subset_id].data_id;
      int integrand_id    = db_input.data_table[data_id].integrand_id;
      for(size_t id = 0; id < n_covariate; ++id)
      {
         size_t index        = size_t(data_id) * n_covariate + id;
         double cov_value    = db_input.data_cov_value[index];
         if( ! std::isnan( cov_value ) )
         {  double reference    = db_input.covariate_table[id].reference;
            double abs_diff     = std::fabs(cov_value - reference);
            //
            index               = size_t(integrand_id) * n_covariate + id;
            max_abs_diff[index] = std::max( abs_diff, max_abs_diff[index] );
         }
      }
   }
   //
   table_name        = "bnd_mulcov";
   size_t n_mulcov   = db_input.mulcov_table.size();
   n_col             = 2;
   col_name.resize(n_col);
   col_type.resize(n_col);
   row_value.resize(n_col * n_mulcov);
   col_unique.resize(n_col);
   //
   col_name[0]     = "max_mulcov";
   col_type[0]     = "real";
   col_unique[0]   = false;
   //
   col_name[1]     = "max_cov_diff";
   col_type[1]     = "real";
   col_unique[1]   = false;
   //
   for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
   {  int covariate_id    = db_input.mulcov_table[mulcov_id].covariate_id;
      int integrand_id    = db_input.mulcov_table[mulcov_id].integrand_id;
# ifndef NDEBUG
         mulcov_type_enum mulcov_type =
            db_input.mulcov_table[mulcov_id].mulcov_type;
      if( integrand_id == DISMOD_AT_NULL_INT )
         assert( mulcov_type == rate_value_enum );
      else
         assert( mulcov_type != rate_value_enum );
# endif
      double max_cov_diff = 0.0;
      if( integrand_id != DISMOD_AT_NULL_INT )
      {  size_t index = integrand_id * n_covariate + covariate_id;
         max_cov_diff = max_abs_diff[index];
      }
      else
      {  for(size_t id = 0; id < n_integrand; ++id)
         {  size_t index = id * n_covariate + size_t(covariate_id);
            max_cov_diff = std::max( max_abs_diff[index], max_cov_diff);
         }
      }
      // max_mulcov
      // is a bound so null corresponds to infinity
      row_value[n_col * mulcov_id + 0] = "";
      // max_cov_diff
      row_value[n_col * mulcov_id + 1] = to_string(max_cov_diff);
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   // -----------------------------------------------------------------------
   // create var table
   size_t n_row = pack_object.size();
   n_col = 11;
   table_name   = "var";
   col_name.resize(n_col);
   col_type.resize(n_col);
   col_unique.resize(n_col);
   row_value.clear();
   row_value.resize(n_col * n_row);
   //
   col_name[0]   = "var_type";
   col_type[0]   = "text";
   col_unique[0] = false;
   //
   col_name[1]   = "smooth_id";
   col_type[1]   = "integer";
   col_unique[1] = false;
   //
   col_name[2]   = "age_id";
   col_type[2]   = "integer";
   col_unique[2] = false;
   //
   col_name[3]   = "time_id";
   col_type[3]   = "integer";
   col_unique[3] = false;
   //
   col_name[4]   = "node_id";
   col_type[4]   = "integer";
   col_unique[4] = false;
   //
   col_name[5]   = "rate_id";
   col_type[5]   = "integer";
   col_unique[5] = false;
   //
   col_name[6]   = "integrand_id";
   col_type[6]   = "integer";
   col_unique[6] = false;
   //
   col_name[7]   = "covariate_id";
   col_type[7]   = "integer";
   col_unique[7] = false;
   //
   col_name[8]   = "mulcov_id";
   col_type[8]   = "integer";
   col_unique[8] = false;
   //
   col_name[9]   = "group_id";
   col_type[9]   = "integer";
   col_unique[9] = false;
   //
   col_name[10]   = "subgroup_id";
   col_type[10]   = "integer";
   col_unique[10] = false;
   //
   // mulstd variables
   size_t n_smooth = db_input.smooth_table.size();
   size_t offset = DISMOD_AT_NULL_SIZE_T;
   size_t var_id = DISMOD_AT_NULL_SIZE_T;
   for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
   {  for(size_t k = 0; k < 3; k++)
      {  offset  = pack_object.mulstd_offset(smooth_id, k);
         if( offset != DISMOD_AT_NULL_SIZE_T )
         {  var_id  = offset;
# ifndef NDEBUG
            for(size_t j = 0; j < n_col; j++)
               assert( row_value[ n_col * var_id + j ] == "" );
# endif
            // var_type
            string var_type;
            switch(k)
            {  case 0:
               var_type = "mulstd_value";
               break;

               case 1:
               var_type = "mulstd_dage";
               break;

               case 2:
               var_type = "mulstd_dtime";
               break;

               default:
               assert(false);
            }
            row_value[ n_col * var_id + 0] = var_type;

            // smooth_id
            row_value[ n_col * var_id + 1] = to_string( smooth_id );
         }
      }
   }
   // rate variables
   size_t n_rate  = db_input.rate_table.size();
   size_t n_child = child_info4data.child_size();
   size_t n_var, n_time, node_id;
   for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
   {  for(size_t child_id = 0; child_id <= n_child; child_id++)
      {  if( child_id == n_child )
            node_id = parent_node_id;
         else
            node_id = child_info4data.child_id2node_id(child_id);
         //
         pack_info::subvec_info info;
         info      = pack_object.node_rate_value_info(rate_id, child_id);
         size_t smooth_id = info.smooth_id;
         if( smooth_id == DISMOD_AT_NULL_SIZE_T )
            n_var = 0;
         else
         {  offset    = info.offset;
            n_var     = info.n_var;
            n_time    = db_input.smooth_table[smooth_id].n_time;
# ifndef NDEBUG
            size_t n_age = db_input.smooth_table[smooth_id].n_age;
            assert( n_var == n_age * n_time );
# endif
         }
         for(size_t index = 0; index < n_var; index++)
         {  size_t i       = index / n_time;
            size_t j       = index % n_time;
            var_id         = offset + index;
            size_t age_id  = s_info_vec[smooth_id].age_id(i);
            size_t time_id = s_info_vec[smooth_id].time_id(j);
            //
            // fit_var_value
# ifndef NDEBUG
            for(size_t k = 0; k < n_col; k++)
               assert( row_value[ n_col * var_id + k ] == "" );
# endif
            row_value[n_col * var_id + 0] = "rate"; // var_type
            row_value[n_col * var_id + 1] = to_string( smooth_id );
            row_value[n_col * var_id + 2] = to_string( age_id );
            row_value[n_col * var_id + 3] = to_string( time_id );
            row_value[n_col * var_id + 4] = to_string( node_id );
            row_value[n_col * var_id + 5] = to_string( rate_id );
         }
      }
   }
   //
   // mulcov_table
   const vector<mulcov_struct>& mulcov_table( db_input.mulcov_table );
   n_mulcov               = mulcov_table.size();
   assert( n_integrand  == db_input.integrand_table.size() );
   //
   // initialize counters for different types of covariate multipliers
   vector<size_t> count_group_meas_value(n_integrand);
   vector<size_t> count_group_meas_noise(n_integrand);
   vector<size_t> count_subgroup_meas_value(n_integrand);
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   {  count_group_meas_value[integrand_id]    = 0;
      count_group_meas_noise[integrand_id]    = 0;
      count_subgroup_meas_value[integrand_id] = 0;
   }
   vector<size_t> count_group_rate_value(n_rate);
   vector<size_t> count_subgroup_rate_value(n_rate);
   for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
   {  count_group_rate_value[rate_id]    = 0;
      count_subgroup_rate_value[rate_id] = 0;
   }
   //
   // group covariate multipliers
   vector<size_t> smooth_id_loop(2), n_subgroup(2);
   for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
   {  mulcov_type_enum mulcov_type;
      mulcov_type              = mulcov_table[mulcov_id].mulcov_type;
      size_t rate_id           = mulcov_table[mulcov_id].rate_id;
      size_t integrand_id      = mulcov_table[mulcov_id].integrand_id;
      size_t covariate_id      = mulcov_table[mulcov_id].covariate_id;
      size_t group_id          = mulcov_table[mulcov_id].group_id;
      size_t first_subgroup_id = pack_object.first_subgroup_id(group_id);
      //
      smooth_id_loop[0]        = mulcov_table[mulcov_id].group_smooth_id;
      smooth_id_loop[1]        = mulcov_table[mulcov_id].subgroup_smooth_id;
      n_subgroup[0]            = 1;
      n_subgroup[1]            = pack_object.subgroup_size(group_id);
      //
      for(size_t loop = 0; loop < 2; ++loop)
      for(size_t k = 0; k < n_subgroup[loop]; ++k)
      if( smooth_id_loop[loop] != DISMOD_AT_NULL_SIZE_T )
      {  size_t smooth_id = smooth_id_loop[loop];
         pack_info::subvec_info info;
         switch( mulcov_type )
         {
            case rate_value_enum:
            if( loop == 0 )
            {  info = pack_object.group_rate_value_info(
                  rate_id, count_group_rate_value[rate_id]++
               );
            }
            else
            {  info = pack_object.subgroup_rate_value_info(
                  rate_id, count_subgroup_rate_value[rate_id], k
               );
               if( k == n_subgroup[loop] - 1 )
                  ++count_subgroup_rate_value[rate_id];
            }
            break;

            case meas_value_enum:
            if( loop == 0 )
            {  info = pack_object.group_meas_value_info(
                  integrand_id, count_group_meas_value[integrand_id]++
               );
            }
            else
            {  info = pack_object.subgroup_meas_value_info(
                  integrand_id,
                  count_subgroup_meas_value[integrand_id],
                  k
               );
               if( k == n_subgroup[loop] - 1 )
                  ++count_subgroup_meas_value[integrand_id];
            }
            break;

            case meas_noise_enum:
            assert(loop == 0 );
            info = pack_object.group_meas_noise_info(
               integrand_id, count_group_meas_noise[integrand_id]++
            );
            break;

            default:
            assert(false);
         }
         //
         assert( smooth_id == info.smooth_id);
         offset    = info.offset;
         n_var     = info.n_var;
         n_time    = db_input.smooth_table[smooth_id].n_time;
   # ifndef NDEBUG
         size_t n_age = db_input.smooth_table[smooth_id].n_age;
         assert( n_var == n_age * n_time );
   # endif
         for(size_t index = 0; index < n_var; index++)
         {  size_t i        = index / n_time;
            size_t j        = index % n_time;
            var_id          = offset + index;
            size_t age_id   = s_info_vec[smooth_id].age_id(i);
            size_t time_id  = s_info_vec[smooth_id].time_id(j);
   # ifndef NDEBUG
            for(size_t ell = 0; ell < n_col; ++ell)
               assert( row_value[ n_col * var_id + ell ] == "" );
   # endif
            if( mulcov_type == rate_value_enum )
            {  row_value[n_col * var_id + 0]  = "mulcov_rate_value";
               row_value[n_col * var_id + 5] = to_string( rate_id );
            }
            else if( mulcov_type == meas_value_enum )
            {  row_value[n_col * var_id + 0]  = "mulcov_meas_value";
               row_value[n_col * var_id + 6] = to_string( integrand_id );
            }
            else if( mulcov_type == meas_noise_enum )
            {  row_value[n_col * var_id + 0]  = "mulcov_meas_noise";
               row_value[n_col * var_id + 6] = to_string( integrand_id );
            }
            else assert(false);
            //
            row_value[n_col * var_id + 1] = to_string( smooth_id );
            row_value[n_col * var_id + 2] = to_string( age_id );
            row_value[n_col * var_id + 3] = to_string( time_id );
            row_value[n_col * var_id + 7] = to_string( covariate_id );
            row_value[n_col * var_id + 8] = to_string( mulcov_id );
            if( loop == 0 )
               row_value[n_col * var_id + 9] = to_string( group_id );
            else
            {  size_t subgroup_id = first_subgroup_id + k;
               row_value[n_col * var_id + 10] = to_string( subgroup_id );
            }
         }
      }
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   // ----------------------------------------------------------------------
   return;
}

} // END_DISMOD_AT_NAMESPACE
