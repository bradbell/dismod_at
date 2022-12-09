// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin open_connection dev}
{xrst_spell
   bool
}

Open An Sqlite3 Database Connection
###################################

Syntax
******

   *db* = ``open_connection`` ( *file_name* , *new_file* )

file_name
*********
This argument has prototype

   ``const std::string&`` *file_name*

It is the name of the file where the data base is
(or will be) stored.

new_file
********
This argument has prototype

   ``bool`` *new_file*

If it is true, and a database with the same name already exists,
the existing database is deleted before creating the connection.

db
**
The return value has prototype

   ``sqlite3`` * *db*

and is the database connection.

close
*****
When you are done with *db* , you must call

   ``sqlite3_close`` ( *db* )

{xrst_end open_connection}
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <dismod_at/open_connection.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

sqlite3* open_connection(const std::string& file_name, bool new_file)
{
   // attempt to open the database
   std::ifstream ifile( file_name.c_str() );
   if( new_file )
   {  if( ifile )
      {   // remove the old version of the file
   ifile.close();
         std::remove( file_name.c_str() );
      }
   }
   else
   {  // check that the database exists
      if( ! ifile )
      {  std::string msg;
         msg = "Cannot find the database " + file_name;
          std::cerr << msg << std::endl;
          std::exit(1);
      }
   }
   ifile.close();

   // open a new database
   sqlite3* db;
   int rc = sqlite3_open(file_name.c_str(), &db);
   if( rc )
   {  std::string msg;
      if( new_file )
         msg = "Can't create a new database in file ";
      else
         msg = "Can't open the existing database in file ";
      std::cerr << msg << file_name << std::endl;
      sqlite3_close(db);
      std::exit(1);
   }
   return db;
}

} // END_DISMOD_AT_NAMESPACE
