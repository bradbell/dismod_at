// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin dismod_at_exe$$
$spell
	Dismod
	Ipopt
	num_iter
$$

$section The Dismod Age Time Executable$$

$head Syntax$$
$codei%dismod_at %database% \
	%parent_node% %ode_step_size% %tolerance% %max_num_iter%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ and $code output$$ tables.
The input tables are not modified and result are written to the
output tables.

$head parent_node$$
If all of the characters in $icode parent_node$$ are decimal digits,
then  it specifies the parent
$cref/node_id/node_table/node_id/$$ for this fit.
Otherwise it specifies the parent
$cref/node_name/node_table/node_name/$$.

$head ode_step_size$$
is the step size used to integrate the ordinary differential equation.
The step size is the same in age and time because it is along cohort lines.

$head tolerance$$
is the desired relative convergence tolerance (requested of Ipopt).

$head max_num_iter$$
is the maximum number of iterations.
The algorithm terminates with an error message if the number of
iterations exceeded this number.

$end
*/
