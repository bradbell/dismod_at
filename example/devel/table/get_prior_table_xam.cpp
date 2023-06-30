// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_prior_table_xam.cpp dev}

C++ get_prior_table: Example and Test
#####################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end get_prior_table_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_prior_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);
   //
   // density table
   size_t n_density = dismod_at::number_density_enum;
   vector<dismod_at::density_enum> density_table(n_density);
   for(size_t density_id = 0; density_id < n_density; ++density_id)
      density_table[density_id] = dismod_at::density_enum(density_id);
   //
   // sql commands
   std::string sql_cmd =
      "create table prior("
         " prior_id      integer primary key,"
         " prior_name    text unique,"
         " density_id   integer,"
         " lower        real,"
         " upper        real,"
         " mean         real,"
         " std          real,"
         " eta          real,"
         " nu           real)";
   dismod_at::exec_sql_cmd(db, sql_cmd.c_str());
   //
   sql_cmd  = "insert into prior values(0, 'none', ";
   sql_cmd += std::to_string( size_t(dismod_at::uniform_enum) );
   sql_cmd += ", null, null, 0.0, null, null, null)",
   dismod_at::exec_sql_cmd(db, sql_cmd.c_str());
   //
   sql_cmd  = "insert into prior values(1, 'rate', ";
   sql_cmd += std::to_string( size_t(dismod_at::gaussian_enum) );
   sql_cmd += ", 0.0,  1.0,  0.1, 1e-4, 1e-5, 5.0)";
   dismod_at::exec_sql_cmd(db, sql_cmd.c_str());
   //
   // get the prior table
   vector<dismod_at::prior_struct> prior_table =
         dismod_at::get_prior_table(db, density_table);
   ok  &= prior_table.size() == 2;
   //
   ok  &= prior_table[0].prior_name  == "none";
   ok  &= prior_table[0].density_id == size_t( dismod_at::uniform_enum );
   ok  &= prior_table[0].lower      == std::atof("-inf");
   ok  &= prior_table[0].upper      == std::atof("+inf");
   ok  &= prior_table[0].mean       == 0.0;
   //
   ok  &= prior_table[1].prior_name  == "rate";
   ok  &= prior_table[1].density_id == size_t( dismod_at::gaussian_enum );
   ok  &= prior_table[1].lower      == 0.0;
   ok  &= prior_table[1].upper      == 1.0;
   ok  &= prior_table[1].mean       == 0.1;
   ok  &= prior_table[1].std        == 1e-4;
   ok  &= prior_table[1].eta        == 1e-5;
   ok  &= prior_table[1].nu         == 5.0;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
