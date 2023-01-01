// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_integrand_table_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ get_integrand_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool get_integrand_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
   "create table integrand"
   "(integrand_id     integer primary key,"
   " integrand_name   text unique,"
   " minimum_meas_cv  real                        )",
   "insert into integrand values(0, 'mtall',       0.1 )",
   "insert into integrand values(1, 'prevalence',  0.2 )",
   "insert into integrand values(2, 'remission',   0.3 )",
   "insert into integrand values(3, 'Sincidence',  0.4 )",
   "insert into integrand values(4, 'susceptible', 0.5 )",
   "insert into integrand values(5, 'withC',       0.6 )",
   "insert into integrand values(6, 'mulcov_1',    0.7 )",
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);

   // A mulcov table with two entries
   vector<dismod_at::mulcov_struct> mulcov_table(2);
   for(size_t j = 0; j < 2; ++j)
   {  mulcov_table[j].mulcov_type = dismod_at::rate_value_enum;
      mulcov_table[j].rate_id = int(j);
      mulcov_table[j].integrand_id = 0;
      mulcov_table[j].covariate_id = 0;
      mulcov_table[j].group_smooth_id = 0;
      mulcov_table[j].subgroup_smooth_id = 0;
   }

   // An empty option table
   vector<dismod_at::option_struct> option_table(0);

   // get the integrand table
   vector<dismod_at::integrand_struct> integrand_table =
      dismod_at::get_integrand_table(db, mulcov_table, option_table);
   //
   ok  &= integrand_table.size() == 7;
   //
   ok  &= integrand_table[0].integrand == dismod_at::mtall_enum;
   ok  &= integrand_table[1].integrand == dismod_at::prevalence_enum;
   ok  &= integrand_table[2].integrand == dismod_at::remission_enum;
   ok  &= integrand_table[3].integrand == dismod_at::Sincidence_enum;
   ok  &= integrand_table[4].integrand == dismod_at::susceptible_enum;
   ok  &= integrand_table[5].integrand == dismod_at::withC_enum;
   ok  &= integrand_table[6].integrand == dismod_at::mulcov_enum;
   //
   for(size_t i = 0; i < 7; i++)
      ok &= integrand_table[i].minimum_meas_cv == double(i + 1) / 10.0;
   for(size_t i = 0; i < 6; i++)
      ok &= integrand_table[i].mulcov_id == DISMOD_AT_NULL_INT;
   ok &= integrand_table[6].mulcov_id == 1;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
