// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <gsl/gsl_statistics_double.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/data_density_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin data_density_command}

Data Density Command: Change the Density for an Integrand
#########################################################

Syntax
******

| ``dismod_at`` *database* ``data_density``
| ``dismod_at`` *database* ``data_density`` *integrand_name* *density_name* *eta_factor* *nu*

Purpose
*******
This command is used to change the density used for an integrand
during subsequent fits. .
If :ref:`data_table@density_id`
corresponds to :ref:`density_table@density_name@binomial`,
the corresponding :ref:`data_table@meas_std` is null and
hence the density can't be changed.

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.
If this is the only argument, all the data densities are set back to the
values specified by the :ref:`data_table-name` .

integrand_name
**************
This is the
:ref:`integrand<integrand_table@integrand_name>` that we setting
the density for.

density_name
************
This is the :ref:`density_table@density_name`
that we are using for the new density,
except when the original density is binomial.

eta_factor
**********
This specifies the value of
:ref:`statistic@Notation@eta` used with this density setting.
This value is not used when the density is
:ref:`density_table@Notation@Linear`
(You can use the text ``null`` in this case).
If the density is
:ref:`density_table@Notation@Log Scaled` ,
the value of *eta* used with this density is

   *eta* = *eta_factor* * ``median`` ( *meas_value*  )

Here ``median`` ( *meas_value* ) is the median of the
:ref:`data_table@meas_value` in the data table.

nu
**
This specifies the value of
:ref:`statistic@Notation@nu` used with this density setting.
This value is only used when the density is
``students`` or ``log_students`` .
(You can use text ``null`` is when it is not used.)

data_subset_table
*****************
Only rows of the :ref:`data_subset_table-name` that correspond to this integrand
are modified.

density_id
==========
The subset table :ref:`data_subset_table@density_id`
for this integrand is changed to this *density* .

eta
===
If the density is
:ref:`density_table@Notation@Log Scaled` ,
the subset table :ref:`data_subset_table@eta`
is set to the value defined above.
Otherwise, it is set to null.

nu
==
If the density is ``students`` or ``log_students`` ,
the subset table :ref:`data_subset_table@nu`
is set to the value specified above.
Otherwise, it is set to null.

Example
*******
The file :ref:`user_data_density.py-name` contains an example and test
using this command.

{xrst_end data_density_command}
*/
void data_density_command(
   sqlite3*                                      db                ,
   std::string&                                  integrand_name    ,
   std::string&                                  density_name      ,
   std::string&                                  eta_factor_str    ,
   std::string&                                  nu_str            ,
   const CppAD::vector<integrand_struct>&        integrand_table   ,
   const CppAD::vector<density_enum>&            density_table     ,
   const CppAD::vector<data_struct>&             data_table        )
{  using std::string;
   using CppAD::vector;
   using CppAD::to_string;
   double nan = std::numeric_limits<double>::quiet_NaN();
   //
   // data_subset_table
   vector<data_subset_struct> data_subset_table = get_data_subset(db);
   //
   // n_subset
   size_t n_subset = data_subset_table.size();
   //
   // case where we are restoring the data table settings
   if( integrand_name == "" )
   {  for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
      {  size_t data_id = data_subset_table[subset_id].data_id;
         data_subset_table[subset_id].density_id =
            data_table[data_id].density_id;
         data_subset_table[subset_id].eta          = data_table[data_id].eta;
         data_subset_table[subset_id].nu           = data_table[data_id].nu;
         data_subset_table[subset_id].sample_size  = \
            data_table[data_id].sample_size;
      }
   }
   else
   {
      // integrand_id
      size_t integrand_id = integrand_table.size();
      for(size_t id = 0; id < integrand_table.size(); ++id)
      {  string name = integrand_enum2name[ integrand_table[id].integrand];
         if( name == integrand_name )
            integrand_id = id;
      }
      if( integrand_id == integrand_table.size() )
      {  string msg = "data_density_command: " + integrand_name;
         msg       += " is not a in the integrand table";
         error_exit(msg);
      }
      // binomial_id, density_id
      size_t binomial_id = density_table.size();
      size_t density_id  = density_table.size();
      for(size_t id = 0; id < density_table.size(); ++id)
      {  string name = density_enum2name[ density_table[id] ];
         if( name == density_name )
            density_id = id;
         if( name == "binomial" )
            binomial_id = id;
      }
      if( density_id == density_table.size() )
      {  string msg = "data_density_command: " + density_name;
         msg       += " is not a in the density table";
         error_exit(msg);
      }
      // density
      density_enum density = density_table[density_id];
      //
      // eta
      double eta = nan;
      if( log_density(density) )
      {  // meas_value
         vector<double> meas_value;
         size_t n_data = data_table.size();
         for(size_t data_id = 0; data_id < n_data; ++data_id)
         {  if( data_table[data_id].integrand_id == int(integrand_id ) )
               meas_value.push_back( data_table[data_id].meas_value );
         }
         // integrand_median
         size_t stride = 1;
         double integrand_median = gsl_stats_median(
            meas_value.data(), stride, meas_value.size()
         );
         // eta_factor
         double eta_factor = std::atof( eta_factor_str.c_str() );
         //
         // eta
         eta = eta_factor * integrand_median;
      }
      // nu
      double nu = nan;
      if( density == students_enum || density == log_students_enum )
         nu = std::atof( nu_str.c_str() );
      //
      // data_subset_table
      for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
      {  size_t data_id = data_subset_table[subset_id].data_id;
         if( data_table[data_id].density_id   != int(binomial_id)
         &&  data_table[data_id].integrand_id == int(integrand_id) )
         {  data_subset_table[subset_id].density_id =  int( density_id );
            data_subset_table[subset_id].eta        = eta;
            data_subset_table[subset_id].nu         = nu;
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
      //
      string eta_str = "";
      if( ! std::isnan( eta ) )
         eta_str = to_string( eta );
      //
      string nu_tmp = "";
      if( ! std::isnan( nu ) )
         nu_tmp = to_string( nu );
      //
      string sample_size_str = "";
      if( sample_size != DISMOD_AT_NULL_INT )
         sample_size_str = to_string( sample_size );
      //
      row_value[n_col * subset_id + 0] = to_string( data_id );
      row_value[n_col * subset_id + 1] = to_string( hold_out );
      row_value[n_col * subset_id + 2] = to_string( density_id );
      row_value[n_col * subset_id + 3] = eta_str;
      row_value[n_col * subset_id + 4] = nu_tmp;
      row_value[n_col * subset_id + 5] = sample_size_str;
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   return;
}
} // END_DISMOD_AT_NAMESPACE
