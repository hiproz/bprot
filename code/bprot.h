#ifndef __BPROT_H__
#define __BPROT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h> //uintxx_t

/*
二进制通讯协议接口参考文档。
为避免变量和宏冲突，采用模块前缀的模块管理方式。
BPK: bprot development kit
*/

#define BPK_MAX_DEV_ID_VAL_BYTE_LEN 4 // ID中的值的长度
#define BPK_MAX_DEV_ID_BYTE_LEN 8
#define BPK_MAX_PHONE_NUM_LEN 16
#define BPK_MAX_USER_COUNT 4
#define BPK_MAX_MSG_CMD_LEN 32
#define BPK_MAX_MSG_RSP_LEN 128
#define BPK_MAX_DOMAIN_LEN 32
#define BPK_MAX_IP_ADDR_LEN 4
#define BPK_MAX_SMS_CONTENT_LEN 140
#define BPK_MAX_FENCE_NUM 4
#define BPK_MAX_APN_NAME_LEN 32
#define BPK_MAX_APN_USER_NAME_LEN 32
#define BPK_MAX_APN_PWD_LEN 16
#define BPK_MAX_APN_USER_DATA_LEN 32
#define BPK_MAX_VER_STR_LEN 16
#define BPK_MAX_CHECK_NUM 48
#define BPK_MAX_DATETIME_BYTE_LEN 6
#define BPK_MAX_TIME_BYTE_LEN 3
#define BPK_MAX_LAT_LNG_STR_LEN 11
#define BPK_MAX_LOGIN_PACKET_LEN 32
#define BPK_MAX_BIG_LIVESTOCK_NUM_IN_1_PAC 16 // 512字节下的最大的小羊的个数
#define BPK_MAX_LAMB_NUM_IN_1_PAC 32          // 512字节下的最大的小羊的个数
#define BPK_MAX_LAMB_NUM 2000
#define BPK_MAX_CFG_SVR_NUM 3             // 默认最多配置3组服务器
#define BPK_MAX_UPLOAD_TIME_NUM 10        // 上传时的时间点序列的规格，最多10个点
#define BPK_MAX_BIG_LIVESTOCK_LOC_NUM 480 // 一次上报之间缓存的GPS位置的个数

#define BPK_MAX_SCAN_AP_NUM 9
#define BPK_MAX_MAC_BYTE_LEN 6
#define BPK_MAX_SSID_LEN 16

#define BPK_MAX_GPS_LOC_NUM ((256 - sizeof(bpk_msg_head_struct) - 2 - 2) / sizeof(bpk_gps_struct))

#define PACK_START 0xFFFF
#define PACK_END 0xEEEE

// 字段偏移宏
#define PACK_START_OFFSET 0
#define PACK_CRC_OFFSET (PACK_START_OFFSET + sizeof(U16))
#define PACK_LEN_OFFSET (PACK_CRC_OFFSET + sizeof(U16))
#define PACK_TYPE_OFFSET (PACK_LEN_OFFSET + sizeof(U16))
#define PACK_VER_OFFSET (PACK_TYPE_OFFSET + sizeof(U8))
#define PACK_SN_OFFSET (PACK_VER_OFFSET + sizeof(U8))
#define PACK_DATETIME_OFFSET (PACK_SN_OFFSET + sizeof(U32))
#define PACK_CRC_EXCLUDE_LEN 6
#define PACK_HEAD_LEN sizeof(BPK_msg_head_struct)
#define PACK_ACK_LEN (PACK_HEAD_LEN + 2)

typedef enum {
  EN_ATP_IP,
  EN_ATP_DOMAIN,
} ADDR_TYPE;

typedef enum {
  EN_CFG_TYPE_SERVER = 1,
  EN_CFG_TYPE_BIG_LIVESTICK = 2,
  EN_CFG_TYPE_LAMB_GW = 3,
} CFG_DATA_TYPE;

typedef enum {
  EN_PACK_TYPE_ECHO = 0,
  EN_PT_BEACON_STATE = 0x01,
  EN_PT_BEACON_LOC = 0x03,
  EN_PT_PLAY_VOICE = 0x05,
  EN_PT_AP_SCAN = 0x07,
  EN_PT_OTA = 0x09,
  EN_PT_WIFI_VER = 0x0B,
  EN_PT_ACK = 0xFF
} PACK_TYPE_EN;

#pragma pack(1)

typedef struct {
  uint16_t start;
  uint16_t crc;
  uint16_t pack_len;
  uint16_t pack_type;
  uint32_t sn;
  uint8_t send_id[8];
  uint8_t datetime[BPK_MAX_DATETIME_BYTE_LEN];
} bpk_msg_head_struct;

// GPS包
typedef struct {
  uint8_t sat_num;
  uint8_t loc_prop;
  uint32_t lat;
  uint32_t lng;
  uint16_t speed;
} bpk_gps_struct;

typedef struct {
  uint8_t loc_num;
  bpk_gps_struct gps[BPK_MAX_GPS_LOC_NUM];
} bpk_gps_req_struct;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif //__BPROT_H__