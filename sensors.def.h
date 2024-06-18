/*
   This file is part of sds.

   sds is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software
   Foundation, either version 3 of the License, or (at your option) any later
   version.

   sds is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
   A PARTICULAR PURPOSE. See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with 
   sds. If not, see <https://www.gnu.org/licenses/>. 
*/

#include <stdio.h>

static const Sensor sensors[] = {
    // name                       // address             // unit
  { "Sensor description",         "192.168.178.1/temp", "C"},
  { "Another sensor description", "192.168.178.1/hum",  "%"},
};
