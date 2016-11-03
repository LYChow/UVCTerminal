// DimDetectorDlg.h : header file
//
#if !defined(AFX_DIMDETECTOR_H__C01379D5_DE69_4605_9CD3_AE6300D988FB__INCLUDED_)
#define AFX_DIMDETECTOR_H__C01379D5_DE69_4605_9CD3_AE6300D988FB__INCLUDED_

typedef char int8_t_dim;
typedef unsigned char uint8_t_dim;
typedef short int16_t_dim;
typedef unsigned short uint16_t_dim;
typedef int int32_t_dim;
typedef unsigned int uint32_t_dim;
typedef long long int64_t_dim;
typedef unsigned long long uint64_t_dim;

//for H264

typedef struct tag_bs_s
{
    uint8_t_dim *p_start;
    uint8_t_dim *p;
    uint8_t_dim *p_end;

    int     i_left;			// i_count number of available bits
    int     i_bits_encoded; // RD only
} bs_t;

struct _sps_info{
    int i_profile_idc;
    int i_level_idc;
	
    int b_constraint_set0;
    int b_constraint_set1;
    int b_constraint_set2;
    int b_constraint_set3;
    int b_constraint_set4;
    int b_constraint_set5;
	
    int id;
	
	// add by lzh for level 4, 2009-10-15
	int i_chroma_format_idc;
	int i_residual_colour_transform_flag;
	int i_bit_depth_luma_minus8;
	int i_bit_depth_chroma_minus8;
	int i_qpprime_y_zero_transform_bypass_flag;
	int i_seq_scaling_matrix_present_flag;
	int i_seq_scaling_list_present_flag[12];
	unsigned char ScalingList4x4[6][16];
	int UseDefaultScalingMatrix4x4Flag[6];
	unsigned char ScalingList8x8[6][64];
	int UseDefaultScalingMatrix8x8Flag[6];
	// end by lzh
	
	int i_log2_max_frame_num;
	int i_poc_type;
	int i_log2_max_poc_lsb;
	
	int i_num_ref_frames;
	int b_gaps_in_frame_num_value_allowed;
    int i_mb_width;
    int i_mb_height;
	int frame_mbs_only_flag;
	int mb_adaptive_frame_field_flag;
	int direct_8x8_inference_flag;
	int frame_cropping_flag;
	int frame_crop_left_offset;
	int frame_crop_right_offset;
	int frame_crop_top_offset;
	int frame_crop_bottom_offset;

	unsigned long width;
	unsigned long height;

	int i_sps_len_inbyte;
	int i_reduce_byte_count;
};

struct _sps265_info{
    int i_sps_video_parameter_set_id;     // u(4)
    int i_sps_max_sub_layers_minus1;      // u(3)
    int i_sps_temporal_id_nesting_flag;   // u(1)
    //profile_tier_level( sps_max_sub_layers_minus1 )
    int i_sps_seq_parameter_set_id;       // ue(v)
    int i_chroma_format_idc;              // ue(v)
    int i_separate_colour_plane_flag;     // u(1)
    int i_pic_width_in_luma_samples;      // ue(v)
    int i_pic_height_in_luma_samples;     // ue(v)
    int i_conformance_window_flag;        // u(1)
    int i_conf_win_left_offset;           // ue(v)
    int i_conf_win_right_offset;          // ue(v)
    int i_conf_win_top_offset;            // ue(v)
    int i_conf_win_bottom_offset;         // ue(v)

	unsigned long width;
	unsigned long height;

	int i_sps_len_inbyte;
	int i_reduce_byte_count;
};

void bs_init( bs_t *s, void *p_data, int i_data );
int bs_eof( bs_t *s );
uint32_t_dim bs_read( bs_t *s, int i_count );
void bs_skip( bs_t *s, int i_count );
int bs_read_ue( bs_t *s );
int bs_read_se( bs_t *s );
/* return -1 if invalid, else the id */
int do_read_sps( /*bs_t *s,*/unsigned char* nalu, int len, _sps_info& sps);
/* return -1 if invalid, else the id */
int do_read_sps( /*bs_t *s,*/unsigned char* nalu, int len, unsigned long *width, unsigned long *height);

	// add by lzh 2014-9-2
/* return -1 if invalid, else the id */
int do_read_265sps( /*bs_t *s,*/unsigned char* nalu, int len, _sps265_info& sps);
/* return -1 if invalid, else the id */
int do_read_265sps( /*bs_t *s,*/unsigned char* nalu, int len, unsigned long *width, unsigned long *height);

	// add by lzh
// for parsing MPEG-4 VOS
// The function next_bits() permits comparison of a bit string with the next bits to be decoded in the bitstream. // MAX 4 byte(32b)
uint32_t_dim next_bits(bs_t *s, int i_count = 32);
// The function bytealigned () returns 1 if the current position is on a byte boundary, that is the next bit in the bitstream
// is the first bit in a byte. Otherwise it returns 0.
int bytealigned(bs_t *s);
// The function nextbits_bytealigned() returns a bit string starting from the next byte aligned position. This permits
// comparison of a bit string with the next byte aligned bits to be decoded in the bitstream. If the current location in the
// bitstream is already byte aligned and the 8 bits following the current location are ¡®01111111¡¯, the bits subsequent to
// these 8 bits are returned. The current location in the bitstream is not changed by this function. // MAX 4 byte(32b)
uint32_t_dim nextbits_bytealigned(bs_t *s, int i_count = 32);
// The next_start_code() function removes any zero bit and a string of 0 to 7 ¡®1¡¯ bits used for stuffing and locates the
// next start code.
int next_start_code(bs_t *s);
// The next_resync_marker() function removes any zero bit and a string of 0 to 7 ¡®1¡¯ bits used for stuffing and locates
// the next resync marker; it thus performs similar operation as next_start_code() but for resync_marker.
int next_resync_marker(bs_t *s);
// The function byte_align_for_upstream() removes a string of ¡®1¡¯ used for stuffing from the upstream message. When
// the message is already byte aligned before the byte_align_for_upstream() function, additional byte stuffing is no
// longer allowed.
int byte_align_for_upstream(bs_t *s, int i_count = 32);

int do_read_vos( bs_t *s, unsigned long *width, unsigned long *height);
	// end by lzh

#endif // !defined(AFX_DIMDETECTOR_H__C01379D5_DE69_4605_9CD3_AE6300D988FB__INCLUDED_)
