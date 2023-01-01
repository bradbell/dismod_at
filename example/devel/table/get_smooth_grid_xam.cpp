// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_smooth_grid_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ get_smooth_grid: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/get_smooth_grid.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>
# include <cmath>

bool get_smooth_grid_xam(void)
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
   density_table[2] = dismod_at::gaussian_enum;
   density_table[3] = dismod_at::log_gaussian_enum;
   //
   // prior_table (only density_id is used by get_smooth_grid)
   CppAD::vector<dismod_at::prior_struct> prior_table(4);
   for(int prior_id = 0; prior_id < 4; prior_id++)
      prior_table[prior_id].density_id = prior_id;

   // sql commands
   const char* sql_cmd[] = {
   "create table smooth("
      "smooth_id                integer primary key,"
      "smooth_name              text unique,"
      "n_age                    integer,"
      "n_time                   integer,"
      "mulstd_value_prior_id    integer,"
      "mulstd_dage_prior_id     integer,"
      "mulstd_dtime_prior_id    integer)",
   "insert into smooth values(0, 'constant',  1, 1, 0, 1, 2)",
   "insert into smooth values(1, 'age_only',  2, 1, 3, 4, 5)",
   "insert into smooth values(2, 'time_only', 1, 2, 6, 7, 8)",
   "insert into smooth values(3, 'age_time',  2, 2, 9, 10, 11)",
   //
   "create table smooth_grid("
      " smooth_grid_id integer primary key,"
      " smooth_id      integer, "
      " age_id         integer,"
      " time_id        integer,"
      " value_prior_id integer,"
      " dage_prior_id  integer,"
      " dtime_prior_id integer,"
      " const_value    real)",
   // smooth_grid_id, smooth_id, age_id, time_id, prior_ids, const_value
   "insert into smooth_grid values(0, 0, 1, 1,   1,    null, null, null)",
   "insert into smooth_grid values(1, 3, 0, 0,   1,       2,    3, null)",
   "insert into smooth_grid values(2, 3, 2, 0,   1,    null,    3, null)",
   "insert into smooth_grid values(3, 3, 0, 2,   1,       2, null, null)",
   "insert into smooth_grid values(4, 3, 2, 2,   null, null, null, 12.0)",
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);

   // get the smooth table
   vector<dismod_at::smooth_struct> smooth_table =
      dismod_at::get_smooth_table(db);
   ok  &= smooth_table.size()    == 4;
   ok  &= smooth_table[0].smooth_name    == "constant";
   ok  &= smooth_table[0].n_age          == 1;
   ok  &= smooth_table[0].n_time         == 1;
   ok  &= smooth_table[0].mulstd_value_prior_id   == 0;
   ok  &= smooth_table[0].mulstd_dage_prior_id    == 1;
   ok  &= smooth_table[0].mulstd_dtime_prior_id   == 2;
   //
   ok  &= smooth_table[1].smooth_name    == "age_only";
   ok  &= smooth_table[1].n_age          == 2;
   ok  &= smooth_table[1].n_time         == 1;
   ok  &= smooth_table[1].mulstd_value_prior_id   == 3;
   ok  &= smooth_table[1].mulstd_dage_prior_id    == 4;
   ok  &= smooth_table[1].mulstd_dtime_prior_id   == 5;
   //
   ok  &= smooth_table[2].smooth_name    == "time_only";
   ok  &= smooth_table[2].n_age          == 1;
   ok  &= smooth_table[2].n_time         == 2;
   ok  &= smooth_table[2].mulstd_value_prior_id   == 6;
   ok  &= smooth_table[2].mulstd_dage_prior_id    == 7;
   ok  &= smooth_table[2].mulstd_dtime_prior_id   == 8;
   //
   ok  &= smooth_table[3].smooth_name    == "age_time";
   ok  &= smooth_table[3].n_age          == 2;
   ok  &= smooth_table[3].n_time         == 2;
   ok  &= smooth_table[3].mulstd_value_prior_id   == 9;
   ok  &= smooth_table[3].mulstd_dage_prior_id    == 10;
   ok  &= smooth_table[3].mulstd_dtime_prior_id   == 11;
   //
   // get the smooth_grid table
   vector<dismod_at::smooth_grid_struct> smooth_grid =
      dismod_at::get_smooth_grid(db, density_table, prior_table);
   ok  &= smooth_grid.size() == 5;
   //
   ok  &= smooth_grid[0].smooth_id      ==  0;
   ok  &= smooth_grid[0].age_id         ==  1;
   ok  &= smooth_grid[0].time_id        ==  1;
   ok  &= smooth_grid[0].value_prior_id ==  1;
   ok  &= smooth_grid[0].dage_prior_id  ==  DISMOD_AT_NULL_INT;
   ok  &= smooth_grid[0].dtime_prior_id ==  DISMOD_AT_NULL_INT;
   ok  &= std::isnan( smooth_grid[0].const_value );
   //
   ok  &= smooth_grid[1].smooth_id      ==  3;
   ok  &= smooth_grid[1].age_id         ==  0;
   ok  &= smooth_grid[1].time_id        ==  0;
   ok  &= smooth_grid[1].value_prior_id ==  1;
   ok  &= smooth_grid[1].dage_prior_id  ==  2;
   ok  &= smooth_grid[1].dtime_prior_id ==  3;
   ok  &= std::isnan( smooth_grid[1].const_value );
   //
   ok  &= smooth_grid[2].smooth_id      ==  3;
   ok  &= smooth_grid[2].age_id         ==  2;
   ok  &= smooth_grid[2].time_id        ==  0;
   ok  &= smooth_grid[2].value_prior_id ==  1;
   ok  &= smooth_grid[2].dage_prior_id  ==  DISMOD_AT_NULL_INT;
   ok  &= smooth_grid[2].dtime_prior_id ==  3;
   ok  &= std::isnan( smooth_grid[2].const_value );
   //
   ok  &= smooth_grid[3].smooth_id      ==  3;
   ok  &= smooth_grid[3].age_id         ==  0;
   ok  &= smooth_grid[3].time_id        ==  2;
   ok  &= smooth_grid[3].value_prior_id ==  1;
   ok  &= smooth_grid[3].dage_prior_id  ==  2;
   ok  &= smooth_grid[3].dtime_prior_id ==  DISMOD_AT_NULL_INT;
   ok  &= std::isnan( smooth_grid[3].const_value );
   //
   ok  &= smooth_grid[4].smooth_id      ==  3;
   ok  &= smooth_grid[4].age_id         ==  2;
   ok  &= smooth_grid[4].time_id        ==  2;
   ok  &= smooth_grid[4].value_prior_id ==  DISMOD_AT_NULL_INT;
   ok  &= smooth_grid[4].dage_prior_id  ==  DISMOD_AT_NULL_INT;
   ok  &= smooth_grid[4].dtime_prior_id ==  DISMOD_AT_NULL_INT;
   ok  &= smooth_grid[4].const_value    ==  12.0;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
