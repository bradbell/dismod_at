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
$begin smooth_info_ctor$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_info: Constructor$$

$head Syntax$$
$codei%smooth_info %s_info%(
   %smooth_id%,
   %age_table%,
   %time_table%,
   %prior_table%,
   %smooth_table%,
   %smooth_grid_table%
)%$$
$codei%smooth_info %s_default%()
%$$
$icode%s_target% = %s_source%
%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_NORMAL_CTOR%// END_NORMAL_CTOR%1
%$$
$srcthisfile%
   0%// BEGIN_DEFAULT_CTOR%// END_DEFAULT_CTOR%1
%$$
$srcthisfile%
   0%// BEGIN_ASSIGNMENT_CTOR%// END_ASSIGNMENT_CTOR%1
%$$

$comment OMhelp is soaking up to much line spacing before next heading$$
$pre

$$

$head Checks$$

$subhead Rectangular Grid$$
Checks the $code smooth_grid$$ table
$cref/rectangular grid/smooth_grid_table/Rectangular Grid/$$ assumption.

$subhead n_age, n_time$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
check that the $cref smooth_table$$ values
$cref/n_age/smooth_table/n_age/$$ and
$cref/n_time/smooth_table/n_time/$$ are the number if age values
and number of time values in the corresponding rectangular grid in
$cref smooth_grid_table$$.

$head smooth_id$$
Is the $cref/smooth_id/smooth_table/smooth_id/$$ for this smoothing.

$head age_table$$
is the $cref/age_table/get_age_table/age_table/$$
returned by $code get_age_table$$.

$head time_table$$
is the $cref/time_table/get_time_table/time_table/$$
returned by $code get_time_table$$.

$head prior_table$$
is the $cref/prior_table/get_prior_table/prior_table/$$
returned by $code get_prior_table$$.

$head smooth_table$$
is the $cref/smooth_table/get_smooth_table/smooth_table/$$
returned by $code get_smooth_table$$.

$head smooth_grid_table$$
is the $cref/smooth_grid/get_smooth_grid/smooth_grid/$$
returned by $code get_smooth_grid_table$$.

$head s_default$$
This is the result of the default constructor.
It can be creates an empty $code smooth_info$$ object
(no age or time points)
that can later be set equal to another $code smooth_info$$ object.
This is useful when creating vectors of such objects.

$head s_target$$
The assignment sets the target smoothing information object
to be a copy of the source object.

$end
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
$begin smooth_info_age_and_time$$
$spell
   const
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_info: Age and Time Information$$

$head Syntax$$
$icode%n_age%     = %s_info%.age_size()
%$$
$icode%n_time%    = %s_info%.time_size()
%$$
$icode%a_id%      = %s_info%.age_id(%i%)
%$$
$icode%t_id%      = %s_info%.time_id(%j%)
%$$

$head s_info$$
this object has prototype
$codei%
   const smooth_info %s_info%
%$$

$head n_age$$
This result has prototype
$codei%
   size_t %n_age%
%$$
and is the number of age values for this smoothing.

$head n_time$$
This result has prototype
$codei%
   size_t %n_time%
%$$
and is the number of time values for this smoothing.

$head i$$
The argument $icode i$$ has prototype
$codei%
   size_t %i%
%$$
and is the age index; $icode%i% < %n_age%$$.

$head j$$
The argument $icode j$$ has prototype
$codei%
   size_t %j%
%$$
and is the time index; $icode%j% < %n_time%$$.

$head a_id$$
This return value has prototype
$codei%
   size_t %a_id%
%$$
and is the $th i$$ $cref/age_id/smooth_grid_table/age_id/$$
for this smoothing and increases with $icode i$$; i.e.,
for $icode%i% < %n_age%-2%$$
$codei%
   %s_info%.age_id(%i%) < %s_info%.age_id(%i%+1)
%$$

$head t_id$$
This return value has prototype
$codei%
   size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/smooth_grid_table/time_id/$$
for this smoothing and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2%$$
$codei%
   %s_info%.time_id(%j%) < %s_info%.time_id(%j%+1)
%$$

$end
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
$begin smooth_info_prior$$
$spell
   dage
   dtime
   const
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_info: Prior Information$$

$head Syntax$$
$icode%const_value%     = %s_info%.const_value(%i%, %j%)
%$$
$icode%value_prior_id%  = %s_info%.value_prior_id(%i%, %j%)
%$$
$icode%dage_prior_id%  = %s_info%.dage_prior_id(%i%, %j%)
%$$
$icode%dtime_prior_id%  = %s_info%.dtime_prior_id(%i%, %j%)
%$$
$icode%all_const_value% = %s_info%.all_const_value()
%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_CONST_VALUE%// END_CONST_VALUE%1
%$$
$srcthisfile%
   0%// BEGIN_VALUE_PRIOR_ID%// END_VALUE_PRIOR_ID%1
%$$
$srcthisfile%
   0%// BEGIN_DAGE_PRIOR_ID%// END_DAGE_PRIOR_ID%1
%$$
$srcthisfile%
   0%// BEGIN_DTIME_PRIOR_ID%// END_DTIME_PRIOR_ID%1
%$$
$srcthisfile%
   0%// BEGIN_ALL_CONST_VALUE%// END_ALL_CONST_VALUE%1
%$$

$head i$$
is the index in the smooth grid of an age point;
$icode%i% < %s_info%.age_size()%$$.

$head j$$
is the index in the smooth grid of a time point;
$icode%j% < %s_info%.time_size()%$$.

$head const_value$$
is the $cref/const_value/smooth_grid_table/const_value/$$
corresponding to this $icode i$$, $icode j$$.

$head value_prior_id$$
is the $cref/prior_id/prior_table/prior_id/$$
for the value prior for this smoothing.
Either $icode const_value$$ is null (nan)
or $icode value_prior_id$$ is null
but not both.

$head dage_prior_id$$
is the $cref/prior_id/prior_table/prior_id/$$
for the dage prior for this smoothing
corresponding to this $icode i$$, $icode j$$.
If $icode%i% == %s_info%.age_size()-1%$$ then
$icode dage_prior_id$$ is $code DISMOD_AT_NULL_SIDE_T$$
(this may not be true in for the corresponding smooth_grid table values).

$head dtime_prior_id$$
is the $cref/prior_id/prior_table/prior_id/$$
for the dtime prior for this smoothing
corresponding to this $icode i$$, $icode j$$.
If $icode%j% == %s_info%.time_size()-1%$$ then
$icode dtime_prior_id$$ is $code DISMOD_AT_NULL_SIDE_T$$
(this may not be true in for the corresponding smooth_grid table values).

$head all_const_value$$
is true if all the value priors are equivalent to a $icode const_value$$; i.e.,
$icode const_value$$ is not null or the upper and lower limit for
$icode value_prior_id$$ are equal.

$end
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
$begin smooth_info_mulstd$$
$spell
   mulstd
   dage
   dtime
   const
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_info: Standard Deviation Multipliers$$

$head Syntax$$
$icode%mulstd_value_prior_id% = %s_info%.mulstd_value()
%$$
$icode%mulstd_dage_prior_id% = %s_info%.mulstd_dage()
%$$
$icode%mulstd_dtime_prior_id% = %s_info%.mulstd_dtime()
%$$

$head s_info$$
this object has prototype
$codei%
   const smooth_info %s_info%
%$$

$head mulstd_value_prior_id$$
is the $cref/mulstd_value_prior_id/smooth_table/mulstd_value_prior_id/$$
for this smoothing.

$head mulstd_dage_prior_id$$
is the $cref/mulstd_dage_prior_id/smooth_table/mulstd_dage_prior_id/$$
for this smoothing.

$head mulstd_dtime_prior_id$$
is the $cref/mulstd_dtime_prior_id/smooth_table/mulstd_dtime_prior_id/$$
for this smoothing.

$end
*/
size_t smooth_info::mulstd_value(void) const
{  return mulstd_value_; }
size_t smooth_info::mulstd_dage(void)  const
{  return mulstd_dage_; }
size_t smooth_info::mulstd_dtime(void) const
{  return mulstd_dtime_; }
/*
-------------------------------------------------------------------------------
$begin smooth_info_test_ctor$$
$spell
   dage
   dtime
   mulstd
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_info: Testing Constructor$$
$spell
   const
$$

$head Syntax$$
$codei%smooth_info %s_test%(
   %age_table%,
   %time_table%,
   %age_id%,
   %time_id%,
   %value_prior_id%,
   %dage_prior_id%,
   %dtime_prior_id%,
   %const_value%,
   %mulstd_value_prior_id%,
   %mulstd_dage_prior_id%,
   %mulstd_dtime_prior_id%
)
%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_TEST_CTOR%// END_TEST_CTOR%1
%$$

$head age_table$$
and is the $cref/age_table/get_age_table/$$.

$head time_table$$
and is the $cref/time_table/get_time_table/$$.

$head age_id$$
This specifies the age grid indices; i.e.
$codei%
   %s_info%.age_id(%i%) = %age_id%[%i%]
%$$

$head time_id$$
This specifies the time grid indices; i.e.
$codei%
   %s_info%.time_id(%j%) = %time_id%[%j%]
%$$

$head type_prior_id$$
For $icode type$$ equal to
$code const$$, $code value$$, $code dage$$, $code dtime$$:
$codei%
   %s_info%.%type%_prior_id(%i%, %j%) = %type_id%[ %i% * %n_time% + %j%]
%$$
where $icode%n_time% = %s_info%.time_size()%$$.

$head %mulstd_prior_id$$
For $icode mulstd_prior_id$$ equal to
$code mulstd_value_prior_id$$,
$code mulstd_dage_prior_id$$,
$code mulstd_dtime_prior_id$$:
$codei%
   %s_info%.%mulstd_prior_id%() = %mulstd_prior_id%
%$$

$children%example/devel/table/smooth_info_xam.cpp
%$$
$head Example$$
The file $cref smooth_info_xam.cpp$$ contains an example that uses
this function.

$end
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
