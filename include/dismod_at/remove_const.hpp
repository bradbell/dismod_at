// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_REMOVE_CONST_HPP
# define DISMOD_AT_REMOVE_CONST_HPP
/*
{xrst_begin remove_const dev}

Remove and Restore Components of a Vector that are Constant
###########################################################

Syntax
******

| ``remove_const`` *remove_obj* ( *lower_bound* , *upper_bound* )
| *n_both* = *remove_obj* . ``both`` ()
| *n_const* = *remove_obj* . ``n_const`` ()
| *n_var* = *remove_obj* . ``n_var`` ()
| *lower* = *remove_obj* . ``lower`` ()
| *upper* = *remove_obj* . ``upper`` ()
| *var* = *remove_obj* . ``remove`` ( *both* )
| *both* = *remove_obj* . ``restore`` ( *var* )
| *var_index* = *remove_obj* . ``both2var_index`` ( *var* )
| *both_index* = *remove_obj* . ``var2both_index`` ( *var* )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

lower_bound
***********
This is the lower bounds for the components of a the vector.

upper_bound
***********
This is the upper bounds for the components of a the vector
(it must have the same size as *lower_bound* ).

n_both
******
is the size of in the bound vectors

n_const
*******
is the number of components of *lower_bound*
that are equal to the corresponding component of *upper_bound*

n_var
*****
is the number of components of *lower_bound*
that are not equal to the corresponding component of *upper_bound*

lower
*****
is a ``const`` reference to a copy of *lower_bound*

upper
*****
is a ``const`` reference to a copy of *upper_bound*

both
****
is a vector with size *n_both* .

var
***
is a vector with size *n_var*
that only contains the values in *both* for which
the lower and upper bound are not equal.
The order of the elements in *var* is the same as their
order in *both* .

both_index
**********
This has size *n_var* and is a mapping from the index in *var*
to the corresponding index in *both* .
It is monotone increasing; i.e., for *i_var* < *n_var* ``- 1`` ,

   *both_index* [ *i_var* ] < *both_index* [ *i_var*  + 1]

var_index
*********
This has size *n_both* . If

   *lower_bound* [ *i_both* ] == *upper_bound* [ *i_both* ]

*var_index* [ *i_both* ] is equal to *n_both* . Otherwise
it is a mapping from the index in *both*
to the corresponding index in *var* ; i.e.,

   *both_index* [ *var_index* [ *i_both* ] ] == *i_both*

{xrst_end remove_const}
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
{  assert( both.size() == n_both_ );
   size_t m_var = n_var();
   CppAD::vector<Scalar> var(m_var);
   for(size_t var_index = 0; var_index < m_var; ++var_index)
      var[var_index] = both[ var2both_index_[var_index] ];
   return var;
}
// restore
template <class Scalar> CppAD::vector<Scalar>
remove_const::restore(const CppAD::vector<Scalar>& var) const
{  assert( var.size() == n_var() );
   CppAD::vector<Scalar> both( n_both_ );
   for(size_t both_index = 0; both_index < n_both_; ++both_index)
   {  if( both2var_index_[both_index] < n_both_ )
         both[both_index] = var[ both2var_index_[both_index] ];
      else
      {  both[both_index] = lower_bound_[both_index];
         assert( lower_bound_[both_index] == upper_bound_[both_index] );
      }
   }
   return both;
}

} // END_DISMOD_AT_NAMESPACE

# endif
