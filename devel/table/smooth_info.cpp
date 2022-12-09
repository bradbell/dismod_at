// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>

// ---------------------------------------------------------------------------
namespace {
   struct key_id {
      double key;
      size_t id;
   };
   void unique_insert_sort(
      CppAD::vector<key_id>&  vec     ,
      key_id&                 element )
   {  // initial number of elements in vec
      size_t n = vec.size();
      //
      // find where the new element belongs
      size_t i = 0;
      while( i < n && vec[i].key < element.key )
      {  // make sure note skipping past this element
         assert( vec[i].id != element.id );
         i++;
      }
      if( i == n )
      {  // put this element at the end of vec
         vec.push_back(element);
         return;
      }
      // check for case where this element is already in vec
      if( vec[i].id == element.id )
      {  assert( vec[i].key == element.key );
         return;
      }
      //
      // case where this element needs to be inserted at slot i
      assert( element.key < vec[i].key );
      vec.push_back( vec[n-1] );
      size_t j = n - 1;
      while( j > i )
      {  vec[j] = vec[j-1];
         j--;
      }
      vec[i] = element;
   }
}

// ---------------------------------------------------------------------------
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
// ---------------------------------------------------------------------------
/*
------------------------------------------------------------------------------
{xrst_begin smooth_info_ctor}

smooth_info: Constructor
########################

Syntax
******

| ``smooth_info`` *s_info* (
| |tab| *smooth_id* ,
| |tab| *age_table* ,
| |tab| *time_table* ,
| |tab| *prior_table* ,
| |tab| *smooth_table* ,
| |tab| *smooth_grid_table*
| ) ``smooth_info`` *s_default* ()
| *s_target* = *s_source*

Prototype
*********
{xrst_literal
   // BEGIN_NORMAL_CTOR
   // END_NORMAL_CTOR
}
{xrst_literal
   // BEGIN_DEFAULT_CTOR
   // END_DEFAULT_CTOR
}
{xrst_literal
   // BEGIN_ASSIGNMENT_CTOR
   // END_ASSIGNMENT_CTOR
}

{xrst_comment OMhelp is soaking up to much line spacing before next heading}

Checks
******

Rectangular Grid
================
Checks the ``smooth_grid`` table
:ref:`smooth_grid_table@Rectangular Grid` assumption.

n_age, n_time
=============
For each :ref:`smooth_table@smooth_id` ,
check that the :ref:`smooth_table-name` values
:ref:`smooth_table@n_age` and
:ref:`smooth_table@n_time` are the number if age values
and number of time values in the corresponding rectangular grid in
:ref:`smooth_grid_table-name` .

smooth_id
*********
Is the :ref:`smooth_table@smooth_id` for this smoothing.

age_table
*********
is the :ref:`get_age_table@age_table`
returned by ``get_age_table`` .

time_table
**********
is the :ref:`get_time_table@time_table`
returned by ``get_time_table`` .

prior_table
***********
is the :ref:`get_prior_table@prior_table`
returned by ``get_prior_table`` .

smooth_table
************
is the :ref:`get_smooth_table@smooth_table`
returned by ``get_smooth_table`` .

smooth_grid_table
*****************
is the :ref:`get_smooth_grid@smooth_grid`
returned by ``get_smooth_grid_table`` .

s_default
*********
This is the result of the default constructor.
It can be creates an empty ``smooth_info`` object
(no age or time points)
that can later be set equal to another ``smooth_info`` object.
This is useful when creating vectors of such objects.

s_target
********
The assignment sets the target smoothing information object
to be a copy of the source object.

{xrst_end smooth_info_ctor}
*/
// BEGIN_NORMAL_CTOR
smooth_info::smooth_info(
   size_t                                   smooth_id         ,
   const CppAD::vector<double>&             age_table         ,
   const CppAD::vector<double>&             time_table        ,
   const CppAD::vector<prior_struct>&       prior_table       ,
   const CppAD::vector<smooth_struct>&      smooth_table      ,
   const CppAD::vector<smooth_grid_struct>& smooth_grid_table )
// END_NORMAL_CTOR
{  size_t id;
   using std::string;
   using CppAD::to_string;
   //
   // for erorr messaging
   string msg, table_name;
   size_t row_id;
   size_t null_size_t = DISMOD_AT_NULL_SIZE_T;

   // only priors in the smooth_table are the multiplier priors
   mulstd_value_   = smooth_table[smooth_id].mulstd_value_prior_id;
   mulstd_dage_    = smooth_table[smooth_id].mulstd_dage_prior_id;
   mulstd_dtime_   = smooth_table[smooth_id].mulstd_dtime_prior_id;

   // determine the age_id_ and time_id_ vectors for this smooth_id
   assert( age_id_.size() == 0 );
   assert( time_id_.size() == 0 );
   CppAD::vector<key_id> age_vec, time_vec;
   size_t n_smooth_grid = smooth_grid_table.size();
   for(size_t i = 0; i < n_smooth_grid; i++)
   {  if( smooth_grid_table[i].smooth_id == int( smooth_id ) )
      {  key_id element;
         //
         element.id   = smooth_grid_table[i].age_id;
         element.key  = age_table[element.id];
         unique_insert_sort( age_vec,  element );
         //
         element.id   = smooth_grid_table[i].time_id;
         element.key  = time_table[element.id];
         unique_insert_sort( time_vec,  element );
      }
   }
   // number of age and time points for this smoothing
   size_t n_age  = age_vec.size();
   if( n_age != size_t( smooth_table[smooth_id].n_age ) )
   {  table_name = "smooth";
      msg  = "In smooth_grid table with smooth_id = " + to_string(smooth_id);
      msg += "\nnumber of unique age values is " + to_string(n_age) + ".\n";
      msg +=  "In smooth table, corresponding n_age = ";
      msg += to_string( smooth_table[smooth_id].n_age ) + ".";
      error_exit(msg, table_name, smooth_id);
   }
   size_t n_time = time_vec.size();
   if( n_time != size_t( smooth_table[smooth_id].n_time ) )
   {  table_name = "smooth";
      msg  = "In smooth_grid table with smooth_id = " + to_string(smooth_id);
      msg += "\nnumber of unique time values is " + to_string(n_time) + ".\n";
      msg +=  "In smooth table, corresponding n_time = ";
      msg += to_string( smooth_table[smooth_id].n_time ) + ".";
      error_exit(msg, table_name, smooth_id);
   }

   // age ids in order of increasing age for this smoothing
   age_id_.resize(n_age);
   for(size_t i = 0; i < n_age; i++)
      age_id_[i] = age_vec[i].id;
   // time ids in order of increasing time for this smoothing
   time_id_.resize(n_time);
   for(size_t j = 0; j < n_time; j++)
      time_id_[j] = time_vec[j].id;

   // set smoothing priors and count number of times each
   // age, time pair appears for this smooth_id
   CppAD::vector<size_t> count(n_age * n_time );
   value_prior_id_.resize( n_age * n_time );
   dage_prior_id_.resize ( n_age * n_time );
   dtime_prior_id_.resize( n_age * n_time );
   const_value_.resize   ( n_age * n_time );
   for(size_t i = 0; i < n_age * n_time; i++)
      count[i] = 0;
   for(size_t i = 0; i < n_smooth_grid; i++)
   {  if( smooth_grid_table[i].smooth_id == int( smooth_id ) )
      {  id           = smooth_grid_table[i].age_id;
         size_t j_age = n_age;
         for(size_t j = 0; j < n_age; j++ )
            if( id == age_id_[j] )
               j_age = j;
         assert( j_age < n_age );
         //
         id            = smooth_grid_table[i].time_id;
         size_t j_time = n_time;
         for(size_t j = 0; j < n_time; j++ )
            if( id == time_id_[j] )
               j_time = j;
         assert( j_time < n_time );
         size_t index = j_age * n_time + j_time;
         count[index]++;
         //
         value_prior_id_[index] = smooth_grid_table[i].value_prior_id;
         dage_prior_id_[index]  = smooth_grid_table[i].dage_prior_id;
         dtime_prior_id_[index] = smooth_grid_table[i].dtime_prior_id;
         const_value_[index]    = smooth_grid_table[i].const_value;
         //
         // set dage_prior_id_ to null for the last age and time
         // (it is not used)
         if( j_age == n_age - 1)
            dage_prior_id_[index] = null_size_t;
         if( j_time == n_time - 1)
             dtime_prior_id_[index] = null_size_t;
      }
   }

   // make sure each age, time pair appears once
   for(size_t i = 0; i < n_age * n_time; i++)
   {  if( count[i] != 1 )
      {  size_t j_time = i % n_time;
         size_t j_age  = (i - j_time) / n_time;
         table_name = "smooth_grid";
         row_id     = null_size_t;
         msg  = "smooth_id = " + to_string( smooth_id );
         msg += ", age_id = " + to_string( age_id_[j_age] );
         msg += ", time_id = " + to_string( time_id_[j_time] );
         msg += ": appears " + to_string(count[i]) + " times";
         msg += " (should be one time). ";
         error_exit(msg, table_name, row_id);
      }
   }

   // all_const_value_
   all_const_value_ = true;
   for(size_t i = 0; i < n_age; ++i)
   {  for(size_t j = 0; j < n_time; ++j)
      {  double value = const_value_[ i * n_time + j ];
         if( std::isnan(value) )
         {  size_t prior_id = value_prior_id_[ i * n_time + j ];
            assert( prior_id != DISMOD_AT_NULL_SIZE_T );
            double lower = prior_table[prior_id].lower;
            double upper = prior_table[prior_id].upper;
            assert( lower <= upper );
            all_const_value_ &= lower == upper;
         }
      }
   }
}

// BEGIN_DEFAULT_CTOR
smooth_info::smooth_info(void)
// END_DEFAULT_CTOR
:
age_id_(0),
time_id_(0),
value_prior_id_(0),
dage_prior_id_(0),
dtime_prior_id_(0),
const_value_(0),
mulstd_value_(0),
mulstd_dage_(0),
mulstd_dtime_(0)
{ }

// BEGIN_ASSIGNMENT_CTOR
void smooth_info::operator=(const smooth_info& s_info)
// END_ASSIGNMENT_CTOR
{  age_id_          = s_info.age_id_;
   time_id_         = s_info.time_id_;
   value_prior_id_  = s_info.value_prior_id_;
   dage_prior_id_   = s_info.dage_prior_id_;
   dtime_prior_id_  = s_info.dtime_prior_id_;
   const_value_     = s_info.const_value_;
   mulstd_value_    = s_info.mulstd_value_;
   mulstd_dage_     = s_info.mulstd_dage_;
   mulstd_dtime_    = s_info.mulstd_dtime_;
   all_const_value_ = s_info.all_const_value_;
}
/*
------------------------------------------------------------------------------
{xrst_begin smooth_info_age_and_time}

smooth_info: Age and Time Information
#####################################

Syntax
******

| *n_age* = *s_info* . ``age_size`` ()
| *n_time* = *s_info* . ``time_size`` ()
| *a_id* = *s_info* . ``age_id`` ( *i* )
| *t_id* = *s_info* . ``time_id`` ( *j* )

s_info
******
this object has prototype

   ``const smooth_info`` *s_info*

n_age
*****
This result has prototype

   ``size_t`` *n_age*

and is the number of age values for this smoothing.

n_time
******
This result has prototype

   ``size_t`` *n_time*

and is the number of time values for this smoothing.

i
*
The argument *i* has prototype

   ``size_t`` *i*

and is the age index; *i* < *n_age* .

j
*
The argument *j* has prototype

   ``size_t`` *j*

and is the time index; *j* < *n_time* .

a_id
****
This return value has prototype

   ``size_t`` *a_id*

and is the *i*-th :ref:`smooth_grid_table@age_id`
for this smoothing and increases with *i* ; i.e.,
for *i* < *n_age* ``-2``

   *s_info* . ``age_id`` ( *i* ) < *s_info* . ``age_id`` ( *i* +1)

t_id
****
This return value has prototype

   ``size_t`` *t_id*

and is the *j*-th :ref:`smooth_grid_table@time_id`
for this smoothing and increases with *j* ; i.e.,
for *j* < *n_time* ``-2``

   *s_info* . ``time_id`` ( *j* ) < *s_info* . ``time_id`` ( *j* +1)

{xrst_end smooth_info_age_and_time}
*/
size_t smooth_info::age_size(void) const
{  return age_id_.size(); }
size_t smooth_info::time_size(void) const
{  return time_id_.size(); }
//
size_t smooth_info::age_id(size_t i) const
{  return age_id_[i]; }
size_t smooth_info::time_id(size_t j) const
{  return time_id_[j]; }
/*
-------------------------------------------------------------------------------
{xrst_begin smooth_info_prior}

smooth_info: Prior Information
##############################

Syntax
******

| *const_value* = *s_info* . ``const_value`` ( *i* , *j* )
| *value_prior_id* = *s_info* . ``value_prior_id`` ( *i* , *j* )
| *dage_prior_id* = *s_info* . ``dage_prior_id`` ( *i* , *j* )
| *dtime_prior_id* = *s_info* . ``dtime_prior_id`` ( *i* , *j* )
| *all_const_value* = *s_info* . ``all_const_value`` ()

Prototype
*********
{xrst_literal
   // BEGIN_CONST_VALUE
   // END_CONST_VALUE
}
{xrst_literal
   // BEGIN_VALUE_PRIOR_ID
   // END_VALUE_PRIOR_ID
}
{xrst_literal
   // BEGIN_DAGE_PRIOR_ID
   // END_DAGE_PRIOR_ID
}
{xrst_literal
   // BEGIN_DTIME_PRIOR_ID
   // END_DTIME_PRIOR_ID
}
{xrst_literal
   // BEGIN_ALL_CONST_VALUE
   // END_ALL_CONST_VALUE
}

i
*
is the index in the smooth grid of an age point;
*i* < *s_info* . ``age_size`` () .

j
*
is the index in the smooth grid of a time point;
*j* < *s_info* . ``time_size`` () .

const_value
***********
is the :ref:`smooth_grid_table@const_value`
corresponding to this *i* , *j* .

value_prior_id
**************
is the :ref:`prior_table@prior_id`
for the value prior for this smoothing.
Either *const_value* is null (nan)
or *value_prior_id* is null
but not both.

dage_prior_id
*************
is the :ref:`prior_table@prior_id`
for the dage prior for this smoothing
corresponding to this *i* , *j* .
If *i* == *s_info* . ``age_size`` () ``-1`` then
*dage_prior_id* is ``DISMOD_AT_NULL_SIDE_T``
(this may not be true in for the corresponding smooth_grid table values).

dtime_prior_id
**************
is the :ref:`prior_table@prior_id`
for the dtime prior for this smoothing
corresponding to this *i* , *j* .
If *j* == *s_info* . ``time_size`` () ``-1`` then
*dtime_prior_id* is ``DISMOD_AT_NULL_SIDE_T``
(this may not be true in for the corresponding smooth_grid table values).

all_const_value
***************
is true if all the value priors are equivalent to a *const_value* ; i.e.,
*const_value* is not null or the upper and lower limit for
*value_prior_id* are equal.

{xrst_end smooth_info_prior}
*/
// BEGIN_CONST_VALUE
double smooth_info::const_value(size_t i, size_t j) const
// END_CONST_VALUE
{  assert( i < age_id_.size() );
   assert( j < time_id_.size() );
   return const_value_[ i * time_id_.size() + j];
}
// BEGIN_VALUE_PRIOR_ID
size_t smooth_info::value_prior_id(size_t i, size_t j) const
// END_VALUE_PRIOR_ID
{  assert( i < age_id_.size() );
   assert( j < time_id_.size() );
   return value_prior_id_[ i * time_id_.size() + j];
}
// BEGIN_DAGE_PRIOR_ID
size_t smooth_info::dage_prior_id(size_t i, size_t j) const
// END_DAGE_PRIOR_ID
{  assert( i < age_id_.size() );
   assert( j < time_id_.size() );
   return dage_prior_id_[ i * time_id_.size() + j];
}
// BEGIN_DTIME_PRIOR_ID
size_t smooth_info::dtime_prior_id(size_t i, size_t j) const
// END_DTIME_PRIOR_ID
{  assert( i < age_id_.size() );
   assert( j < time_id_.size() );
   return dtime_prior_id_[ i * time_id_.size() + j];
}
// BEGIN_ALL_CONST_VALUE
bool smooth_info::all_const_value(void) const
{  return all_const_value_; }
// END_ALL_CONST_VALUE
/*
------------------------------------------------------------------------------
{xrst_begin smooth_info_mulstd}

smooth_info: Standard Deviation Multipliers
###########################################

Syntax
******

| *mulstd_value_prior_id* = *s_info* . ``mulstd_value`` ()
| *mulstd_dage_prior_id* = *s_info* . ``mulstd_dage`` ()
| *mulstd_dtime_prior_id* = *s_info* . ``mulstd_dtime`` ()

s_info
******
this object has prototype

   ``const smooth_info`` *s_info*

mulstd_value_prior_id
*********************
is the :ref:`smooth_table@mulstd_value_prior_id`
for this smoothing.

mulstd_dage_prior_id
********************
is the :ref:`smooth_table@mulstd_dage_prior_id`
for this smoothing.

mulstd_dtime_prior_id
*********************
is the :ref:`smooth_table@mulstd_dtime_prior_id`
for this smoothing.

{xrst_end smooth_info_mulstd}
*/
size_t smooth_info::mulstd_value(void) const
{  return mulstd_value_; }
size_t smooth_info::mulstd_dage(void)  const
{  return mulstd_dage_; }
size_t smooth_info::mulstd_dtime(void) const
{  return mulstd_dtime_; }
/*
-------------------------------------------------------------------------------
{xrst_begin smooth_info_test_ctor}

smooth_info: Testing Constructor
################################

Syntax
******

| ``smooth_info`` *s_test* (
| |tab| *age_table* ,
| |tab| *time_table* ,
| |tab| *age_id* ,
| |tab| *time_id* ,
| |tab| *value_prior_id* ,
| |tab| *dage_prior_id* ,
| |tab| *dtime_prior_id* ,
| |tab| *const_value* ,
| |tab| *mulstd_value_prior_id* ,
| |tab| *mulstd_dage_prior_id* ,
| |tab| *mulstd_dtime_prior_id*
| )

Prototype
*********
{xrst_literal
   // BEGIN_TEST_CTOR
   // END_TEST_CTOR
}

age_table
*********
and is the :ref:`age_table<get_age_table-name>` .

time_table
**********
and is the :ref:`time_table<get_time_table-name>` .

age_id
******
This specifies the age grid indices; i.e.

   *s_info* . ``age_id`` ( *i* ) = *age_id* [ *i* ]

time_id
*******
This specifies the time grid indices; i.e.

   *s_info* . ``time_id`` ( *j* ) = *time_id* [ *j* ]

type_prior_id
*************
For *type* equal to
``const`` , ``value`` , ``dage`` , ``dtime`` :

   *s_info* . *type* _ ``prior_id`` ( *i* , *j* ) = *type_id* [ *i* * *n_time* + *j* ]

where *n_time* = *s_info* . ``time_size`` () .

%mulstd_prior_id
****************
For *mulstd_prior_id* equal to
``mulstd_value_prior_id`` ,
``mulstd_dage_prior_id`` ,
``mulstd_dtime_prior_id`` :

   *s_info* . *mulstd_prior_id* () = *mulstd_prior_id*

{xrst_toc_hidden
   example/devel/table/smooth_info_xam.cpp
}
Example
*******
The file :ref:`smooth_info_xam.cpp-name` contains an example that uses
this function.

{xrst_end smooth_info_test_ctor}
*/

// BEGIN_TEST_CTOR
smooth_info::smooth_info(
   const CppAD::vector<double>& age_table              ,
   const CppAD::vector<double>& time_table             ,
   const CppAD::vector<size_t>& age_id                 ,
   const CppAD::vector<size_t>& time_id                ,
   const CppAD::vector<size_t>& value_prior_id         ,
   const CppAD::vector<size_t>& dage_prior_id          ,
   const CppAD::vector<size_t>& dtime_prior_id         ,
   const CppAD::vector<double>& const_value            ,
   size_t                       mulstd_value_prior_id  ,
   size_t                       mulstd_dage_prior_id   ,
   size_t                       mulstd_dtime_prior_id  ,
   bool                         all_const_value        )
// END_TEST_CTOR
{  age_id_          = age_id;
   time_id_         = time_id;
   value_prior_id_  = value_prior_id;
   dage_prior_id_   = dage_prior_id;
   dtime_prior_id_  = dtime_prior_id;
   const_value_     = const_value;
   mulstd_value_    = mulstd_value_prior_id;
   mulstd_dage_     = mulstd_dage_prior_id;
   mulstd_dtime_    = mulstd_dtime_prior_id;
   all_const_value_ = all_const_value;
   //
   size_t n_age  = age_id.size();
   size_t n_time = time_id.size();
# ifndef NDEBUG
   for(size_t i = 1; i < n_age; i++)
      assert( age_table[age_id[i-1]] < age_table[age_id[i]] );
   for(size_t j = 1; j < n_time; j++)
      assert( time_table[time_id[j-1]] < time_table[time_id[j]] );
# endif
   for(size_t i = 0; i < n_age; ++i)
      dtime_prior_id_[ i * n_time + (n_time - 1)] = DISMOD_AT_NULL_SIZE_T;
   for(size_t j = 0; j < n_time; ++j)
      dage_prior_id_[ (n_age - 1) * n_time + j]   = DISMOD_AT_NULL_SIZE_T;
}

// ---------------------------------------------------------------------------
} // END_DISMOD_AT_NAMESPACE
// ---------------------------------------------------------------------------
