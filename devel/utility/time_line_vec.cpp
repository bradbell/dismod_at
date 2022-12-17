// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/time_line_vec.hpp>
# include <dismod_at/a1_double.hpp>

/*
$begin time_line_vec$$
$spell
   vec
   struct
   diff
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Vector of Time Lines For Averaging a Function of Age and Time$$

$head Syntax$$
$icode%near% = time_line_vec<%Float%>::near_equal(double %x%, double %y%)
%$$
$codei%time_line_vec %vec%(%age_grid%)
%$$
$icode%vec%.specialize(
   %age_lower%, %age_upper%, %time_lower%, %time_upper%
)
%$$
$icode%extend_grid% = %vec%.extend_grid()
%$$
$icode%sub_lower% = %vec%.sub_lower()
%$$
$icode%sub_upper% = %vec%.sub_upper()
%$$
$icode%vec%.add_point(%age_index%, %point%)
%$$
$icode%time_line% = %vec%.time_line(%age_index%)
%$$
$icode%time_diff% = %vec%.max_time_diff(%age_index%, %time_index%)
%$$
$icode%avg% = %vec%.age_time_avg()
%$$

$head Float$$
The type $icode Float$$ is $code double$$ or $cref a1_double$$.

$head time_point$$
This structure is defined in the $codei%time_line_vec<%Float%>%$$ class
as follows:
$codei%
   struct time_point {double time; double weight; Float value; };
%$$

$head near_equal$$
Checks if $icode x$$ and $icode y$$ are nearly equal
to multiple of numerical precision that is greater than 10
and less than 100.
The return value $icode near$$ is true, if the values are nearly equal
and false otherwise.

$head time_line_vec$$
This constructs the object $icode vec$$ for managing vectors of time lines.

$head age_grid$$
This vector specifies the grid for averaging w.r.t. age.
This vector is monotone increasing; i.e.,
$codei%
   %age_grid%[%j%] < %age_grid%[%j%+1]
%$$

$head specialize$$
This creates an extended age grid for averaging between
the specified lower and upper ages and times where
$codei%
   %age_lower%  <= %age_upper%
   %time_lower% <= %time_upper%
%$$
There is a time line for each sub grid point
and it is initialized as empty.

$head extend_grid$$
This return value is an extended age grid and is monotone increasing.
It include all the points in the original $icode age_grid$$
and the values $icode age_lower$$ and $icode age_upper$$.

$head sub_lower$$
This return value is the index in $icode extend_grid$$
where the sub grid starts;
$codei%
   true == near_equal( %age_lower%, %extend_grid%[ %sub_lower% ] )
%$$

$head sub_upper$$
This return value is the index in $icode extend_grid$$
where the sub grid ends;
$codei%
   true == near_equal( %age_upper%, %extend_grid%[ %sub_upper% ] )
%$$

$head age_index$$
This is the index, for the time line, in the extended age grid
$codei%
   %sub_lower% <= %age_index% <= %sub_upper%
%$$.

$head add_point$$
This adds a time point to the specified time line.

$subhead point$$
This is the time point that is added to the time line.
The value $icode%point%.value%$$ ($icode%point%.weight%$$)
is the value of the function (weighting)
that we are sampling at age $icode%extend_grid[%age_index%]%$$
and time $icode%point.time%$$.
The time must satisfy
$codei%
   %time_lower% <= %point%.time <= %time_upper%
%$$
In addition, two calls to $code add_point$$ cannot have the
same $icode age_index$$ and $icode%point%.time%$$.

$head time_line$$
This vector contains the points in the time line
that corresponds to the specified $icode age_index$$.
The vector monotone non-decreasing in time; i.e.,
$codei%
   %time_line%[%i%].time <= %time_line%[%i%+1].time
%$$

$head max_time_diff$$
This function find the maximum value for
$codei%
   %max_time_diff% = %time_line%[%time_index% + 1] - %time_line%[%time_index%]
%$$

$subhead age_index$$
The input value of this argument does not matter.
Upon return, it is the age index for the maximum time difference.

$subhead time_index$$
The input value of this argument does not matter.
Upon return, it is the time index for the maximum time difference.

$subhead max_diff$$
It the return value $icode%time_diff% > 0%$$,
it is the maximum time difference for the time lines in $icode vec$$.
In addition, $icode%time_index% > 0%$$ and
$codei%
   %max_time_diff% = %time_line%[%time_index%] - %time_line%[%time_index% - 1]
%$$
where $icode time_line$$ corresponds to $icode age_index$$.

$head age_time_avg$$
This uses the
$cref numeric_average$$ method to approximate the average
of the weighted sampled function
with respect to the specified age and time limits.
Each time line corresponding to
$codei%
   %sub_lower% <= %age_index% <= %sub_upper%
%$$.
must have a point with time nearly equal to
$icode time_lower$$ and a point with time nearly equal to $icode time_upper$$.
If the upper and lower time limits are nearly equal,
only one call to $code add_point$$ for each time line is necessary.


$children%example/devel/utility/time_line_vec_xam.cpp
%$$
$head Example$$
The file $cref time_line_vec_xam.cpp$$ contains an example and test
of using this routine.

$head Prototype$$
$srcthisfile%
   0%// BEGIN_NEAR_EQUAL_PROTOTYPE%// END_NEAR_EQUAL_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_TIME_LINE_VEC_PROTOTYPE%// END_TIME_LINE_VEC_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_SPECIALIZE_PROTOTYPE%// END_SPECIALIZE_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_ADD_POINT_PROTOTYPE%// END_ADD_POINT_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_EXTEND_GRID_PROTOTYPE%// END_EXTEND_GRID_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_SUB_LOWER_PROTOTYPE%// END_SUB_LOWER_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_SUB_UPPER_PROTOTYPE%// END_SUB_UPPER_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_TIME_LINE_PROTOTYPE%// END_TIME_LINE_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_MAX_TIME_DIFF_PROTOTYPE%// END_MAX_TIME_DIFF_PROTOTYPE%1
%$$
$srcthisfile%
   0%// BEGIN_AGE_TIME_AVG_PROTOTYPE%// END_AGE_TIME_AVG_PROTOTYPE%1
%$$

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ---------------------------------------------------------------------------
// BEGIN_NEAR_EQUAL_PROTOTYPE
template <class Float>
bool time_line_vec<Float>::near_equal(double x, double y)
// END_NEAR_EQUAL_PROTOTYPE
{  // some constants for near equal calculations
   double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
   double min99 = 99.0 * std::numeric_limits<double>::min();
   //
   double abs_x = std::fabs(x);
   double abs_y = std::fabs(y);
   //
   if( abs_x <= min99 && abs_y <= min99 )
      return true;
   //
   return std::abs(1.0 - x / y) <= eps99;
}

// ---------------------------------------------------------------------------
// BEGIN_TIME_LINE_VEC_PROTOTYPE
template <class Float>
time_line_vec<Float>::time_line_vec(
   const CppAD::vector<double>& age_grid
)
// END_TIME_LINE_VEC_PROTOTYPE
: age_grid_(age_grid)
{
# ifndef NDEBUG
   assert( 2 <= age_grid_.size() );
   for(size_t j = 1; j < age_grid.size(); ++j)
      assert( age_grid_[j-1] < age_grid_[j] );
# endif
}

// ---------------------------------------------------------------------------
// BEGIN_SPECIALIZE_PROTOTYPE
template <class Float>
void time_line_vec<Float>::specialize(
   const double& age_lower  ,
   const double& age_upper  ,
   const double& time_lower ,
   const double& time_upper )
// END_SPECIALIZE_PROTOTYPE
{  assert( time_lower <= time_upper );
   //
   assert( age_lower <= age_upper );
   assert( age_grid_[0] <= age_lower );
   assert( age_upper <= age_grid_[age_grid_.size() - 1] );
   // -----------------------------------------------------------------
   double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
   // -----------------------------------------------------------------
   time_lower_ = time_lower;
   time_upper_ = time_upper;
   // -----------------------------------------------------------------
   // extend_grid_
   //
   size_t n_age     = age_grid_.size();
   size_t age_index = 0;
   extend_grid_.resize(0);
   //
   // ages < age_lower
   while( age_grid_[age_index] < (1.0 - eps99) * age_lower )
   {  extend_grid_.push_back( age_grid_[age_index] );
      ++age_index;
   }
   //
   // age_lower
   sub_lower_ = extend_grid_.size();
   sub_upper_ = sub_lower_;
   extend_grid_.push_back( age_lower );
   if( near_equal( age_grid_[age_index], age_lower ) )
      ++age_index;
   //
   if( ! near_equal( age_lower, age_upper ) )
   {  //
      // ages < age_upper
      assert( age_index < n_age );
      while( age_grid_[age_index] < (1.0 - eps99) * age_upper )
      {  extend_grid_.push_back( age_grid_[age_index] );
         ++age_index;
      }
      //
      // age_upper
      sub_upper_ = extend_grid_.size();
      extend_grid_.push_back( age_upper );
   }
   if( age_index < n_age && near_equal( age_grid_[age_index], age_upper ) )
      ++age_index;
   while( age_index < n_age )
   {  extend_grid_.push_back( age_grid_[age_index] );
      ++age_index;
   }
   // -----------------------------------------------------------------
   // vec_
   size_t n_sub = sub_upper_ - sub_lower_ + 1;
   vec_.resize(n_sub);
   for(size_t j = 0; j < n_sub; ++j)
      vec_[j].resize(0);
}
// ---------------------------------------------------------------------------
// BEGIN_EXTEND_GRID_PROTOTYPE
template <class Float>
const CppAD::vector<double>& time_line_vec<Float>::extend_grid(void) const
// END_EXTEND_GRID_PROTOTYPE
{  return extend_grid_; }

// ---------------------------------------------------------------------------
// BEGIN_SUB_LOWER_PROTOTYPE
template <class Float>
size_t time_line_vec<Float>::sub_lower(void) const
// END_SUB_LOWER_PROTOTYPE
{  return sub_lower_; }

// ---------------------------------------------------------------------------
// BEGIN_SUB_UPPER_PROTOTYPE
template <class Float>
size_t time_line_vec<Float>::sub_upper(void) const
// END_SUB_UPPER_PROTOTYPE
{  return sub_upper_; }


// ---------------------------------------------------------------------------
// BEGIN_ADD_POINT_PROTOTYPE
template <class Float>
void time_line_vec<Float>::add_point(
   const size_t&     age_index ,
   const time_point& point     )
// END_ADD_POINT_PROTOTYPE
{  assert( time_lower_ <= point.time || near_equal(time_lower_, point.time) );
   assert( point.time <= time_upper_ || near_equal(time_upper_, point.time) );
   assert( sub_lower_ <= age_index );
   assert( age_index <= sub_upper_ );
   //
   // this time line
   CppAD::vector<time_point>& time_line = vec_[age_index - sub_lower_];
   size_t n_time = time_line.size();
   //
   // time index at which to insert this point
   size_t time_index = 0;
   while( time_index < n_time && time_line[time_index].time < point.time )
      ++time_index;
   //
   // case where this point goes at the end of the line
   if( time_index == n_time )
   {  time_line.push_back(point);
      return;
   }
   //
   // make sure two calls do not have the same time
   assert( ! near_equal( point.time, time_line[time_index].time ) );
   //
   // case where this point is inserted at time_index
   time_line.push_back( time_line[n_time-1] );
   for(size_t i = n_time - 1; i > time_index; --i)
      time_line[i] = time_line[i-1];
   time_line[time_index] = point;
   //
   return;
}

// ---------------------------------------------------------------------------
// BEGIN_TIME_LINE_PROTOTYPE
template <class Float>
const CppAD::vector<typename time_line_vec<Float>::time_point>&
time_line_vec<Float>::time_line(const size_t& age_index) const
// END_TIME_LINE_PROTOTYPE
{
   assert( sub_lower_ <= age_index );
   assert( age_index <= sub_upper_ );
   return vec_[age_index - sub_lower_];
}
// ---------------------------------------------------------------------------
// BEGIN_MAX_TIME_DIFF_PROTOTYPE
template <class Float>
double time_line_vec<Float>::max_time_diff(
   size_t& age_index, size_t& time_index
) const
// END_MAX_TIME_DIFF_PROTOTYPE
{  double max_diff   = 0.0;
   size_t n_sub = sub_upper_ - sub_lower_ + 1;
   for(size_t i = 0; i < n_sub; ++i)
   {  const CppAD::vector<time_point>& time_line = vec_[i];
      size_t n_time = time_line.size();
      if( n_time > 1 )
      {  for(size_t j = 1; j < n_time; ++j)
         {  double diff = time_line[j].time - time_line[j-1].time;
            if( diff > max_diff )
            {  age_index  = sub_lower_ + i;
               time_index = j;
               max_diff   = diff;
            }
         }
      }
   }
   return max_diff;
}

// ---------------------------------------------------------------------------
// BEGIN_AGE_TIME_AVG_PROTOTYPE
template <class Float>
Float time_line_vec<Float>::age_time_avg(void) const
// END_AGE_TIME_AVG_PROTOTYPE
{  size_t n_sub = sub_upper_ - sub_lower_ + 1;
   //
   // compute average w.r.t time for each age
   CppAD::vector<double> sum_w(n_sub);
   CppAD::vector<Float>  sum_wv(n_sub);
   for(size_t i = 0; i < n_sub; ++i)
   {  const CppAD::vector<time_point>& line( vec_[i] );
      size_t n_time = line.size();
      //
      assert( n_time >= 1 );
      assert( near_equal( line[0].time, time_lower_ ) );
      assert( near_equal( line[n_time - 1].time, time_upper_ ) );
      //
      if( n_time == 1 )
      {  sum_w[i]  = line[0].weight;
         sum_wv[i] = line[0].weight * line[0].value;
      }
      else
      {
         sum_w[i] = 0.0;
         sum_wv[i]  = 0.0;
         for(size_t j = 1; j < n_time; ++j )
         {  double t_m = line[j-1].time;
            double w_m = line[j-1].weight;
            Float  v_m = line[j-1].value;
            //
            double t_j = line[j].time;
            double w_j = line[j].weight;
            Float  v_j = line[j].value;
            //
            sum_w[i]  += (t_j - t_m) * (w_j + w_m) / 2.0;
            sum_wv[i] += (t_j - t_m) * (w_j*v_j + w_m*v_m) / Float(2);
         }
      }
   }
   // compute average w.r.t age
   double weight(0);
   Float  result(0);
   if( n_sub == 1 )
      result = sum_wv[0] / sum_w[0];
   else
   {  for(size_t i = 1; i < n_sub; ++i)
      {  double w   = (sum_w[i] + sum_w[i-1]) / 2.0;
         Float  wv  = (sum_wv[i] + sum_wv[i-1]) / Float(2);
         size_t k   = i + sub_lower_;
         double da  = extend_grid_[k] - extend_grid_[k-1];
         weight    += w * da;
         result    += wv * da;
      }
      result /= weight;
   }
   return result;
}


// instantiation
template class time_line_vec<double>;
template class time_line_vec<a1_double>;


} // END_DISMOD_AT_NAMESPACE
