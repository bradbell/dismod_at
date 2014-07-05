// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin table_error_exit$$

$section Print Table Error Message and Exit$$

$head Syntax$$
$codei%# include <dismod_at/table_error_exit.hpp>
%$$
$codei%table_error_exit(%table_name%, %row_id%, %message%)%$$

$head Purpose$$
Print an error message and exit the program with a non-zero
error status.

$head table_name$$
It the name of the table where the error occurred.

$head row_id$$
Is the row identifier value $icode%table_name%_id%$$; 
i.e., it startes at zero and increments by one between rows.

$head messae$$
Is the error message. An addition line identifying the 
row is added to the message.

$end
------------------------------------------------------------------------------
*/
# include <iostream>
# include <cstdlib>

namespace dismod_at {
	void table_error_exit(
		const std::string& table_name , 
		size_t             row_id     ,
		const std::string& message    )
	{	using std::cerr;
		using std::endl;
		cerr << message << endl;
		cerr << "Detected in " << table_name << " table";
		if( row_id >= 0 )
			cerr << " at " << table_name << "_id = " << row_id;
		cerr << "." << endl;
		exit(1);
	}
}
