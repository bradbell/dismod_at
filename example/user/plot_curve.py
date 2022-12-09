# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_plot_curve.py}
# {xrst_spell
#     lambsa
# }
# {xrst_comment_ch #}
#
# Example Plotting Log-Scaled Values w.r.t Age and Time
# #####################################################
#
# z_name
# ******
# For this example the name of the z variables are ``A`` , ``B`` ,
# ``C`` , and ``D``
#
# Age Grid
# ********
# The age grid for this example in 0.0, 5.0, ..., 100.0.
#
# Time Grid
# *********
# The time grid for this example is 1980.0, 1985.0, ..., 2020.0.
#
# Function
# ********
# The function for this example is
#
# .. math::
#
#    f(a, t) = \lambda * \exp \left[
#        - \left( \frac{a - 50}{100} \right)^2
#        - \left( \frac{(t - 2000.0}{20.0} \right)^2
#  \right]
#
# where :math:`a` is age, :math:`t` is time, and
# :math:`lambsa` is 1, 2, 3, 4 for variables
# ``A`` , ``B`` , ``C`` , ``D`` respectively.
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_plot_curve.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import string
import math
test_program = 'example/user/plot_curve.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# fun
def fun(a, t, z_name) :
   index     = ord(z_name) - ord('A')
   factor    = index + 1
   a_scaled  = (a - 50.0) / 100.0
   t_scaled  = (t - 2000.0) / 20.0
   quad      = a_scaled**2 + t_scaled**2
   z         = factor * math.exp( - quad )
   return z
#
# plot_data
plot_data = dict()
for z_name in [ 'A', 'B', 'C', 'D' ] :
   plot_data[z_name] = list()
   for age in range(0, 101, 5) :
      for time in range(1980, 2021, 5) :
         value = fun(age, time, z_name)
         std   = value / 10.0
         row = { 'age': age, 'time': time, 'value': value, 'std':  std }
         plot_data[z_name].append( row )
#
# plot_limit
plot_limit = {
   'age_min': 0.0, 'age_max': 100.0, 'time_min': 1980.0, 'time_max': 2020.0
}
#
# plot_curve
pdf_file   = 'example.pdf'
plot_title = 'Example Curve Plot'
dismod_at.plot_curve( pdf_file, plot_title, plot_limit, plot_data)
# -----------------------------------------------------------------------------
print(f'Plot file: build/example/user/{pdf_file}')
print('plot_curve.py: OK')
# END PYTHON
