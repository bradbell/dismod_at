// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_column_max_xam.cpp$$
$spell
   xam
$$

$section C++ get_column_max: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cstdlib>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_column_max.hpp>

bool get_column_max_xam(void)
{
   bool   ok = true;
   using std::string;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // create the mytable table
   const char* sql_cmd[] = {
   "create table mytable("
      "mytable_id        integer primary key, "
      "name_one          real, "
      "name_two          real, "
      "name_three        real"
   ")",
   "insert into mytable values(0, 3.0, 1.0, null)",
   "insert into mytable values(1, 2.0, 2.0, null)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);
   //
   // check mytable_id
   string select_cmd  = "select * from mytable";
   string column_name = "mytable_id";
   string max_str     = dismod_at::get_column_max(
      db, select_cmd, column_name
   );
   ok              &= std::atoi(max_str.c_str()) == 1;

   // check name_one
   column_name = "name_one";
   max_str     = dismod_at::get_column_max(db, select_cmd, column_name);
   ok          &= std::atof(max_str.c_str()) == 3.0;

   // check name_two
   column_name = "name_two";
   max_str     = dismod_at::get_column_max(db, select_cmd, column_name);
   ok          &= std::atof(max_str.c_str()) == 2.0;

   // check name_three
   column_name = "name_three";
   max_str     = dismod_at::get_column_max(db, select_cmd, column_name);
   ok          &= max_str == "";

   // check a case where select_cmd limits the number of records
   select_cmd  = "select * from mytable where mytable_id=0";
   column_name = "name_two";
   max_str     = dismod_at::get_column_max(db, select_cmd, column_name);
   ok          &= std::atof(max_str.c_str()) == 1.0;

   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
