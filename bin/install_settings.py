#! /usr/bin/env python3
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2024 Bradley M. Bell
# ----------------------------------------------------------------------------
#
import os
import sys
import shutil
import subprocess
#
# {xrst_begin install_settings.py}
# {xrst_spell
#     cmake
#     cxx
#     eval
#     fortran
#     gcc
#     getcwd
#     gfortran
#     libdir
#     makefile
#     pthread
#     sys
# }
# {xrst_comment_ch #}
#
# bin/install_settings.py: User Configuration Options
# ###################################################
#
# Syntax
# ******
# The cases below must be executed from the directory that
# the dismod_at repository was cloned into; i.e., the top source directory.
#
# Python
# ======
# This sets install_settings to a python dictionary that has a key-value
# pair for of the settings listed below:
#
# .. code-block:: py
#
#     sys.path.insert(0, os.getcwd() + '/bin')
#     import install_settings
#     install_settings = install_settings.install_settings()
#     sys.path.pop(0)
#
# Bash
# ====
# This prints the value for each of the install settings:
#
# .. code-block:: sh
#
#     bin/install_settings.py
#
# This sets a bash variable to its value for each of the install settings:
#
# .. code-block:: sh
#
#     eval $(bin/install_settings.py)
#
# verbose_makefile
# ****************
# Use 'no' for normal and 'yes' for verbose make output:
# {xrst_code py}
verbose_makefile = 'no'
# {xrst_code}
#
# build_type
# **********
# Use either ``debug`` or ``release`` for the type of this build:
# {xrst_code py}
build_type = 'release'
# {xrst_code}
#
# dismod_at_prefix
# ****************
# Prefix where dismod_at will be installed:
# {xrst_code py}
dismod_at_prefix = os.environ['HOME'] + '/prefix/dismod_at'
# {xrst_code}
# If *dismod_at_prefix* ends in ``dismod_at`` ,
# ``install_settings.py`` will use a soft link from this prefix to
# *dismod_at_prefix* . ``debug`` or
# *dismod_at_prefix* . ``release``
# depending on the choice for *build_type* .
#
# Debug and Release
# =================
# If a soft link is used for the install,
# the same technique will be used to map the ``build``
# directory to the debug or release version.
# If you are using both a debug and release versions of dismod_at,
# both versions of the
# :ref:`install_unix@Special Requirements`
# will need to be installed.
#
# python3_executable
# ******************
# Path to the python3 executable on this machine.
# {xrst_code py}
python3_executable = sys.executable
# {xrst_code}
# You can use the command ``which python3`` to determine the location
# of the default version for this system.
#
# specific_compiler
# *****************
# On some systems, e.g. the Mac using port, there are problems with mixing
# different compiler systems for fortran and C++; see
# `ipopt issue 471 <https://github.com/coin-or/Ipopt/discussions/471>`_.
# This variable allows you to set a specific compiler for
# C, and or CXX and or FC. For example
# ``specific_compiler='CC=gcc CXX=g++ FC=gfortran'``
# uses the gnu versions of these compilers.
# The configuration will automatically find compilers that are not specified;
# i.e., if
# {xrst_code py}
specific_compiler = ''
# {xrst_code}
#
# extra_cxx_flags
# ***************
# Extra C++ flags used during compilation:
# {xrst_spell_off}
# {xrst_code py}
extra_cxx_flags  = '-std=c++17 -Wpedantic -Wall -Wshadow -Wconversion'
extra_cxx_flags += ' -Wno-bitwise-instead-of-logical'
if shutil.which('brew') != None :
   cmd    = [ 'brew', '--prefix' ]
   result = subprocess.run(cmd, capture_output=True, encoding='utf-8')
   if result.returncode == 0:
      brew_prefix      = result.stdout.strip()
      extra_cxx_flags += f' -Wno-sign-conversion -I {brew_prefix}/include'
# {xrst_code}
# {xrst_spell_on}
# An alternative might be ``-Wall`` .
#
# cmake_libdir
# ************
# Sub-directory of each prefix where libraries are installed.
# {xrst_code py}
cmake_libdir = 'lib64'
# {xrst_code}
#
# system_specific_library_list
# ****************************
# List of libraries that are needed for a particular system. For example,
# if when you build ``dismod_at`` the ``pthread`` library is
# required by your system, then include it here.
# Libraries in the list can be separated by spaces or semi-colons.
# {xrst_code py}
system_specific_library_list = ''
# {xrst_code}
#
# {xrst_end install_settings.py}
# ============================================================================
#
# msg
msg = 'install_settings.py: '
#
# build_type
if build_type not in [ 'debug', 'release' ] :
   msg += 'build_type is not debug or release'
   assert False, msg
#
# verbose_makefile
if verbose_makefile not in [ 'yes', 'no' ] :
   msg += 'verbose_makefile is not yes or no'
   assert False, msg
#
# install_settings
def install_settings() :
   settings = {
      'verbose_makefile'             : verbose_makefile              ,
      'build_type'                   : build_type                    ,
      'dismod_at_prefix'             : dismod_at_prefix              ,
      'python3_executalbe'           : python3_executalbe            ,
      'specific_compiler'            : specific_compiler             ,
      'extra_cxx_flags'              : extra_cxx_flags               ,
      'cmake_libdir'                 : cmake_libdir                  ,
      'system_specific_library_list' : system_specific_library_list  ,
   }
   return settings
#
# main
if __name__ == '__main__' :
   #
   # output
   output  = f"verbose_makefile='{verbose_makefile}'\n"
   output += f"build_type='{build_type}'\n"
   output += f"dismod_at_prefix='{dismod_at_prefix}'\n"
   output += f"python3_executable='{python3_executable}'\n"
   output += f"specific_compiler='{specific_compiler}'\n"
   output += f"extra_cxx_flags='{extra_cxx_flags}'\n"
   output += f"cmake_libdir='{cmake_libdir}'\n"
   output += f"system_specific_library_list='{system_specific_library_list}'\n"
   print(output)
