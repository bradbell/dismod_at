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
$icode%n_both%     = %remove_obj%.both()
%$$
$icode%n_const%    = %remove_obj%.n_const()
%$$
$icode%n_var%      = %remove_obj%.n_var()
%$$
$icode%lower%      = %remove_obj%.lower()
%$$
$icode%upper%      = %remove_obj%.upper()
%$$
$icode%var%        = %remove_obj%.remove(%both%)
%$$
$icode%both%       = %remove_obj%.restore(%var%)
%$$
$icode%var_index%  = %remove_obj%.both2var_index(%var%)
%$$
$icode%both_index% = %remove_obj%.var2both_index(%var%)
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

$head both_index$$
This has size $icode n_var$$ and is a mapping from the index in $icode var$$
to the corresponding index in $icode both$$.
It is monotone increasing; i.e., for $icode%i_var% < %n_var% - 1%$$,
$codei%
	%both_index%[%i_var%] < %both_index%[%i_var% + 1]
%$$

$head var_index$$
This has size $icode n_both$$. If
$codei%
	%lower_bound%[%i_both%] == %upper_bound%[%i_both%]
%$$
$codei%%var_index%[%i_both%]%$$ is equal to $icode n_both$$. Otherwise
it is a mapping from the index in $icode both$$
to the corresponding index in $icode var$$; i.e.,
$codei%
	%both_index%[ %var_index%[ %i_both% ] ] == %i_both%
%$$

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
CppAD::vector<size_t>       var2both_index_;
CppAD::vector<size_t>       both2var_index_;
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
const CppAD::vector<size_t>& var2both_index(void) const;
const CppAD::vector<size_t>& both2var_index(void) const;
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
	size_t m_var = n_var();
	CppAD::vector<Scalar> var(m_var);
	for(size_t var_index = 0; var_index < m_var; ++var_index)
		var[var_index] = both[ var2both_index_[var_index] ];
	return var;
}
// restore
template <class Scalar> CppAD::vector<Scalar>
remove_const::restore(const CppAD::vector<Scalar>& var) const
{	assert( var.size() == n_var() );
	CppAD::vector<Scalar> both( n_both_ );
	for(size_t both_index = 0; both_index < n_both_; ++both_index)
	{	if( both2var_index_[both_index] < n_both_ )
			both[both_index] = var[ both2var_index_[both_index] ];
		else
		{	both[both_index] = lower_bound_[both_index];
			assert( lower_bound_[both_index] == upper_bound_[both_index] );
		}
	}
	return both;
}

} // END_DISMOD_AT_NAMESPACE

# endif
