// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin remove_const$$
$spell
	obj
	const
	var
$$

$section Remove and Replace Components of a Vector that are Constant$$

$head Syntax$$
$codei%remove_const %remove_obj%(%lower_bound%, %upper_bound%)
%$$
$icode%n_both% = remove_obj.both()
%$$
$icode%n_const% = remove_obj.n_const()
%$$
$icode%n_var% = remove_obj.n_var()
%$$
$icode%var%  = remove_obj.remove(%both%)
%$$
$icode%both%  = remove_obj.remove(%var%)
%$$

$head Prototype$$
$srcfile%devel/utility/remove_const.cpp%
	0%// BEGIN_N_BOTH%// END_N_BOTH%1
%$$
$srcfile%devel/utility/remove_const.cpp%
	0%// BEGIN_N_CONST%// END_N_CONST%1
%$$
$srcfile%devel/utility/remove_const.cpp%
	0%// BEGIN_N_VAR%// END_N_VAR%1
%$$
$srcfile%devel/utility/remove_const.cpp%
	0%// BEGIN_REMOVE%// END_REMOVE%1
%$$
$srcfile%devel/utility/remove_const.cpp%
	0%// BEGIN_REPLACE%// END_REPLACE%1
%$$

$head lower_bound$$
This is the lower bounds for the components of a the vector.

$head upper_bound$$
This is the upper bounds for the components of a the vector
(it must have the same size as $icode lower_bound$$).

$head n_both$$
is the size of in the bound vectors.

$head n_const$$
is the number of components of $icode lower_bound$$
that are equal to the corresponding component of $icode upper_bound$$

$head n_var$$
is the number of components of $icode lower_bound$$
that are not equal to the corresponding component of $icode upper_bound$$

$head both$$
is a vector with size $icode n_both$$.

$head var$$
is a vector with size $icode n_var$$
that only contains the values in $icode both$$ for which
the lower and upper bound are not equal.
The order of the elements in $icode var$$ is the same as their
order in $icode both$$.

$end
*/
# include <dismod_at/remove_const.hpp>

namespace dismod_at {
	// 2DO: remove default constructor
	remove_const::remove_const(void)
	:
	lower_bound_(0)  ,
	upper_bound_(0)  ,
	n_both_(0)
	{	n_const_ = 0; }
	//
	// BEGIN_CTOR
	remove_const::remove_const(
		CppAD::vector<double> lower_bound ,
		CppAD::vector<double> upper_bound )
	// END_CTOR
	:
	lower_bound_(lower_bound)     ,
	upper_bound_(upper_bound)     ,
	n_both_( lower_bound.size() )
	{	assert( lower_bound.size() == upper_bound.size() );
		//
		// n_const_
		n_const_ = 0;
		for(size_t i = 0; i < n_both_; ++i)
			if( lower_bound[i] == upper_bound[i] )
				++n_const_;
	}
	// BEGIN_N_BOTH
	size_t remove_const::n_both(void) const
	// END_N_BOTH
	{	return n_both_; }
	//
	// BEGIN_N_CONST
	size_t remove_const::n_const(void) const
	// END_N_CONST
	{	return n_const_; }
	//
	// BEGIN_N_VAR
	size_t remove_const::n_var(void) const
	// END_N_VAR
	{	return n_both_ - n_const_; }
	//
	// BEGIN_REMOVE
	CppAD::vector<double>
	remove_const::remove(const CppAD::vector<double>& both) const
	// END_REMOVE
	{	assert( both.size() == n_both_ );
		CppAD::vector<double> var( n_var() );
		size_t k = 0;
		for(size_t i = 0; i < n_both_; ++i)
		{	if( lower_bound_[i] != upper_bound_[i] )
			{	var[k] = both[i];
				++k;
			}
		}
		assert( k == n_var() );
		return var;
	}
	//
	// BEGIN_REPLACE
	CppAD::vector<double>
	remove_const::replace(const CppAD::vector<double>& var) const
	// END_REPLACE
	{	assert( var.size() == n_var() );
		CppAD::vector<double> both( n_both_ );
		size_t k = 0;
		for(size_t i = 0; i < n_both_; ++i)
		{	if( lower_bound_[i] == upper_bound_[i] )
				both[i] = lower_bound_[i];
			else
			{	both[i] = var[k];
				++k;
			}
		}
		assert( k == n_var() );
		return both;
	}

}
