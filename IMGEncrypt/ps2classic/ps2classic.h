/*+-----------------------------+*/
/*|2013 USER                    |*/
/*|decrypt algos by flatz       |*/
/*|                             |*/
/*|GPL v3,  DO NOT USE IF YOU   |*/
/*|DISAGREE TO RELEASE SRC :P   |*/
/*+-----------------------------+*/

#include "types.h"

#define PS2_META_SEGMENT_START		1
#define PS2_DATA_SEGMENT_START		2
#define PS2_DEFAULT_SEGMENT_SIZE	0x4000
#define PS2_META_ENTRY_SIZE		0x20

#define PS2_VMC_ENCRYPT			1
#define PS2_VMC_DECRYPT			0

#define CEX 0
#define DEX 1

//prototypes
void ps2_set_keylicensee(const char* file_licensse);
void ps2_encrypt_image(int mode, char* image_name, char* data_file, char* real_out_name, char* CID);
void ps2_decrypt_image(int mode, char image_name[], char meta_file[], char data_file[]);
void ps2_crypt_vmc(char mode[], char vmc_path[], char vmc_out[], u8 root_key[], int crypt_mode);
static void build_ps2_header(u8 * buffer, int npd_type, char content_id[], char filename[], s64 iso_size);

extern int porcentage;