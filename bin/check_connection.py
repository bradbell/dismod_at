#! /usr/bin/env python
# -----------------------------------------------------------------------------
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2025 Bradley M. Bell
# -----------------------------------------------------------------------------
#
# This program checks that each connection opened by create_connection
# is later close in the same file.
#
import sys
import os
import re
#
# start_pattern
start_pattern = re.compile( r'\n *([^\n ]*).*dismod_at[.]create_connection' )
#
# check_connection
def check_connection(file_path) :
   #
   # ok
   ok = True
   #
   # data
   file_obj  = open(file_path, 'r')
   data      = file_obj.read()
   #
   # m_start
   m_start = start_pattern.search(data)
   while m_start != None :
      #
      # variable, m_stop, ok
      variable     = m_start.group(1)
      stop_pattern = re.compile( r'\n *' + variable + r'.close[(][)]' )
      m_stop       = stop_pattern.search(data, m_start.end() )
      if m_stop == None :
         print( f'{file_path}: {variable}.close() not after create connection' )
         ok = False
      else :
         #
         # ok
         change_pattern = re.compile( r'\n *' + variable + ' *=' )
         m_change       = change_pattern.search(
            data, m_start.end(), m_stop.start(),
         )
         if m_change != None :
            print( f'{file_path}: {variable}.close() not before {variable} =' )
            ok = False
      #
      # m_start
      m_start      = start_pattern.search(data, m_start.end() )
   #
   return ok
#
def main() :
   ok = True
   for subdir in [ 'example', 'test', 'python/dismod_at', 'python/bin' ] :
      for root, dirs, files in os.walk(subdir) :
         for file in files :
            if file.endswith('.py') and not file.startswith('temp.') :
               file_path = os.path.join(root, file)
               if not check_connection(file_path) :
                  ok = False
   if not ok :
      sys.exit( 'check_connection.py: Error' )
   print( 'check_connection.py: OK' )
#
main()
