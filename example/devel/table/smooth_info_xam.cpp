// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin smooth_info_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++ smooth_info: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/null_int.hpp>
# include <cmath>

bool smooth_info_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);
   //
   // density_table
   CppAD::vector<dismod_at::density_enum> density_table(4);
   density_table[0] = dismod_at::uniform_enum;
   density_table[1] = dismod_at::cen_gaussian_enum;
   //
   // prior_table (only density_id is used by get_smooth_grid)
   CppAD::vector<dismod_at::prior_struct> prior_table(10);
   for(int prior_id = 0; prior_id < 4; prior_id++)
      prior_table[prior_id].density_id = 0; // uniform
   // prior_id = 1 is not used for a dage or dtime prior
   prior_table[1].density_id = 1; // cen_gaussian

   // sql commands
   const char* sql_cmd[] = {
   "create table age(age_id integer primary key, age real)",
   "insert into age values(0,  00.0)",
   "insert into age values(1,  25.0)",
   "insert into age values(2,  50.0)",
   "insert into age values(3,  75.0)",
   "insert into age values(4, 100.0)",
   //
   "create table time(time_id integer primary key, time real)",
   "insert into time values(0,  1990.0)",
   "insert into time values(1,  2010.0)",
   //
   "create table smooth("
      " smooth_id               integer primary key,"
      " smooth_name             text unique,   "
      " n_age                   integer,"
      " n_time                  integer,"
      " mulstd_value_prior_id   integer,"
      " mulstd_dage_prior_id    integer,"
      " mulstd_dtime_prior_id   integer)",
   "insert into smooth values(0, 'bilinear', 3, 2, 1, 1, 1)",
   //
   "create table smooth_grid("
      " smooth_grid_id integer primary key,"
      " smooth_id      integer,"
      " age_id         integer,"
      " time_id        integer,"
      " value_prior_id integer,"
      " dage_prior_id  integer,"
      " dtime_prior_id integer,"
      " const_value    real)",
   "insert into smooth_grid values(0,  0,  0,  1,  1,       2, null, null)",
   "insert into smooth_grid values(1,  0,  2,  1,  2,       3, null, null)",
   "insert into smooth_grid values(2,  0,  4,  1,  3,    null, null, null)",
   "insert into smooth_grid values(3,  0,  0,  0,  5,       6,    7, null)",
   "insert into smooth_grid values(4,  0,  2,  0,  7,       8,    9, null)",
   "insert into smooth_grid values(5,  0,  4,  0,  null, null,    3, 10.0)",
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);

   // get age table
   vector<double> age_table = dismod_at::get_age_table(db);

   // get time table
   vector<double> time_table = dismod_at::get_time_table(db);

   // get the smooth table
   vector<dismod_at::smooth_struct>
      smooth_table = dismod_at::get_smooth_table(db);

   // get the smooth_grid table
   vector<dismod_at::smooth_grid_struct> smooth_grid_table =
      dismod_at::get_smooth_grid(db, density_table, prior_table);

   // extract the smoothing information
   size_t smooth_id = 0;
   dismod_at::smooth_info  s_info(
      smooth_id,
      age_table,
      time_table,
      prior_table,
      smooth_table,
      smooth_grid_table
   );
   //
   size_t null_size_t = DISMOD_AT_NULL_SIZE_T;

   ok  &= s_info.age_size()  == 3;
   ok  &= s_info.time_size() == 2;
   //
   ok  &= s_info.age_id(0)   == 0;
   ok  &= s_info.age_id(1)   == 2;
   ok  &= s_info.age_id(2)   == 4;
   //
   ok  &= s_info.time_id(0)  == 0;
   ok  &= s_info.time_id(1)  == 1;
   //
   ok  &= s_info.value_prior_id(0, 1) ==  1;
   ok  &= s_info.value_prior_id(1, 1) ==  2;
   ok  &= s_info.value_prior_id(2, 1) ==  3;
   ok  &= s_info.value_prior_id(0, 0) ==  5;
   ok  &= s_info.value_prior_id(1, 0) ==  7;
   ok  &= s_info.value_prior_id(2, 0) ==  null_size_t;
   //
   ok  &= s_info.dage_prior_id(0, 1)  ==  2;
   ok  &= s_info.dage_prior_id(1, 1)  ==  3;
   ok  &= s_info.dage_prior_id(2, 1)  == null_size_t;
   ok  &= s_info.dage_prior_id(0, 0)  ==  6;
   ok  &= s_info.dage_prior_id(1, 0)  ==  8;
   ok  &= s_info.dage_prior_id(2, 0)  == null_size_t;
   //
   ok  &= s_info.dtime_prior_id(0, 1) == null_size_t;
   ok  &= s_info.dtime_prior_id(1, 1) == null_size_t;
   ok  &= s_info.dtime_prior_id(2, 1) == null_size_t;
   ok  &= s_info.dtime_prior_id(0, 0) ==  7;
   ok  &= s_info.dtime_prior_id(1, 0) ==  9;
   ok  &= s_info.dtime_prior_id(2, 0) ==  3;
   //
   ok  &= s_info.mulstd_value()      == 1;
   ok  &= s_info.mulstd_dage()       == 1;
   ok  &= s_info.mulstd_dtime()      == 1;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
