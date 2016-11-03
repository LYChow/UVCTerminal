// DimDetector.cpp : Defines the class behaviors for the application.
//
#include "DimDetector.h"
#include "SwBaseWin.h"

static const uint32_t_dim i_mask[33] ={
	0x00,
		0x01,      0x03,      0x07,      0x0f,
		0x1f,      0x3f,      0x7f,      0xff,
		0x1ff,     0x3ff,     0x7ff,     0xfff,
		0x1fff,    0x3fff,    0x7fff,    0xffff,
		0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
		0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
		0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
		0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff
};

void bs_init( bs_t *s, void *p_data, int i_data )
{
    s->p_start = (uint8_t_dim*)p_data;
    s->p       = (uint8_t_dim*)p_data;
    s->p_end   = s->p + i_data;
    s->i_left  = 8;
}


int bs_pos( bs_t *s )
{
    return( 8 * ( s->p - s->p_start ) + 8 - s->i_left );
}


int bs_eof( bs_t *s )
{
    return( s->p >= s->p_end ? 1: 0 );
}


uint32_t_dim bs_read( bs_t *s, int i_count )
{
	// add by lzh
	if (i_count > 32)
	{
		return 0x0;
	}
	// end by lzh

    int      i_shr;
    uint32_t_dim i_result = 0;

    while( i_count > 0 )
    {
        if( s->p >= s->p_end )
        {
            break;
        }

        if( ( i_shr = s->i_left - i_count ) >= 0 )
        {
            /* more in the buffer than requested */
            i_result |= ( *s->p >> i_shr )&i_mask[i_count];
            s->i_left -= i_count;
            if( s->i_left == 0 )
            {
                s->p++;
                s->i_left = 8;
            }
            return( i_result );
        }
        else
        {
           // less in the buffer than requested
           i_result |= (*s->p&i_mask[s->i_left]) << -i_shr;
           i_count  -= s->i_left;
           s->p++;
           s->i_left = 8;
        }
    }

    return( i_result );
}


uint32_t_dim bs_read1( bs_t *s )
{

    if( s->p < s->p_end )
    {
        unsigned int i_result;

        s->i_left--;
        i_result = ( *s->p >> s->i_left )&0x01;
        if( s->i_left == 0 )
        {
            s->p++;
            s->i_left = 8;
        }
        return i_result;
    }

    return 0;
}


uint32_t_dim bs_show( bs_t *s, int i_count )
{
    if( s->p < s->p_end && i_count > 0 )
    {
        uint32_t_dim i_cache = ((s->p[0] << 24)+(s->p[1] << 16)+(s->p[2] << 8)+s->p[3]) << (8-s->i_left);
        return( i_cache >> ( 32 - i_count) );
    }
    return 0;
}


/* TODO optimize */
void bs_skip( bs_t *s, int i_count )
{
    s->i_left -= i_count;

    while( s->i_left <= 0 )
    {
        s->p++;
        s->i_left += 8;
    }
}


int bs_read_ue( bs_t *s )
{
    int i = 0;

    while( bs_read1( s ) == 0 && s->p < s->p_end && i < 32 )
    {
        i++;
    }

    return( ( 1 << i) - 1 + bs_read( s, i ) );
}



int bs_read_se( bs_t *s )
{
    int val = bs_read_ue( s );

    return val & 0x01 ? (val + 1) / 2 : -(val / 2);
}


int bs_read_te( bs_t *s, int x )
{
    if( x == 1 )
    {
        return 1 - bs_read1( s );
    }
    else if( x > 1 )
    {
        return bs_read_ue( s );
    }
    return 0;
}

void bs_write( bs_t *s, int i_count, uint32_t_dim i_bits )
{
    if( s->p >= s->p_end - 4 )
	{
	    return;
	}

    while( i_count > 0 )
    {
        if( i_count < 32 )
		{
		    i_bits &= (1 << i_count) - 1;
		}

        if( i_count < s->i_left )
        {
            *s->p = (*s->p << i_count) | i_bits;
            s->i_left -= i_count;
            break;
        }
        else
        {
            *s->p = (*s->p << s->i_left) | (i_bits >> (i_count - s->i_left));
            i_count -= s->i_left;
            s->p++;
            s->i_left = 8;
        }
    }
}

void bs_write1( bs_t *s, uint32_t_dim i_bit )
{
    if( s->p < s->p_end )
    {
        *s->p <<= 1;
        *s->p |= i_bit;
        s->i_left--;
        if( s->i_left == 0 )
        {
            s->p++;
            s->i_left = 8;
        }
    }
}

void bs_align_0( bs_t *s )
{
    if( s->i_left != 8 )
    {
        *s->p <<= s->i_left;
        s->i_left = 8;
        s->p++;
    }
}


void bs_align_1( bs_t *s )
{
    if( s->i_left != 8 )
    {
        *s->p <<= s->i_left;
        *s->p |= (1 << s->i_left) - 1;
        s->i_left = 8;
        s->p++;
    }
}


void bs_align( bs_t *s )
{
    bs_align_0( s );
}



/* golomb functions */
void bs_write_ue( bs_t *s, unsigned int val )
{
    int i_size = 0;
    static const int i_size0_255[256] =
    {
        1,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
    };

    if( val == 0 )
    {
        bs_write1( s, 1 );
    }
    else
    {
        unsigned int tmp = ++val;

        if( tmp >= 0x00010000 )
        {
            i_size += 16;
            tmp >>= 16;
        }
        if( tmp >= 0x100 )
        {
            i_size += 8;
            tmp >>= 8;
        }
        i_size += i_size0_255[tmp];

        bs_write( s, 2 * i_size - 1, val );
    }
}

void bs_write_se( bs_t *s, int val )
{
    bs_write_ue( s, val <= 0 ? -val * 2 : val * 2 - 1);
}

void bs_write_te( bs_t *s, int x, int val )
{
    if( x == 1 )
    {
        bs_write1( s, 1&~val );
    }
    else if( x > 1 )
    {
        bs_write_ue( s, val );
    }
}

void bs_rbsp_trailing( bs_t *s )
{
    bs_write1( s, 1 );
    if( s->i_left != 8 )
    {
        bs_write( s, s->i_left, 0x00 );
    }
}

int bs_size_ue( unsigned int val )
{
    static const int i_size0_254[255] =
    {
        1, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,
        9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
        11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,11,11,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
        13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
        13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
        13,13,13,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
        15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
    };

    if( val < 255 )
    {
        return i_size0_254[val];
    }
    else
    {
        int i_size = 0;

        val++;

        if( val >= 0x10000 )
        {
            i_size += 32;
            val = (val >> 16) - 1;
        }
        if( val >= 0x100 )
        {
            i_size += 16;
            val = (val >> 8) - 1;
        }
        return i_size0_254[val] + i_size;
    }
}

int bs_size_se( int val )
{
    return bs_size_ue( val <= 0 ? -val * 2 : val * 2 - 1);
}

int bs_size_te( int x, int val )
{
    if( x == 1 )
    {
        return 1;
    }
    else if( x > 1 )
    {
        return bs_size_ue( val );
    }
    return 0;
}

// add by lzh for level 4, 2009-10-15
void scaling_list(bs_t *s, unsigned char scalingList[], int sizeOfScalingList, int & useDefaultScalingMatrixFlag)
{
	int lastScale = 8;
	int nextScale = 8;
	int i_delta_scale;
	for(int j = 0; j < sizeOfScalingList; j++)
	{
		if(nextScale != 0)
		{
			i_delta_scale = bs_read_se(s);
			nextScale = (lastScale + i_delta_scale + 256) % 256; 
			useDefaultScalingMatrixFlag = (j == 0 && nextScale == 0);
		}
		scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
		lastScale = scalingList[j];
	}
}
// end by lzh
// void scaling_list(bs_t *s, int scalingList[], int sizeOfScalingList, int & useDefaultScalingMatrixFlag)
// // scalingList, sizeOfScalingList, useDefaultScalingMatrixFlag )
//  {
// 	int lastScale = 8;
// 	int nextScale = 8;
// 	int i_delta_scale;
// 	for(int j = 0; j < sizeOfScalingList; j++)
// 	{
// 	if( nextScale != 0 ) {
// 		delta_scale 0 | 1 se(v)
// 			nextScale = ( lastScale + delta_scale + 256 ) % 256
// 			useDefaultScalingMatrixFlag = ( j = = 0 && nextScale = = 0 )
// 	}
// 	scalingList[ j ] = ( nextScale = = 0 ) ? lastScale : nextScale
// 		lastScale = scalingList[ j ]
// }
// }

// add by lzh (from ffmpeg-2.0.2\libavcodec), 2014-4-9
static const unsigned char default_scaling4[2][16]={
	{   6,13,20,28,
		13,20,28,32,
		20,28,32,37,
		28,32,37,42
	},{
		10,14,20,24,
		14,20,24,27,
		20,24,27,30,
		24,27,30,34
	}
};
	
static const unsigned char default_scaling8[2][64]={
	{   6,10,13,16,18,23,25,27,
		10,11,16,18,23,25,27,29,
		13,16,18,23,25,27,29,31,
		16,18,23,25,27,29,31,33,
		18,23,25,27,29,31,33,36,
		23,25,27,29,31,33,36,38,
		25,27,29,31,33,36,38,40,
		27,29,31,33,36,38,40,42
	},{
		9,13,15,17,19,21,22,24,
		13,13,17,19,21,22,24,25,
		15,17,19,21,22,24,25,27,
		17,19,21,22,24,25,27,28,
		19,21,22,24,25,27,28,30,
		21,22,24,25,27,28,30,32,
		22,24,25,27,28,30,32,33,
		24,25,27,28,30,32,33,35
	}
};

static const unsigned char zigzag_scan[16+1] = {
    0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
    1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
    1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
    3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

const unsigned char ff_zigzag_direct[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

//static void decode_scaling_list(bs_t *s, _sps_info& sps, unsigned char* factors, int& present, int size,
//									const unsigned char* jvt_list, const unsigned char* fallback_list
//								/*H264Context *h, uint8_t *factors, int size,
//                                const uint8_t *jvt_list, const uint8_t *fallback_list*/){
//    int i = 0, last = 8, next = 8;
//    const unsigned char *scan = size == 16 ? zigzag_scan : ff_zigzag_direct;
//	present = bs_read(s, 1);
//    if(!present)
//        memcpy(factors, fallback_list, size*sizeof(unsigned char));
//    else
//	{
//		for(i=0;i<size;i++)
//		{
//			if(next)
//				next = (last + bs_read_se(s)) & 0xff;
//			if(!i && !next){ /* matrix not written, we use the preset one */
//				memcpy(factors, jvt_list, size*sizeof(unsigned char));
//				break;
//			}
//			last = factors[scan[i]] = next ? next : last;
//		}
//	}
//}
//
//static void decode_scaling_matrices(bs_t *s, _sps_info& sps/*H264Context *h, SPS *sps, PPS *pps, int is_sps,
//									uint8_t (*scaling_matrix4)[16], uint8_t (*scaling_matrix8)[64]*/)
//{
//	const unsigned char *fallback[4] =
//		{ default_scaling4[0], default_scaling4[1], default_scaling8[0], default_scaling8[1] };
//
//	sps.i_seq_scaling_matrix_present_flag = bs_read(s, 1);
//    if( sps.i_seq_scaling_matrix_present_flag )
//    {
//        decode_scaling_list(s, sps, sps.ScalingList4x4[0], sps.i_seq_scaling_list_present_flag[0], 16, default_scaling4[0], fallback[0]); // Intra, Y
//        decode_scaling_list(s, sps, sps.ScalingList4x4[1], sps.i_seq_scaling_list_present_flag[1], 16, default_scaling4[0], sps.ScalingList4x4[0]); // Intra, Cr
//        decode_scaling_list(s, sps, sps.ScalingList4x4[2], sps.i_seq_scaling_list_present_flag[2], 16, default_scaling4[0], sps.ScalingList4x4[1]); // Intra, Cb
//        decode_scaling_list(s, sps, sps.ScalingList4x4[3], sps.i_seq_scaling_list_present_flag[3], 16, default_scaling4[1], fallback[1]); // Inter, Y
//        decode_scaling_list(s, sps, sps.ScalingList4x4[4], sps.i_seq_scaling_list_present_flag[4], 16, default_scaling4[1], sps.ScalingList4x4[3]); // Inter, Cr
//        decode_scaling_list(s, sps, sps.ScalingList4x4[5], sps.i_seq_scaling_list_present_flag[5], 16, default_scaling4[1], sps.ScalingList4x4[4]); // Inter, Cb
//
//        decode_scaling_list(s, sps, sps.ScalingList8x8[0], sps.i_seq_scaling_list_present_flag[6], 64, default_scaling8[0], fallback[2]);  // Intra, Y
//        decode_scaling_list(s, sps, sps.ScalingList8x8[3], sps.i_seq_scaling_list_present_flag[7], 64, default_scaling8[1], fallback[3]);  // Inter, Y
//        if(sps.i_chroma_format_idc == 3){
//            decode_scaling_list(s, sps, sps.ScalingList8x8[1], sps.i_seq_scaling_list_present_flag[8], 64, default_scaling8[0], sps.ScalingList8x8[0]);  // Intra, Cr
//            decode_scaling_list(s, sps, sps.ScalingList8x8[4], sps.i_seq_scaling_list_present_flag[9], 64, default_scaling8[1], sps.ScalingList8x8[3]);  // Inter, Cr
//            decode_scaling_list(s, sps, sps.ScalingList8x8[2], sps.i_seq_scaling_list_present_flag[10], 64, default_scaling8[0], sps.ScalingList8x8[1]);  // Intra, Cb
//            decode_scaling_list(s, sps, sps.ScalingList8x8[5], sps.i_seq_scaling_list_present_flag[11], 64, default_scaling8[1], sps.ScalingList8x8[4]);  // Inter, Cb
//        }
//    }
//}
// end by lzh

/* return -1 if invalid, else the id */
int do_read_sps(/*bs_t *s,*/unsigned char* nalu, int len, _sps_info& sps)
{
	if (!nalu || len<1)
	{
		return -1;
	}

	bs_t bs;
	bs_t* s = &bs;
	bs_init(s, nalu, len);
/*
    int sps.i_profile_idc;
    int sps.i_level_idc;

    int sps.b_constraint_set0;
    int sps.b_constraint_set1;
    int sps.b_constraint_set2;

    int sps.id;

	// add by lzh for level 4, 2009-10-15
	int sps.i_chroma_format_idc;
	int sps.i_residual_colour_transform_flag;
	int sps.i_bit_depth_luma_minus8;
	int sps.i_bit_depth_chroma_minus8;
	int sps.i_qpprime_y_zero_transform_bypass_flag;
	int sps.i_seq_scaling_matrix_present_flag;
	int sps.i_seq_scaling_list_present_flag[8];
	int sps.ScalingList4x4[6][16];
	int sps.UseDefaultScalingMatrix4x4Flag[6];
	int sps.ScalingList8x8[2][64];
	int sps.UseDefaultScalingMatrix8x8Flag[2];
	// end by lzh

	int sps.i_log2_max_frame_num;
	int sps.i_poc_type;
	int sps.i_log2_max_poc_lsb;

	int sps.i_num_ref_frames;
	int sps.b_gaps_in_frame_num_value_allowed;
    int sps.i_mb_width;
    int sps.i_mb_height;
	int sps.frame_mbs_only_flag = 0;
*/

	int i=0, i_result=0;
	unsigned char *p_data = nalu;
	int CropUnitX = 1;
	int CropUnitY = 1;

	if (next_bits(s, 32) == 0x00000001)
	{
		sps.i_reduce_byte_count += 4;
		p_data += 4;
	}
	else if (next_bits(s, 24) == 0x000001)
	{
		sps.i_reduce_byte_count += 3;
		p_data += 3;
	}

	if((p_data[0] & 0x0F) != 0x07 )
	{
		return -2;
	}
	p_data++;
	len -= (p_data-nalu);
	bs_init(s, p_data, len);

	while (!bs_eof(s))
	{
		if (next_bits(s, 32) == 0x00000001
			|| next_bits(s, 24) == 0x000001)
		{
			break;
		}
		s->p++;
	}
	len = s->p-s->p_start;
	bs_init(s, p_data, len);
	p_data = new unsigned char[len];
	while (!bs_eof(s))
	{
		if (next_bits(s, 24) == 0x000003)
		{
			p_data[i++] = s->p[0];
			p_data[i++] = s->p[1];
			s->p += 3;
			sps.i_reduce_byte_count++;
		}
		else
		{
			p_data[i++] = s->p[0];
			s->p++;
		}
	}
	len = i;
	sps.i_sps_len_inbyte = len;
	bs_init(s, p_data, len);

    sps.i_profile_idc     = bs_read( s, 8 );
    sps.b_constraint_set0 = bs_read( s, 1 );
    sps.b_constraint_set1 = bs_read( s, 1 );
    sps.b_constraint_set2 = bs_read( s, 1 );
    sps.b_constraint_set3 = bs_read( s, 1 );
    sps.b_constraint_set4 = bs_read( s, 1 );
    sps.b_constraint_set5 = bs_read( s, 1 );
    bs_skip( s, 2 );    // reserved
    sps.i_level_idc       = bs_read( s, 8 );

    sps.id = bs_read_ue( s );
    if( bs_eof( s ) || sps.id >= 32 )
    { // the sps is invalid
		i_result = -1;
		goto error;
    }

// 	// add by lzh for level 4, 2009-10-15
// 	if(sps.i_profile_idc == 100 || sps.i_profile_idc == 110 ||
// 		sps.i_profile_idc == 122 || sps.i_profile_idc == 144)
// 	{
// 		sps.i_chroma_format_idc = bs_read_ue(s);
// 		if(sps.i_chroma_format_idc == 3)
// 			sps.i_residual_colour_transform_flag = bs_read(s, 1);
// 		sps.i_bit_depth_luma_minus8 = bs_read_ue(s);
// 		sps.i_bit_depth_chroma_minus8 = bs_read_ue(s);
// 		sps.i_qpprime_y_zero_transform_bypass_flag = bs_read(s, 1);
// 		sps.i_seq_scaling_matrix_present_flag = bs_read(s, 1);
// 		if(sps.i_seq_scaling_matrix_present_flag)
// 		{
// 			for(int i = 0; i < 8; i++ )
// 			{
// 				sps.i_seq_scaling_list_present_flag[i] = bs_read(s, 1);
// 				if(sps.i_seq_scaling_list_present_flag[i])
// 				{
// 					if(i < 6)
// 						scaling_list(s, sps.ScalingList4x4[i], 16, sps.UseDefaultScalingMatrix4x4Flag[i]);
// 					else
// 						scaling_list(s, sps.ScalingList8x8[i - 6], 64, sps.UseDefaultScalingMatrix8x8Flag[i - 6]);
// 				}
// 			}
// 		}
// 	}
// 	// end by lzh

	sps.i_chroma_format_idc = 1; // edit by lzh, 2014-4-9: When chroma_format_idc is not present, it shall be inferred to be equal to 1 (4:2:0 chroma format).

/*
	// edit by lzh (from ffmpeg-2.0.2\libavcodec), 2014-4-9
    if( sps.i_profile_idc == 100 || sps.i_profile_idc == 110 ||
        sps.i_profile_idc == 122 || sps.i_profile_idc == 244 ||
        sps.i_profile_idc ==  44 || sps.i_profile_idc ==  83 ||
        sps.i_profile_idc ==  86 || sps.i_profile_idc == 118 ||
        sps.i_profile_idc == 128 || sps.i_profile_idc == 144 )
    {
        / * chroma_format_idc * /
		sps.i_chroma_format_idc = bs_read_ue( s );
        if( sps.i_chroma_format_idc == 3 )
			sps.i_residual_colour_transform_flag = bs_read(s, 1);
		sps.i_bit_depth_luma_minus8 = bs_read_ue(s);
		sps.i_bit_depth_chroma_minus8 = bs_read_ue(s);
		sps.i_qpprime_y_zero_transform_bypass_flag = bs_read(s, 1);
		decode_scaling_matrices(s, sps);
/ *
		sps.i_seq_scaling_matrix_present_flag = bs_read(s, 1);
		if( sps.i_seq_scaling_matrix_present_flag )
		{
/ *
			for( int i = 0; i < ((3 != sps.i_chroma_format_idc) ? 8 : 12); i++ )
			{ 
				/ * seq_scaling_list_present_flag[i] * /
				int i_tmp = bs_read( s, 1 );
				if( !i_tmp )
					continue;
				const int i_size_of_scaling_list = (i < 6 ) ? 16 : 64;
				/ * scaling_list (...) * /
				int i_lastscale = 8;
				int i_nextscale = 8;
				for( int j = 0; j < i_size_of_scaling_list; j++ )
				{
					if( i_nextscale != 0 )
					{
						/ * delta_scale * /
						i_tmp = bs_read_se( s );
						i_nextscale = ( i_lastscale + i_tmp + 256 ) % 256;
						/ * useDefaultScalingMatrixFlag = ... * /
					}
					/ * scalinglist[j] * /
					i_lastscale = ( i_nextscale == 0 ) ? i_lastscale : i_nextscale;
				}
			}
* /
			for( int i = 0; i < ((3 != sps.i_chroma_format_idc) ? 8 : 12); i++ )
			{
				sps.i_seq_scaling_list_present_flag[i] = bs_read(s, 1);
				if( sps.i_seq_scaling_list_present_flag[i] )
				{
					if(i < 6)
						scaling_list(s, sps.ScalingList4x4[i], 16, sps.UseDefaultScalingMatrix4x4Flag[i]);
					else
						scaling_list(s, sps.ScalingList8x8[i - 6], 64, sps.UseDefaultScalingMatrix8x8Flag[i - 6]);
				}
			}
		}
* /
    }
	// end by lzh
*/

	// edit by lzh (from T-REC-H.264-201304), 2014-4-10
    if( sps.i_profile_idc == 100 || sps.i_profile_idc == 110 ||
		sps.i_profile_idc == 122 || sps.i_profile_idc == 244 ||
		sps.i_profile_idc ==  44 || sps.i_profile_idc ==  83 ||
		sps.i_profile_idc ==  86 || sps.i_profile_idc == 118 ||
		sps.i_profile_idc == 128 || sps.i_profile_idc == 138 )
    {
        /* chroma_format_idc */
		sps.i_chroma_format_idc = bs_read_ue( s );
        if( sps.i_chroma_format_idc == 3 )
			sps.i_residual_colour_transform_flag = bs_read(s, 1);
		sps.i_bit_depth_luma_minus8 = bs_read_ue(s);
		sps.i_bit_depth_chroma_minus8 = bs_read_ue(s);
		sps.i_qpprime_y_zero_transform_bypass_flag = bs_read(s, 1);
		sps.i_seq_scaling_matrix_present_flag = bs_read(s, 1);
		if( sps.i_seq_scaling_matrix_present_flag )
		{
			for( int i = 0; i < ((3 != sps.i_chroma_format_idc) ? 8 : 12); i++ )
			{
				sps.i_seq_scaling_list_present_flag[i] = bs_read(s, 1);
				if( sps.i_seq_scaling_list_present_flag[i] )
				{
					if(i < 6)
						scaling_list(s, sps.ScalingList4x4[i], 16, sps.UseDefaultScalingMatrix4x4Flag[i]);
					else
						scaling_list(s, sps.ScalingList8x8[i - 6], 64, sps.UseDefaultScalingMatrix8x8Flag[i - 6]);
				}
			}
		}
    }
	// end by lzh

    sps.i_log2_max_frame_num = bs_read_ue( s ) + 4;

    sps.i_poc_type = bs_read_ue( s );
    if( sps.i_poc_type == 0 )
    {
        sps.i_log2_max_poc_lsb = bs_read_ue( s ) + 4;
    }
    else if( sps.i_poc_type == 1 )
    {
        int i;

		int b_delta_pic_order_always_zero;
        int i_offset_for_non_ref_pic;
        int i_offset_for_top_to_bottom_field;
        int i_num_ref_frames_in_poc_cycle;
		int i_offset_for_ref_frame [256];

        b_delta_pic_order_always_zero    = bs_read( s, 1 );
        i_offset_for_non_ref_pic         = bs_read_se( s );
        i_offset_for_top_to_bottom_field = bs_read_se( s );
        i_num_ref_frames_in_poc_cycle    = bs_read_ue( s );
        if( i_num_ref_frames_in_poc_cycle > 256 )
        {
            // FIXME what to do
            i_num_ref_frames_in_poc_cycle = 256;
        }

        for( i = 0; i < i_num_ref_frames_in_poc_cycle; i++ )
        {
            i_offset_for_ref_frame[i] = bs_read_se( s );
        }
    }
    else if( sps.i_poc_type > 2 )
    {
		i_result = -1;
        goto error;
    }

    sps.i_num_ref_frames				   = bs_read_ue( s );
    sps.b_gaps_in_frame_num_value_allowed = bs_read( s, 1 );
    sps.i_mb_width  = bs_read_ue( s ) + 1;
    sps.i_mb_height = bs_read_ue( s ) + 1;
	sps.frame_mbs_only_flag = bs_read1(s);//是一帧还是一场
	if (sps.frame_mbs_only_flag < 0 || sps.frame_mbs_only_flag > 1) 
	{
		sps.frame_mbs_only_flag = 1;
	}
	if(!sps.frame_mbs_only_flag)
		sps.mb_adaptive_frame_field_flag = bs_read1(s);
	sps.direct_8x8_inference_flag = bs_read1(s);
	sps.frame_cropping_flag = bs_read1(s);
	if(sps.frame_cropping_flag)
	{
		sps.frame_crop_left_offset = bs_read_ue( s );
		sps.frame_crop_right_offset = bs_read_ue( s );
		sps.frame_crop_top_offset = bs_read_ue( s );
		sps.frame_crop_bottom_offset = bs_read_ue( s );
	}
	// vui_parameters_present_flag 0 u(1)
	// if( vui_parameters_present_flag )
	// 	vui_parameters( ) 0
	// rbsp_trailing_bits( )

	switch (sps.i_chroma_format_idc)
	{
	case 0:
		CropUnitX = 1;
		CropUnitY = 2-sps.frame_mbs_only_flag;
		break;
	case 1:
		CropUnitX = 2/*SubWidthC*/;
		CropUnitY = 2/*SubHeightC*/*(2-sps.frame_mbs_only_flag);
		break;
	case 2:
		CropUnitX = 2/*SubWidthC*/;
		CropUnitY = 1/*SubHeightC*/*(2-sps.frame_mbs_only_flag);
		break;
	case 3:
		CropUnitX = 1/*SubWidthC*/;
		CropUnitY = 1/*SubHeightC*/*(2-sps.frame_mbs_only_flag);
		break;
	}
	sps.width  = sps.i_mb_width*16-CropUnitX*(sps.frame_crop_left_offset+sps.frame_crop_right_offset);
	sps.height = sps.i_mb_height*16*(2-sps.frame_mbs_only_flag)-CropUnitY*(sps.frame_crop_top_offset+sps.frame_crop_bottom_offset);

	delete[] p_data;
    return sps.id;

error:
	delete[] p_data;
    return i_result;
}// End of do_read_sps().

/* return -1 if invalid, else the id */
int do_read_sps( /*bs_t *s,*/unsigned char* nalu, int len, unsigned long *width, unsigned long *height)
{
	if (!nalu || len<1 || !width || !height)
		return -1;

	_sps_info sps;
	memset(&sps, 0, sizeof(sps));
	int ir = do_read_sps(nalu, len, sps);
	if (ir >= 0)
	{
		*width = sps.width;
		*height = sps.height;
	}
	return ir;
}// End of do_read_sps().


// add by lzh 2014-9-2
int read_profile_tier_level(bs_t *s, int sps_max_sub_layers_minus1)
{
    if (!s || sps_max_sub_layers_minus1<0 || sps_max_sub_layers_minus1>6)
        return -1;

    uint8_t_dim *p = s->p;
    int     i_left = s->i_left;

    int sub_layer_profile_present_flag[6] = {0};
    int sub_layer_level_present_flag[6] = {0};

	int i = 0;

    bs_read(s, 2);  // general_profile_space u(2)
    bs_read(s, 1);  // general_tier_flag u(1)
    bs_read(s, 5);  // general_profile_idc u(5)
    for(int j = 0; j<32; j++)
        bs_read(s, 1);// general_profile_compatibility_flag[ j ] u(1)
    bs_read(s, 1);  // general_progressive_source_flag u(1)
    bs_read(s, 1);  // general_interlaced_source_flag u(1)
    bs_read(s, 1);  // general_non_packed_constraint_flag u(1)
    bs_read(s, 1);  // general_frame_only_constraint_flag u(1)
    bs_skip(s, 44); // general_reserved_zero_44bits u(44)
    bs_read(s, 8);  // general_level_idc u(8)
    for(i=0; i<sps_max_sub_layers_minus1; i++)
    {
        sub_layer_profile_present_flag[i] = bs_read(s, 1);// u(1)
        sub_layer_level_present_flag[i] = bs_read(s, 1);// u(1)
    }
    if(sps_max_sub_layers_minus1 > 0)
    {
        for(i=sps_max_sub_layers_minus1; i<8; i++)
            bs_skip(s, 2);// reserved_zero_2bits[ i ] u(2)
    }
    for(i=0; i<sps_max_sub_layers_minus1; i++)
    {
        if(sub_layer_profile_present_flag[i])
        {
            bs_read(s, 2);  // sub_layer_profile_space[ i ] u(2)
            bs_read(s, 1);  // sub_layer_tier_flag[ i ] u(1)
            bs_read(s, 5);  // sub_layer_profile_idc[ i ] u(5)
            for(int j = 0; j<32; j++)
                bs_read(s, 1);  // sub_layer_profile_compatibility_flag[ i ][ j ] u(1)
            bs_read(s, 1);  // sub_layer_progressive_source_flag[ i ] u(1)
            bs_read(s, 1);  // sub_layer_interlaced_source_flag[ i ] u(1)
            bs_read(s, 1);  // sub_layer_non_packed_constraint_flag[ i ] u(1)
            bs_read(s, 1);  // sub_layer_frame_only_constraint_flag[ i ] u(1)
            bs_skip(s, 44); // sub_layer_reserved_zero_44bits[ i ] u(44)
        }
        if(sub_layer_level_present_flag[i])
            bs_read(s, 8);  // sub_layer_level_idc[ i ] u(8)
    }

    return (s->p-p)*8+i_left-s->i_left;
}

/* return -1 if invalid, else the id */
int do_read_265sps(/*bs_t *s,*/unsigned char* nalu, int len, _sps265_info& sps)
{
	if (!nalu || len<2)
	{
		return -1;
	}

	bs_t bs;
	bs_t* s = &bs;
	bs_init(s, nalu, len);

	int i=0, i_result=0;
	unsigned char *p_data = nalu;
	int CropUnitX = 1;
	int CropUnitY = 1;

    int SubWidthC = 1;
    int SubHeightC = 1;

	if (next_bits(s, 32) == 0x00000001)
	{
		sps.i_reduce_byte_count += 4;
		p_data += 4;
	}
	else if (next_bits(s, 24) == 0x000001)
	{
		sps.i_reduce_byte_count += 3;
		p_data += 3;
	}

	if((p_data[0] & 0x7E) != 0x42)
	{
		return -2;
	}
	p_data += 2;
	len -= (p_data-nalu);
	bs_init(s, p_data, len);

	while (!bs_eof(s))
	{
		if (next_bits(s, 32) == 0x00000001
			|| next_bits(s, 24) == 0x000001)
		{
			break;
		}
		s->p++;
	}
	len = s->p-s->p_start;
	bs_init(s, p_data, len);
	p_data = new unsigned char[len];
	while (!bs_eof(s))
	{
		if (next_bits(s, 24) == 0x000003)
		{
			p_data[i++] = s->p[0];
			p_data[i++] = s->p[1];
			s->p += 3;
			sps.i_reduce_byte_count++;
		}
		else
		{
			p_data[i++] = s->p[0];
			s->p++;
		}
	}
	len = i;
	sps.i_sps_len_inbyte = len;
	bs_init(s, p_data, len);

    sps.i_sps_video_parameter_set_id    = bs_read(s, 4);
    sps.i_sps_max_sub_layers_minus1     = bs_read(s, 3);
    sps.i_sps_temporal_id_nesting_flag  = bs_read(s, 1);
    if (read_profile_tier_level(s, sps.i_sps_max_sub_layers_minus1) < 0)
        goto error;
    sps.i_sps_seq_parameter_set_id      = bs_read_ue(s);
    sps.i_chroma_format_idc             = bs_read_ue(s);
    if(sps.i_chroma_format_idc == 3)
        sps.i_separate_colour_plane_flag= bs_read(s, 1);
    else
        sps.i_separate_colour_plane_flag= 0;
    sps.i_pic_width_in_luma_samples     = bs_read_ue(s);
    sps.i_pic_height_in_luma_samples    = bs_read_ue(s);
    sps.i_conformance_window_flag       = bs_read(s, 1);
    if(sps.i_conformance_window_flag)
    {
        sps.i_conf_win_left_offset      = bs_read_ue(s);
        sps.i_conf_win_right_offset     = bs_read_ue(s);
        sps.i_conf_win_top_offset       = bs_read_ue(s);
        sps.i_conf_win_bottom_offset    = bs_read_ue(s);
    }
    else
    {
        sps.i_conf_win_left_offset      = 0;
        sps.i_conf_win_right_offset     = 0;
        sps.i_conf_win_top_offset       = 0;
        sps.i_conf_win_bottom_offset    = 0;
    }

	switch (sps.i_chroma_format_idc)
	{
    case 0:
        SubWidthC = 1;
        SubHeightC = 1;
		break;
    case 1:
        SubWidthC = 2;
        SubHeightC = 2;
		break;
    case 2:
        SubWidthC = 2;
        SubHeightC = 1;
		break;
    case 3:
        //if (sps.i_separate_colour_plane_flag)
        //{
        //    SubWidthC = 1;
        //    SubHeightC = 1;
        //}
        //else
        {
            SubWidthC = 1;
            SubHeightC = 1;
        }
		break;
	}
	sps.width  = sps.i_pic_width_in_luma_samples-SubWidthC*(sps.i_conf_win_left_offset+sps.i_conf_win_right_offset);
	sps.height = sps.i_pic_height_in_luma_samples-SubHeightC*(sps.i_conf_win_top_offset+sps.i_conf_win_bottom_offset);

	delete[] p_data;
    return sps.i_sps_seq_parameter_set_id;

error:
	delete[] p_data;
    return i_result;
}// End of do_read_265sps().

/* return -1 if invalid, else the id */
int do_read_265sps( /*bs_t *s,*/unsigned char* nalu, int len, unsigned long *width, unsigned long *height)
{
	if (!nalu || len<1 || !width || !height)
		return -1;

	_sps265_info sps;
	memset(&sps, 0, sizeof(sps));
	int ir = do_read_265sps(nalu, len, sps);
	if (ir >= 0)
	{
		*width = sps.width;
		*height = sps.height;
	}
	return ir;
}// End of do_read_265sps().

	// add by lzh
// for parsing MPEG-4 VOS
// The function next_bits() permits comparison of a bit string with the next bits to be decoded in the bitstream. // MAX 4 byte(32b)
uint32_t_dim next_bits(bs_t *s, int i_count)
{
	if (i_count > 32)
	{
		return 0x0;
	}

	bs_t s2;
	memcpy(&s2, s, sizeof(bs_t));

    int      i_shr;
    uint32_t_dim i_result = 0;

    while( i_count > 0 )
    {
        if( s2.p >= s2.p_end )
        {
            break;
        }

        if( ( i_shr = s2.i_left - i_count ) >= 0 )
        {
            /* more in the buffer than requested */
            i_result |= ( *s2.p >> i_shr )&i_mask[i_count];
            s2.i_left -= i_count;
            if( s2.i_left == 0 )
            {
                s2.p++;
                s2.i_left = 8;
            }
            return( i_result );
        }
        else
        {
			// less in the buffer than requested
			i_result |= (*s2.p&i_mask[s2.i_left]) << -i_shr;
			i_count  -= s2.i_left;
			s2.p++;
			s2.i_left = 8;
        }
    }
	
    return( i_result );
}
// The function bytealigned () returns 1 if the current position is on a byte boundary, that is the next bit in the bitstream
// is the first bit in a byte. Otherwise it returns 0.
int bytealigned(bs_t *s)
{
	if (s->i_left == 8)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
// The function nextbits_bytealigned() returns a bit string starting from the next byte aligned position. This permits
// comparison of a bit string with the next byte aligned bits to be decoded in the bitstream. If the current location in the
// bitstream is already byte aligned and the 8 bits following the current location are ‘01111111’, the bits subsequent to
// these 8 bits are returned. The current location in the bitstream is not changed by this function. // MAX 4 byte(32b)
uint32_t_dim nextbits_bytealigned(bs_t *s, int i_count)
{
	bs_t s2;
	memcpy(&s2, s, sizeof(bs_t));
	
	if (s2.i_left < 8 || (s2.i_left == 8 && s2.p[0] == 0x7F))
	{
		bs_read(&s2, s2.i_left);
		return next_bits(&s2, i_count);
	}
	return 0x0;
}
// The next_start_code() function removes any zero bit and a string of 0 to 7 ‘1’ bits used for stuffing and locates the
// next start code.
int next_start_code(bs_t *s)
{
	if (bs_read1(s) != 0)// zero_bit 1 ‘0’
	{
		return 0;
	}
	while (bytealigned(s) != 1)
	{
		if (bs_read1(s) != 1)// one_bit 1 ‘1’
		{
			return 0;
		}
	}
	return 1;
}

// resync_marker: This is a binary string of at least 16 zero’s followed by a one ‘0 0000 0000 0000 0001’. For an IVOP
// or a VOP where video_object_layer_shape has the value “binary_only”, the resync marker is 16 zeros
// followed by a one. The length of this resync marker is dependent on the value of vop_fcode_forward, for a P-VOP
// or a S(GMC)-VOP, and the larger value of either vop_fcode_forward and vop_fcode_backward for a B-VOP. For a
// P-VOP and a S(GMC)-VOP, the resync_marker is (15+fcode) zeros followed by a one; for a B-VOP, the
// resync_marker is max(15+fcode,17) zeros followed by a one. It is only present when resync_marker_disable flag is
// set to ‘0’. A resync marker shall only be located immediately before a macroblock and aligned with a byte.

// The next_resync_marker() function removes any zero bit and a string of 0 to 7 ‘1’ bits used for stuffing and locates
// the next resync marker; it thus performs similar operation as next_start_code() but for resync_marker.
int next_resync_marker(bs_t *s);
// The function byte_align_for_upstream() removes a string of ‘1’ used for stuffing from the upstream message. When
// the message is already byte aligned before the byte_align_for_upstream() function, additional byte stuffing is no
// longer allowed.
int byte_align_for_upstream(bs_t *s);

/* return -1 if invalid, else the vos header length */
int do_read_vos( bs_t *s, unsigned long *width, unsigned long *height)
{
// 	static const unsigned int nHCLen = 4;
// 	static const unsigned char visual_object_sequence_start_code[nHCLen] =	{0x00, 0x00, 0x01, 0xB0};
// 	static const unsigned char visual_object_sequence_end_code[nHCLen] =	{0x00, 0x00, 0x01, 0xB1};
// 	static const unsigned char user_data_start_code[nHCLen] =				{0x00, 0x00, 0x01, 0xB2};
// 	static const unsigned char video_object_start_code[nHCLen] =			{0x00, 0x00, 0x01, 0x00};
// 	static const unsigned char video_object_layer_start_code[nHCLen] =		{0x00, 0x00, 0x01, 0x20};
// 	static const unsigned char video_object_layer_start_code_mask[nHCLen] =	{0xFF, 0xFF, 0xFF, 0xF0};
// 	static const unsigned char group_of_vop_start_code[nHCLen] =			{0x00, 0x00, 0x01, 0xB3};
// 	static const unsigned char visual_object_start_code[nHCLen] =			{0x00, 0x00, 0x01, 0xB5};
// 	static const unsigned char vop_start_code[nHCLen] =						{0x00, 0x00, 0x01, 0xB6};

// 	static const unsigned char [nHCLen] =			{0x00, 0x00, 0x01, 0x00};
// 	static const unsigned char [nHCLen] =		{0x00, 0x00, 0x01, 0x20};
// 	static const unsigned char [nHCLen] =	{0xFF, 0xFF, 0xFF, 0xF0};
// 	static const unsigned char [nHCLen] =			{0x00, 0x00, 0x01, 0xB3};
// 	static const unsigned char [nHCLen] =			{0x00, 0x00, 0x01, 0xB5};
// 	static const unsigned char [nHCLen] =						{0x00, 0x00, 0x01, 0xB6};
	static const uint32_t_dim visual_object_sequence_start_code =	0x000001B0;
	static const uint32_t_dim visual_object_sequence_end_code =		0x000001B1;
	static const uint32_t_dim user_data_start_code =				0x000001B2;
	static const uint32_t_dim video_object_start_code =				0x00000100;
	static const uint32_t_dim video_object_layer_start_code_28 =	0x0000012;
//	static const uint32_t_dim video_object_layer_start_code_mask =	0xFFFFFFF0;
	static const uint32_t_dim group_of_vop_start_code =				0x000001B3;
	static const uint32_t_dim visual_object_start_code =			0x000001B5;
	static const uint32_t_dim vop_start_code =						0x000001B6;

	static const uint32_t_dim a_new_start_code_24 =					0x000001;

	int32_t_dim nVosLen = -1;
	unsigned char * pData = s->p;
	uint32_t_dim nxtbts = 0x0;
	while (bs_eof(s) != 1)
	{
		while (bytealigned(s) != 1)
		{
			bs_skip(s, s->i_left);
		}

		nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
		if (nxtbts != visual_object_sequence_start_code)
		{
			if (s->p > s->p_start + 16)
			{ // give up if cannot find visual_object_sequence_start_code in the first 16 bytes
				break;
			}

			bs_skip(s, 8);
			continue;
		}

		bs_read(s, 32);	// visual_object_sequence_start_code
		nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
		bs_read(s, 8);	// profile_and_level_indication

		nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
		while (nxtbts == user_data_start_code)
		{
			bs_read(s, 32); // user_data_start_code
			while(1)
			{
				nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
				nxtbts = next_bits(s, 24);
				if (nxtbts == a_new_start_code_24)
				{
					break;
				}
				bs_read(s, 8); // user_data 8 uimsbf
			}
			nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
		}

		if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
		if (nxtbts == visual_object_start_code)
		{
			bs_read(s, 32); // visual_object_start_code

			nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			uint32_t_dim is_visual_object_identifier = bs_read1(s);
			if (is_visual_object_identifier == 1) // 
			{
				uint32_t_dim visual_object_verid = bs_read(s, 4);
				uint32_t_dim visual_object_priority  = bs_read(s, 3);
			}

			nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			uint32_t_dim visual_object_type = bs_read(s, 4);
			if (visual_object_type == 0x01 || // "video ID" 0b0001
				visual_object_type == 0x02 ) // "still texture ID" 0b0010
			{ // video_signal_type()
				uint32_t_dim video_signal_type = bs_read1(s);
				if (video_signal_type == 1)
				{
					uint32_t_dim video_format = bs_read(s, 3);

					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					uint32_t_dim video_range = bs_read1(s);
					uint32_t_dim colour_description = bs_read1(s);
					if (colour_description == 1)
					{
						uint32_t_dim colour_primaries = bs_read(s, 8);
						uint32_t_dim transfer_characteristics = bs_read(s, 8);
						uint32_t_dim matrix_coefficients = bs_read(s, 8);
					}
				}
			}
			
			next_start_code(s);

			nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			while (nxtbts == user_data_start_code)
			{
				bs_read(s, 32); // user_data_start_code
				while(1)
				{
					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					nxtbts = next_bits(s, 24);
					if (nxtbts == a_new_start_code_24)
					{
						break;
					}
					bs_read(s, 8); // user_data 8 uimsbf
				}
				nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			}

			if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			if (visual_object_type == 0x01) // "video ID" 0b0001
			{
				if (nxtbts != video_object_start_code)
				{
					break;
				}
				bs_read(s, 32); // video_object_start_code

				nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
				nxtbts = next_bits(s, 28);
				int8_t_dim short_video_header = 0;
				if(nxtbts == video_object_layer_start_code_28)
				{
					short_video_header = 0;
					bs_read(s, 32); // video_object_layer_start_code

					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					bs_read1(s); // random_accessible_vol 1 bslbf
					bs_read(s, 8); // video_object_type_indication 8 uimsbf
					int8_t_dim video_object_layer_verid = 0;
					int8_t_dim is_object_layer_identifier = bs_read1(s); // 1 uimsbf
					if (is_object_layer_identifier == 1)
					{
						video_object_layer_verid = bs_read(s, 4); // 4 uimsbf
						bs_read(s, 3); // video_object_layer_priority 3 uimsbf
					}

					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					int8_t_dim aspect_ratio_info = bs_read(s, 4); // 4 uimsbf
					if (aspect_ratio_info == 0x0F) // "extended_PAR" 0b1111
					{
						bs_read(s, 8); // par_width 8 uimsbf
						bs_read(s, 8); // par_height 8 uimsbf
					}
					int8_t_dim vol_control_parameters = bs_read1(s); // 1 uimsbf
					if (vol_control_parameters == 1)
					{
						bs_read(s, 2); // chroma_format 2 uimsbf
						bs_read1(s); // low_delay 1 uimsbf
						int8_t_dim vbv_parameters = bs_read1(s); // 1 uimsbf
						if (vbv_parameters == 1)
						{
							nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
							bs_read(s, 15); // first_half_bit_rate 15 uimsbf
							bs_read1(s); // marker_bit 1 bslbf
							bs_read(s, 15); // latter_half_bit_rate 15 uimsbf
							bs_read1(s); // marker_bit 1 bslbf

							nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
							bs_read(s, 15); // first_half_vbv_buffer_size 15 uimsbf
							bs_read1(s); // marker_bit 1 bslbf
							bs_read(s, 3); // latter_half_vbv_buffer_size 3 uimsbf
							bs_read(s, 11); // first_half_vbv_occupancy 11 uimsbf
							bs_read1(s); // marker_bit 1 blsbf

							nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
							bs_read(s, 15); // latter_half_vbv_occupancy 15 uimsbf
							bs_read1(s); // marker_bit 1 blsbf
						}
					}

					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					int8_t_dim video_object_layer_shape = bs_read(s, 2); //  2 uimsbf
					if (video_object_layer_shape == 0x03 && //  "grayscale" 0b11
						video_object_layer_verid != 0x01) // "0001"
						bs_read(s, 4); // video_object_layer_shape_extension 4 uimsbf
					bs_read1(s); // marker_bit 1 blsbf
					int16_t_dim vop_time_increment_resolution = bs_read(s, 16); //  16 uimsbf
					bs_read1(s); // marker_bit 1 blsbf

					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					int8_t_dim fixed_vop_rate = bs_read1(s); // 1 bslbf
					if (fixed_vop_rate == 1)
					{
						int8_t_dim bitCnt = 0;
						int16_t_dim vtirT = vop_time_increment_resolution - 1;
						for (bitCnt = 0; bitCnt < 16;) // 16: bits cnt of vop_time_increment_resolution
						{
							if (vtirT > 0)
							{
								vtirT = vtirT>>1;
							}
							else
							{
								break;
							}
							bitCnt++;
						}
						bs_read(s, bitCnt); // fixed_vop_time_increment 1-16 uimsbf
					}
/*fixed_vop_time_increment: This value represents the number of ticks between two successive VOPs in the
display order. The length of a tick is given by vop_time_increment_resolution. It can take a value in the range of
[0,vop_time_increment_resolution). The number of bits representing the value is calculated as the minimum
number of unsigned integer bits required to represent the above range. fixed_vop_time_increment shall only be
present if fixed_vop_rate is '1' and its value must be identical to the constant given by the distance between the
display time of any two successive VOPs in the display order. In this case, the fixed VOP rate is given as
(vop_time_increment_resolution / fixed_vop_time_increment). A zero value is forbidden.*/

// video_object_layer_shape
// Shape format	Meaning
// 		00		rectangular
// 		01		binary
// 		10		binary only
// 		11		grayscale
					nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
					if (video_object_layer_shape != 0x02) // "binary only" 0b10
					{
						if (video_object_layer_shape == 0x00) // "rectangular" 0b00
						{ // it's my goal!  lzh 2009-09-10
							bs_read1(s); // marker_bit 1 blsbf
							int32_t_dim video_object_layer_width = bs_read(s, 13); // 13 uimsbf
							bs_read1(s); // marker_bit 1 blsbf
							int32_t_dim video_object_layer_height = bs_read(s, 13); // 13 uimsbf
							bs_read1(s); // marker_bit 1 blsbf

					// now return......
							while (bytealigned(s) != 1)
							{
								bs_skip(s, s->i_left);
							}
							nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;

							// find b3/b6
							while (nxtbts != group_of_vop_start_code && nxtbts != vop_start_code)
							{
								bs_skip(s, 8);
								nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
							}

							// it's not a strict calculation
							int32_t_dim vosLenT = s->p - s->p_start;

// 							// gov header & vop header parsing is not needed for getting dimension. lzh 2010-1-22
// 							if (nxtbts == group_of_vop_start_code) // group_of_vop_start_code
// 							{
// 								// do parsing b3(gov) header (group_of_vop header)
// 							}
// 							else if (nxtbts == vop_start_code) // vop_start_code
// 							{
// 								// do parsing b6(vop) header (VideoObjectPlane header)
// 							}

							if (width != NULL && height != NULL)
							{
								*width = video_object_layer_width;
								*height = video_object_layer_height;
								nVosLen = vosLenT;
								return nVosLen;
							}
						}
//						interlaced 1 bslbf
// 						obmc_disable 1 bslbf
// 						if (video_object_layer_verid == ‘0001’)
// 							sprite_enable 1 bslbf
// 						else
// 							sprite_enable 2 uimsbf
// 
// 						if (sprite_enable== “static” || sprite_enable == “GMC”)
// 						{
// 							if (sprite_enable != “GMC”)
// 							{
// 								sprite_width 13 uimsbf
// 								marker_bit 1 bslbf
// 								sprite_height 13 uimsbf
// 								marker_bit 1 bslbf
// 								sprite_left_coordinate 13 simsbf
// 								marker_bit 1 bslbf
// 								sprite_top_coordinate 13 simsbf
// 								marker_bit 1 bslbf
// 							}
// 							no_of_sprite_warping_points 6 uimsbf
// 							sprite_warping_accuracy 2 uimsbf
// 							sprite_brightness_change 1 bslbf
// 							if (sprite_enable != “GMC”)
// 								low_latency_sprite_enable 1 bslbf
// 						}
// 						if (video_object_layer_verid != ‘0001’ &&
// 							video_object_layer_shape != ”rectangular”)
// 							sadct_disable 1 bslbf
// 
// 						not_8_bit 1 bslbf
// 
// 						if (not_8_ bit) 
// 						{
// 							quant_precision 4 uimsbf
// 							bits_per_pixel 4 uimsbf
// 						}
// 						if (video_object_layer_shape==”grayscale”)
// 						{
// 							no_gray_quant_update 1 bslbf
// 							composition_method 1 bslbf
// 							linear_composition 1 bslbf
// 						}
// 
// 						quant_type 1 bslbf
// 
// 						if (quant_type)
// 						{
// 							load_intra_quant_mat 1 bslbf
// 							if (load_intra_quant_mat)
// 								intra_quant_mat 8*[2-64] uimsbf
// 							
// 							load_nonintra_quant_mat 1 bslbf
// 							
// 							if (load_nonintra_quant_mat)
// 								nonintra_quant_mat 8*[2-64] uimsbf
// 
// 							if(video_object_layer_shape==”grayscale”)
// 							{
// 								for(i=0; i<aux_comp_count; i++)
// 								{
// 									load_intra_quant_mat_grayscale 1 bslbf
// 
// 									if(load_intra_quant_mat_grayscale)
// 										intra_quant_mat_grayscale[i] 8*[2-64] uimsbf
// 
// 									load_nonintra_quant_mat_grayscale 1 bslbf
// 									if(load_nonintra_quant_mat_grayscale)
// 										nonintra_quant_mat_grayscale[i] 8*[2-64] uimsbf
// 								}
// 							}
// 						}
// 						if (video_object_layer_verid != ‘0001’)
// 							quarter_sample 1 bslbf
// 
// 						complexity_estimation_disable 1 bslbf
// 
// 						if (!complexity_estimation_disable)
// 							define_vop_complexity_estimation_header()
// 
// 						resync_marker_disable 1 bslbf
// 						data_partitioned 1 bslbf
// 
// 						if(data_partitioned)
// 							reversible_vlc 1 bslbf
// 
// 						if(video_object_layer_verid != ’0001’)
// 						{
// 							newpred_enable 1 bslbf
// 							if (newpred_enable) 
// 							{
// 								requested_upstream_message_type 2 uimsbf
// 								newpred_segment_type 1 bslbf
// 							}
// 							reduced_resolution_vop_enable 1 bslbf
// 						}
// 						scalability 1 bslbf
// 
// 						if (scalability)
// 						{
// 							hierarchy_type 1 bslbf
// 							ref_layer_id 4 uimsbf
// 							ref_layer_sampling_direc 1 bslbf
// 							hor_sampling_factor_n 5 uimsbf
// 							hor_sampling_factor_m 5 uimsbf
// 							vert_sampling_factor_n 5 uimsbf
// 							vert_sampling_factor_m 5 uimsbf
// 							enhancement_type 1 bslbf
// 
// 							if(video_object_layer == “binary” &&
// 								hierarchy_type== ‘0’)
// 							{
// 								use_ref_shape 1 bslbf
// 								use_ref_texture 1 bslbf
// 								shape_hor_sampling_factor_n 5 uimsbf
// 								shape_hor_sampling_factor_m 5 uimsbf
// 								shape_vert_sampling_factor_n 5 uimsbf
// 								shape_vert_sampling_factor_m 5 uimsbf
// 							}
//						}
					}
// 					else 
// 					{
// 						if(video_object_layer_verid !=”0001”)
// 						{
// 							scalability 1 bslbf
// 
// 							if(scalability)
// 							{
// 								ref_layer_id 4 uimsbf
// 								shape_hor_sampling_factor_n 5 uimsbf
// 								shape_hor_sampling_factor_m 5 uimsbf
// 								shape_vert_sampling_factor_n 5 uimsbf
// 								shape_vert_sampling_factor_m 5 uimsbf
// 							}
// 						}
// 						resync_marker_disable 1 bslbf
// 					}
// 					next_start_code()
// 
// 					while ( next_bits()== user_data_start_code)
// 					{
// 						user_data()
// 					}
// 
// 					if (sprite_enable == “static” && !low_latency_sprite_enable)
// 						VideoObjectPlane()
// 
// 					do {
// 						if (next_bits() == group_of_vop_start_code)
// 							Group_of_VideoObjectPlane()
// 
// 						VideoObjectPlane()
// 
// 						if ((preceding_vop_coding_type == "B" ||
// 							 preceding_vop_coding_type == "S" ||
// 							 video_object_layer_shape != "rectangular") &&
// 							next_bits() == stuffing_start_code)
// 						{
// 							stuffing_start_code 32 bslbf
// 
// 							while (next_bits() != ‘0000 0000 0000 0000 0000 0001’)
// 								stuffing_byte 8 bslbf
// 						}
// 					} while ((next_bits() == group_of_vop_start_code) ||
// 						(next_bits() == vop_start_code))
				}
				else
				{
					short_video_header = 1;
					break; // discard video_plane_with_short_header
// 					do {
// 						video_plane_with_short_header()
// 					} while(next_bits() == short_video_start_marker)
				}
			}
			else
			{
				break; // discard all other visual_object_type
			}

			nxtbts = next_bits(s); if (nxtbts == 0x0 || nxtbts == visual_object_sequence_end_code)	break;
			nxtbts = next_bits(s, 24);
			if (nxtbts != a_new_start_code_24) // "0000 0000 0000 0000 0000 0001"
			{
				next_start_code(s);
			}
		}
	}

//TRACE("do_read_vos nVosLen: %d\n", nVosLen);
	return nVosLen;
}
	// end by lzh
