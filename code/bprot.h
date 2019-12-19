#ifndef __BPROT_H__
#define __BPROT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h> //uintxx_t

/*
二进制通讯协议接口参考文档。
为避免变量和宏冲突，采用模块前缀的模块管理方式。
BP: binary protocol
*/

#define BP_DEVICE_ID_BYTE_LEN 8
#define BP_DATETIME_BYTE_LEN 6
#define BP_GPS_LOC_NUM 10
#define BP_WIFI_LOC_NUM 4
#define BP_CELL_LOC_NUM 4
#define BP_PACK_START 0xFFFF
#define BP_PACK_END 0xEEEE
#define BP_ENC_RES_LEN 32 // 加密结果
#define BP_RET_DESC_LEN 32
#define BP_MAC_BYTE_LEN 6
#define BP_SSID_LEN 32
#define BP_ICCID_STR_LEN 20
#define BP_IMSI_STR_LEN 15
#define BP_IMEI_STR_LEN 15

// 字段偏移宏
///////////////////////////////////////////
#define BP_PACK_START_OFFSET 0
#define BP_PACK_CRC_OFFSET (BP_PACK_START_OFFSET + 2)
#define BP_PACK_LEN_OFFSET (BP_PACK_CRC_OFFSET + 2)
#define BP_PACK_SN_OFFSET (BP_PACK_LEN_OFFSET + 2)
#define BP_PACK_DATETIME_OFFSET (PACK_SN_OFFSET + 4)
#define BP_PACK_FROM_UID_OFFSET (BP_PACK_DATETIME_OFFSET + 6)
#define BP_PACK_FROM_DID_OFFSET (BP_PACK_FROM_UID_OFFSET + 4)
#define BP_PACK_TO_UID_OFFSET (BP_PACK_FROM_DID_OFFSET + 8)
#define BP_PACK_TO_DID_OFFSET (BP_PACK_TO_UID_OFFSET + 4)
#define BP_PACK_CONTENT_START_OFFSET (BP_PACK_TO_DID_OFFSET + 8)

#define BP_PACK_CRC_EXCLUDE_LEN 6 //校验时总包需要排除的长度。起始+结尾+校验
#define BP_PACK_HEAD_LEN sizeof(bp_msg_head_struct)
/////////////////////////////////
// data type, 2 byte
typedef enum {
  EN_DT_RSP = 10,       // 响应节点
  EN_DT_LOGIN = 20,     // 登录信息
  EN_DT_POWER = 30,     // 电量
  EN_DT_PRESSURE = 40,  // 气压
  EN_DT_TEMP_HUMI = 50, // 温湿度
  EN_DT_GPS = 60,
  EN_DT_WIFI = 70,
  EN_DT_CELL = 80,

  EN_DT_ENCRYPT = 65500,
} DATA_TYPE_EN;

#pragma pack(1)
typedef struct {
  uint16_t start;
  uint16_t len;
  uint16_t crc;
  uint32_t sn;
  uint8_t datetime[BP_DATETIME_BYTE_LEN];
  uint32_t from_uid;
  uint8_t from_did[BP_DEVICE_ID_BYTE_LEN];
  uint32_t to_uid;
  uint8_t to_did[BP_DEVICE_ID_BYTE_LEN];
} bp_msg_head_struct;

typedef struct {
  uint16_t end;
} bp_msg_tail_struct;
/////////////////////////
// 回复响应
typedef struct {
  uint16_t len;
  uint16_t type;
  uint16_t ret_code;              // respons code
  char ret_desc[BP_RET_DESC_LEN]; //实际长度以0为截至，不超过最大规格
} respons_struct;

// 加密
typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t key_type;                  // key 类型，md5,sha1,sha2等
  uint8_t key_index;                 // 在平台创建的key索引，跟用户id有关
  uint16_t enc_data[BP_ENC_RES_LEN]; // 最终加密的结果
} encrypt_struct;

// 登录包
typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t device_id[BP_DEVICE_ID_BYTE_LEN];
  uint8_t iccid[BP_ICCID_STR_LEN]; //898604481618C0688019
  uint8_t imsi[BP_IMSI_STR_LEN];   //460046807308019
  uint8_t imei[BP_IMEI_STR_LEN];
  uint8_t mac[BP_MAC_BYTE_LEN];
  uint8_t sw_ver;
  uint8_t hw_ver;
} login_struct;

// 电量
typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t power; // 剩余电量
  uint16_t volt; // 电压
} power_struct;

// 气压
typedef struct {
  uint16_t len;
  uint16_t type;
  uint16_t air_presure;
} bp_comm_struct;

// 温湿度
typedef struct {
  uint16_t len;
  uint16_t type;
  int16_t temp;
  uint8_t humi; //湿度
} temp_humi_struct;

// GPS包
typedef struct {
  uint8_t fix_num;
  float lat;
  float lng;
  uint32_t speed;
} gps_node_struct;

typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t gps_num;
  gps_node_struct gps_list[BP_GPS_LOC_NUM];
} gps_info_struct;

// wifi
typedef struct {
  uint8_t mac[BP_MAC_BYTE_LEN];
  int16_t sig_stren;
  uint8_t ssid[BP_SSID_LEN];
} wifi_node_struct;

typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t wifi_num;
  wifi_node_struct wifi_list[BP_WIFI_LOC_NUM];
} wifi_info_struct;

// lbs
typedef struct {
  uint16_t mcc;
  uint8_t mnc;
  uint16_t lac;
  uint32_t cellid;
  //------------
  uint16_t sid;
  uint16_t nid;
  uint16_t bid;
  int16_t signal; //信号强度
} cell_node_struct;

typedef struct {
  uint16_t len;
  uint16_t type;
  uint8_t cell_type; // 基站类型:0 移动 1 联通 2 电信
  uint8_t cell_num;
  cell_node_struct cell_list[BP_CELL_LOC_NUM];
} cell_info_struct;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif //__BPROT_H__