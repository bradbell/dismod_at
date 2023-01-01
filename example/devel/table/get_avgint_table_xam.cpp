// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_avgint_table_xam.cpp$$
$spell
   avgint
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ get_avgint_table: Example and Test$$

$srcthisfile%
   0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_avgint_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_avgint_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
   "create table avgint("
      " avgint_id    integer primary key,"
      " integrand_id   integer,"
      " node_id        integer,"
      " subgroup_id    integer,"
      " weight_id      integer,"
      " age_lower      real,"
      " age_upper      real,"
      " time_lower     real,"
      " time_upper     real,"
      " x_0            real,"
      " x_1            real"
   ")",
   "insert into avgint values("
      "0,"                       // avgint_id
      "1,"                       // integrand_id
      "3,"                       // node_id
      "4,"                       // subgroup_id
      "5,"                       // weight_id
      "10.0,"                    // age_lower
      "90.0,"                    // age_upper
      "2000,"                    // time_lower
      "2010,"                    // time_upper
      "0.5,"                     // x_0  (sex)
      "1000"                     // x_1  (income)
   ")"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);

   // get the data table
   size_t n_covariate = 2;
   double age_min     = 0.0;
   double age_max     = 100.0;
   double time_min    = 1900.;
   double time_max    = 2015.;
   vector<dismod_at::avgint_struct> avgint_table(0);
   vector<double> avgint_cov_value(0);
   dismod_at::get_avgint_table(
         db, n_covariate, age_min, age_max, time_min, time_max,
         avgint_table, avgint_cov_value
   );
   ok  &= avgint_table.size() == 1;
   //
   ok  &= avgint_table[0].integrand_id      == 1;
   ok  &= avgint_table[0].node_id           == 3;
   ok  &= avgint_table[0].subgroup_id       == 4;
   ok  &= avgint_table[0].weight_id         == 5;
   ok  &= avgint_table[0].age_lower         == 10.0;
   ok  &= avgint_table[0].age_upper         == 90.0;
   ok  &= avgint_table[0].time_lower        == 2000.0;
   ok  &= avgint_table[0].time_upper        == 2010.0;
   ok  &= avgint_cov_value[ 0 * n_covariate + 0] == 0.5;
   ok  &= avgint_cov_value[ 0 * n_covariate + 1] == 1000.0;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
