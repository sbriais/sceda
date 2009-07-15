/*
   Copyright Sebastien Briais 2008, 2009

   This file is part of SCEDA.
   
   SCEDA is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
   
   SCEDA is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with SCEDA.  If not, see
   <http://www.gnu.org/licenses/>.
*/
#include <string.h>

/** Hash function for strings. 

    @param[in] s = string to hash

    @return hash code */
unsigned int SCEDA_string_hash(const char *ptr) {
  int val = 0;

  while(*ptr != '\0') {
    int tmp;
    val = (val << 4) + (*ptr);
    if((tmp = (val & 0xf0000000))) {
      val = val ^ (tmp >> 24);
      val = val ^ tmp;
    }
    ptr++;
  }

  return (unsigned int)val;
}

/** Equality function for strings. 

    @param[in] s1 = first string 
    @param[in] s2 = second string 

    @return TRUE iff the two strings are equal */
int SCEDA_string_match(const char *s1, const char *s2) {
  return (strcmp(s1, s2) == 0);
}
