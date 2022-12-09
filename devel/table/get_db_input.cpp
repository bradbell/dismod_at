// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_db_input}
{xrst_spell
   preform
}

C++: Get the Data Base Input Tables
###################################

Syntax
******
``get_db_input`` ( *db* , *db_input* )

See Also
********
See the ``smooth_info`` constructor
:ref:`smooth_info_ctor@Checks` .

Purpose
*******
Read all the input tables and return them as a C++ data structure.
In addition, preform the following checks:

Primary Key
===========
Check that all occurrences of *table_name* _ ``id`` are with in
the limit for the corresponding table.
Note that this only checks limits, and not positional dependent limits.
For example, ``null`` might appear anywhere in
:ref:`smooth_grid_table@dage_prior_id` .

Initial Prevalence Grid
=======================
See :ref:`check_pini_n_age-name` .

Check Rate Limits
=================
See :ref:`check_rate_limit-name` .

Child Priors
============
See :ref:`check_child_prior-name` .

Check Zero Sum Constraint
=========================
See :ref:`check_zero_sum-name` .

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

db_input
********
The return value has prototype

   ``db_input_struct&`` *db_input*

where ``db_input_struct`` is defined by
{xrst_literal
   include/dismod_at/get_db_input.hpp
   // BEGIN STRUCT
   // END STRUCT
}

Each *name* _ ``table`` above is defined by the corresponding
``get_`` *name* _ ``table`` routine.
For example, ``age_table`` is the return value of
:ref:`get_age_table-name` routine.
All of the tables must be empty when ``get_db_input`` is called; i.e.,
the size of the corresponding vector must be zero.
Upon return, each table will have the corresponding database *db*
information.

{xrst_end get_db_input}
-----------------------------------------------------------------------------
*/
# include <limits>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/check_pini_n_age.hpp>
# include <dismod_at/check_rate_limit.hpp>
# include <dismod_at/check_child_prior.hpp>
# include <dismod_at/check_child_nslist.hpp>
# include <dismod_at/check_zero_sum.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/error_exit.hpp>

# define DISMOD_AT_CHECK_PRIMARY_ID(in_table, in_name, primary_table)\
for(size_t row_id = 0; row_id < db_input.in_table ## _table.size(); row_id++) \
{  int id_value = db_input.in_table ## _table[row_id].in_name; \
   int upper = int( db_input.primary_table ## _table.size() ) - 1; \
   bool ok   = 0 <= id_value && id_value <= upper; \
   ok       |= id_value == DISMOD_AT_NULL_INT; \
   if( ! ok ) \
   {  table_name = #in_table; \
      message    = #in_name " = "; \
      message   += to_string( id_value ) + " does not appear as "; \
      message   += #primary_table "_id in " #primary_table " table"; \
      error_exit(message, table_name, row_id); \
   } \
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_db_input(sqlite3* db, db_input_struct& db_input)
{  using CppAD::vector;
   using CppAD::to_string;
   //
   assert( db_input.age_table.size() == 0 );
   assert( db_input.time_table.size() == 0 );
   assert( db_input.rate_table.size() == 0 );
   assert( db_input.density_table.size() == 0 );
   assert( db_input.weight_table.size() == 0 );
   assert( db_input.smooth_table.size() == 0 );
   assert( db_input.covariate_table.size() == 0 );
   assert( db_input.node_table.size() == 0 );
   assert( db_input.weight_grid_table.size() == 0 );
   assert( db_input.mulcov_table.size() == 0 );
   assert( db_input.option_table.size() == 0 );
   assert( db_input.nslist_table.size() == 0 );
   assert( db_input.nslist_pair_table.size() == 0 );
   assert( db_input.subgroup_table.size() == 0 );
   //
   assert( db_input.prior_table.size() == 0 );
   assert( db_input.smooth_grid_table.size() == 0 );
   assert( db_input.integrand_table.size() == 0 );
   assert( db_input.data_table.size() == 0 );
   assert( db_input.avgint_table.size() == 0 );
   //
   db_input.age_table         = get_age_table(db);
   db_input.time_table        = get_time_table(db);
   db_input.rate_table        = get_rate_table(db);
   db_input.density_table     = get_density_table(db);
   db_input.weight_table      = get_weight_table(db);
   db_input.smooth_table      = get_smooth_table(db);
   db_input.covariate_table   = get_covariate_table(db);
   db_input.node_table        = get_node_table(db);
   db_input.weight_grid_table = get_weight_grid(db);
   db_input.option_table      = get_option_table(db);
   db_input.nslist_table      = get_nslist_table(db);
   db_input.nslist_pair_table = get_nslist_pair(db);
   db_input.subgroup_table    = get_subgroup_table(db);
   //
   // get_mulcov_table uses subgroup table
   // to check for erros
   db_input.mulcov_table = get_mulcov_table(db, db_input.subgroup_table);
   //
   // get_prior_table uses density_table
   // to check for errors
   db_input.prior_table = get_prior_table(db, db_input.density_table);
   //
   // get_smooth_grid_table uses density_table and prior_table
   // to check for errors
   db_input.smooth_grid_table = get_smooth_grid(
      db, db_input.density_table, db_input.prior_table
   );
   //
   // get_integrand_table uses mulcov_table and option_table
   // to check for errors
   db_input.integrand_table  = get_integrand_table(
      db, db_input.mulcov_table, db_input.option_table
   );
   //
   // get_data_table and get_avgint_table use this information
   // to check for errors
   size_t n_covariate      = db_input.covariate_table.size();
   double age_min          = min_vector( db_input.age_table );
   double age_max          = max_vector( db_input.age_table );
   double time_min         = min_vector( db_input.time_table );
   double time_max         = max_vector( db_input.time_table );
   get_data_table(
      db, db_input.density_table,
      n_covariate, age_min, age_max, time_min, time_max,
      db_input.data_table, db_input.data_cov_value
   );
   get_avgint_table(
      db, n_covariate, age_min, age_max, time_min, time_max,
      db_input.avgint_table, db_input.avgint_cov_value
   );
   //
   // -----------------------------------------------------------------------
   // check primary keys
   // -----------------------------------------------------------------------
   std::string message, table_name;
   //
   // node table
   DISMOD_AT_CHECK_PRIMARY_ID(node, parent, node);

   // prior table
   DISMOD_AT_CHECK_PRIMARY_ID(prior, density_id, density);

   // weight_grid table
   DISMOD_AT_CHECK_PRIMARY_ID(weight_grid, weight_id, weight);
   DISMOD_AT_CHECK_PRIMARY_ID(weight_grid, age_id,    age);
   DISMOD_AT_CHECK_PRIMARY_ID(weight_grid, time_id, time);

   // smooth table
   DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_value_prior_id, prior);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dage_prior_id,  prior);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dtime_prior_id, prior);

   // smooth_grid table
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, smooth_id,      smooth);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, value_prior_id, prior);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dage_prior_id,  prior);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dtime_prior_id, prior);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, age_id,         age);
   DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, time_id,        time);

   // mulcov table
   DISMOD_AT_CHECK_PRIMARY_ID(mulcov, rate_id,             rate);
   DISMOD_AT_CHECK_PRIMARY_ID(mulcov, integrand_id,        integrand);
   DISMOD_AT_CHECK_PRIMARY_ID(mulcov, covariate_id,        covariate);
   DISMOD_AT_CHECK_PRIMARY_ID(mulcov, group_smooth_id,     smooth);
   DISMOD_AT_CHECK_PRIMARY_ID(mulcov, subgroup_smooth_id,  smooth);

   // data table
   DISMOD_AT_CHECK_PRIMARY_ID(data, integrand_id, integrand);
   DISMOD_AT_CHECK_PRIMARY_ID(data, density_id,   density);
   DISMOD_AT_CHECK_PRIMARY_ID(data, node_id,      node);
   DISMOD_AT_CHECK_PRIMARY_ID(data, subgroup_id,  subgroup);
   DISMOD_AT_CHECK_PRIMARY_ID(data, weight_id,    weight);

   // avgint table
   DISMOD_AT_CHECK_PRIMARY_ID(avgint, integrand_id, integrand);
   DISMOD_AT_CHECK_PRIMARY_ID(avgint, node_id,      node);
   DISMOD_AT_CHECK_PRIMARY_ID(avgint, subgroup_id,  subgroup);
   DISMOD_AT_CHECK_PRIMARY_ID(avgint, weight_id,    weight);

   // rate table
   DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth);
   DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth);
   DISMOD_AT_CHECK_PRIMARY_ID(rate, child_nslist_id,  nslist);

   // nslist_pair table
   DISMOD_AT_CHECK_PRIMARY_ID(nslist_pair, nslist_id, nslist);
   DISMOD_AT_CHECK_PRIMARY_ID(nslist_pair, smooth_id, smooth);
   DISMOD_AT_CHECK_PRIMARY_ID(nslist_pair, node_id,   node);

   // -----------------------------------------------------------------------
   // get rate_case
   std::string rate_case;
   size_t n_option = db_input.option_table.size();
   //
   // note index is option_table is not the same as in database
   for(size_t i = 0; i < n_option; i++)
      if( db_input.option_table[i].option_name == "rate_case" )
         rate_case = db_input.option_table[i].option_value;
   assert( rate_case != "" );
   // -----------------------------------------------------------------------
   // other checks
   check_pini_n_age(
      db                        ,
      db_input.rate_table       ,
      db_input.smooth_table
   );
   check_rate_limit(
      db                        ,
      rate_case                 ,
      db_input.rate_table       ,
      db_input.prior_table      ,
      db_input.smooth_grid_table
   );
   check_child_prior(
      db                         ,
      db_input.rate_table        ,
      db_input.smooth_grid_table ,
      db_input.nslist_pair_table ,
      db_input.prior_table
   );
   check_child_nslist(
      db                         ,
      db_input.option_table      ,
      db_input.rate_table        ,
      db_input.node_table        ,
      db_input.nslist_table      ,
      db_input.nslist_pair_table
   );
   check_zero_sum(
      db                         ,
      db_input.rate_table        ,
      db_input.option_table
   );
   return;
}

} // END DISMOD_AT_NAMESPACE
