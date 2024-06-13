/* 
 *  File: packing.h
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PACKING_H
#define PACKING_H
#pragma once

#ifdef __GNUC__

#define PACKED(type_to_pack) type_to_pack __attribute__((__packed__))

#define ALIGNED(type_to_align, n) type_to_pack __attribute__((aligned(n)))

#else

#define PACKED(type_to_pack) __pragma(pack(push, 1)) type_to_pack __pragma(pack(pop))

#define ALIGNED(type_to_align, n) __declspec(align(n)) type_to_align

#endif

#endif // PACKING_H
