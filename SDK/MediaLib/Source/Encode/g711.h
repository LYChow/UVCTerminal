/*
* Copyright (C) 2010-2011 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tdav_codec_g711.h
 * @brief G.711u and G.711a (a.k.a PCMU and PCMA) codec plugins.
 *
 * @author Mamadou Diop <diopmamadou(at)doubango.org>
 *

 */
#ifndef TINYDAV_CODEC_G711_IMPLEMENTATION_H
#define TINYDAV_CODEC_G711_IMPLEMENTATION_H

union UNION_711_IN
{
    unsigned char in_char[2];
    short         in_short;
};

union UNION_711_OUT
{
    unsigned char out_char[2];
    short         out_short;
};

unsigned char linear2alaw(short	pcm_val);
short alaw2linear(unsigned char	a_val);
unsigned char linear2ulaw(short	pcm_val);
short ulaw2linear(unsigned char	u_val);

#endif /* TINYDAV_CODEC_G711_IMPLEMENTATION_H */
