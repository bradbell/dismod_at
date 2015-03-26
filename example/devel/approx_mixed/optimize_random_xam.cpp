// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin optimize_random_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ optimize_random: Example and Test$$

$code
$verbatim%example/devel/approx_mixed/optimize_random_xam.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/optimize_random.hpp>

namespace {
	using CppAD::vector;
	using CppAD::log;
	using CppAD::AD;

	class Neglogden {
	private:
		const vector<double>& y_;
	public:
		// constructor
		Neglogden(const vector<double>& y) : y_(y)
		{ }

		// function object
		template <class Float>
		vector<Float> operator()(
			const vector<Float>& theta  ,
			const vector<Float>& u      )
		{	vector<Float> neglogden_vec(1);

			// initialize part of log-density that is alwasy smooth
			neglogden_vec[0] = Float(0.0);

			// compute this factor once
			Float sqrt_2 = CppAD::sqrt( Float(2.0) );

			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = u[i];
				Float sigma  = theta[i];
				Float res    = (y_[i] - mu) / sigma;

				if( i % 2 )
				{	// This is a Gaussian term, so entire density is smooth
					// (do not need 2*pi inside of log)
					neglogden_vec[0]  += (log(sigma) + res*res) / Float(2.0);
				}
				else
				{	// This term is Laplace distributed
					// (do not need sqrt(2) inside of log)
					neglogden_vec[0] += log(sigma);

					// part of the density that need absolute value
					neglogden_vec.push_back(sqrt_2 * res);
				}
			}
			return neglogden_vec;
		}
	};
}

bool optimize_random_xam(void)
{
	bool   ok = true;

	size_t n_data = 10;
	vector<double> data(n_data), fixed_vec(n_data), random_in(n_data);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]      = double(i + 1);
		fixed_vec[i] = 1.0;
		random_in[i] = 0.0;
	}

	// object that computes negative log-density
	Neglogden neglogden(data);

	// determine the optimal random effects
	vector<double> random_out = dismod_at::optimize_random(
		fixed_vec, random_in, neglogden
	);

	// check the result
	for(size_t i = 0; i < n_data; i++)
	{	// debugging print out
		// std::cout << random_out[i] / data[i] - 1.0 << std::endl;
		ok &= CppAD::abs(random_out[i] / data[i] - 1.0) < 1e-10;
	}

	return ok;
}
// END C++
