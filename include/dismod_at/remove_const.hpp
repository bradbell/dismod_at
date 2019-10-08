// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_REMOVE_CONST_HPP
# define DISMOD_AT_REMOVE_CONST_HPP
/*
$begin remove_const$$
$spell
	obj
	const
	var
$$

$section Remove and Restore Components of a Vector that are Constant$$

$head Syntax$$
$codei%remove_const %remove_obj%(%lower_bound%, %upper_bound%)
%$$
$icode%n_both%  = %remove_obj%.both()
%$$
$icode%n_const% = %remove_obj%.n_const()
%$$
$icode%n_var%   = %remove_obj%.n_var()
%$$
$icode%lower%   = %remove_obj%.lower()
%$$
$icode%upper%   = %remove_obj%.upper()
%$$
$icode%var%     = %remove_obj%.remove(%both%)
%$$
$icode%both%    = %remove_obj%.restore(%var%)
%$$

$head Prototype$$
$srcfile%include/dismod_at/remove_const.hpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head lower_bound$$
This is the lower bounds for the components of a the vector.

$head upper_bound$$
This is the upper bounds for the components of a the vector
(it must have the same size as $icode lower_bound$$).

$head n_both$$
is the size of in the bound vectors

$head n_const$$
is the number of components of $icode lower_bound$$
that are equal to the corresponding component of $icode upper_bound$$

$head n_var$$
is the number of components of $icode lower_bound$$
that are not equal to the corresponding component of $icode upper_bound$$

$head lower$$
is a $code const$$ reference to a copy of $icode lower_bound$$

$head upper$$
is a $code const$$ reference to a copy of $icode upper_bound$$

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
# include <cppad/utility/vector.hpp>
namespace dismod_at {

class remove_const{
// --------------------------------------------------------------------------
private:
const CppAD::vector<double> lower_bound_;
const CppAD::vector<double> upper_bound_;
const size_t                n_both_;
// effectevly const: set by constructor and does not change.
size_t                      n_const_;
// --------------------------------------------------------------------------
public:
// BEGIN_PROTOTYPE
remove_const(
	CppAD::vector<double> lower_bound ,
	CppAD::vector<double> upper_bound
);
size_t n_both(void) const;
size_t n_const(void) const;
size_t n_var(void) const;
const CppAD::vector<double>& lower(void) const;
const CppAD::vector<double>& upper(void) const;
// remove
template <class Scalar> CppAD::vector<Scalar>
remove(const CppAD::vector<Scalar>& both) const;
// replace
template <class Scalar> CppAD::vector<Scalar>
restore(const CppAD::vector<Scalar>& var) const;
// END_PROTOTYPE
};
// remove
template <class Scalar> CppAD::vector<Scalar>
remove_const::remove(const CppAD::vector<Scalar>& both) const
{	assert( both.size() == n_both_ );
	CppAD::vector<Scalar> var( n_var() );
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
// restore
template <class Scalar> CppAD::vector<Scalar>
remove_const::restore(const CppAD::vector<Scalar>& var) const
{	assert( var.size() == n_var() );
	CppAD::vector<Scalar> both( n_both_ );
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

} // END_DISMOD_AT_NAMESPACE

# endif
