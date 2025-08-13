// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <iostream>
# include <cassert>
# include <cstring>

// this directory
extern bool age_time_order(void);
extern bool data_model_subset(void);
extern bool grid2line(void);
extern bool meas_mulcov(void);
extern bool rate_mulcov(void);
extern bool cppad_mixed_xam(void);
extern bool create_table_split(void);

// anonymous namespace
namespace {
   using std::cout;
   using std::endl;

   // function that runs one test
   static size_t Run_ok_count    = 0;
   static size_t Run_error_count = 0;
   void Run(bool test_fun(void), const char* test_name)
   {
      std::streamsize width = 30;
      cout.width( width );
      cout.setf( std::ios_base::left );
      cout << test_name << ':';
      assert( std::strlen(test_name) < size_t(width) );
      //
      bool ok = test_fun();
      if( ok )
      {  cout << "OK" << endl;
         Run_ok_count++;
      }
      else
      {  cout << "Error" << endl;
         Run_error_count++;
      }
   }
}
// macro for calls Run
# define RUN(test_name) Run( test_name, #test_name )

// main program that runs all the tests
int main(void)
{
   // this directory
   RUN(age_time_order);
   RUN(data_model_subset);
   RUN(grid2line);
   RUN(meas_mulcov);
   RUN(rate_mulcov);
   RUN(cppad_mixed_xam);
   RUN(create_table_split);

   // summary report
   int return_flag;
   if( Run_error_count == 0 )
   {  cout << "All " << Run_ok_count << " tests passed." << endl;
      return_flag = 0;
   }
   else
   {  cout << Run_error_count << " tests failed." << endl;
      return_flag = 1;
   }
   return return_flag;
}
