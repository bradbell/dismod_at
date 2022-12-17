# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin unicode_tuple.py$$ $newlinech #$$
# $spell
#  unicode_tuple
# $$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section unicode_tuple: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
#
def unicode_tuple() :
   import dismod_at
   #
   iterable     = [ 1, 2.00, 'three' ]
   quote_string = True
   u_tuple      = dismod_at.unicode_tuple(iterable, quote_string)
   check        = u'( 1, 2.0, \'three\' )'
   assert u_tuple == check
   #
   print('unicode_tuple: OK')
# END PYTHON
