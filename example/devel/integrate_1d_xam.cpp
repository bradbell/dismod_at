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
$begin integrate_1d_xam.cpp$$
$spell
	xam
$$

$section C++ integrate_d1: Example and Test$$
$index example, C++ integrate_d1$$
$index integrate_d1, C++ example$$

$code
$verbatim%example/devel/integrate_1d_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <utility>
# include <limits>
# include <cmath>
# include <dismod_at/dismod_at.hpp>

namespace {
	double integral(double s, double a, double b)
	{	// w(s) = a * s + b
		// v(s) = b * s + a
		// v(s) * w(s) = a*b*s^2 + (a*a + b*b)*s + a*b
		// integral v(s)*w(s) = a*b*s^3/3 + (a*a+b*b)*s^2/2 + a*b*s
		double s2 = s * s / 2.0;
		double s3 = s * s * s / 3.0; 
		return a*b*s3 + (a*a+b*b)*s2 + a*b*s;
	}
}
bool integrate_1d_xam(void)
{
	bool   ok = true;
	double eps = std::numeric_limits<double>::epsilon();

	double a  = 0.2;
	double b  = 1.5;

	std::pair<double,double> q, r, v, w, c;
	//
	q.first  =  0.1;
	q.second =  1.0; 
	//
	r.first  =  0.2;
	r.second =  0.4;
	//
	w.first  =  a * r.first  + b; // w( r.first ) 
	w.second =  a * r.second + b; // w( r.second ) 
	//
	c = dismod_at::integrate_1d(q, r, w);
	//
	// compute the integral using the coefficients
	double v1 =  b * q.first  + a;
	double v2 =  b * q.second + a;
	double I  = c.first * v1 + c.second * v2;
	//
	double check = integral(r.second, a, b) - integral(r.first, a, b);
	ok = std::fabs( 1.0 - I / check ) < eps * 100;
	return ok;
}
// END C++
