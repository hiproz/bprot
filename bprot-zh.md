# 目标
此文档旨在建议一种简单，高效，可扩展的使用于硬件和服务器之间的二进制协议。此协议可以直接在tcp层面运用，也可以作为MQTT的用户数据结构的设计参考。

# 协议框架结构
请求和响应都是使用相同的包结构，只是内容体不一样而已。

## 协议结构

| 字段     | 起始符 | 长度 | 校验 | 序列号  | 时间 | 发送客户ID | 发送设备ID |接收客户ID  | 接收客户ID| 消息内容0 |消息内容1 | 结束符  | 
| ----    | -----  | ----| ---- | ----   | ---- | ----      | ----      |----       |----      | ----     |----     | ----   |
| 字节长度 | 2      | 2   | 2    | 4      | 6    | 4         | 8         |4          |8         | N0       |N1       | 2      |


1.	起始符：0xFFFF
2.	长度：从校验到内容结束的总长度
3.	校验：从序列号到内容结束的校验，不包含结束符。crc16校验
4.	序列号（SN）：发送方的序列号，开机后自增
5.	时间：年月日时分秒 ，6个字节，或者uint32类型的utc时间戳（高两位为0）
6.	发送客户ID：注册平台时，得到的用户id，全平台唯一。
7.	发送设备ID：设备id，由设备商自定义。
8.	接收客户ID：注册平台时，得到的用户id，全平台唯一。
9.	接收设备ID：设备id由设备商自定义。
10.	消息内容：每个消息类型有自己独立的数据内容，见下面的消息结构
11.	结束符：0xEEEE

# 内容节点数据结构
每个内容节点都是相同的结构

| 字段     | 数据长度 | 数据类型 | 数据内容 |
| ----    | -----   | ----     | ---- |
| 字节长度 | 2       | 2        | N    |

1. 长度：包含类型和内容的总长度

# 约定

1.	加密节点内容在最后一个位置，标识前面所有内容的总加密结果。
2.	设备ID，对于硬件可以是IMEI，MAC，生产序列号等。对于服务器，建议用IP（byte数组）
3.	所有数据事件都使用UTC事件。
4.	每一种消息，发送方既可以是终端，也可以是服务器，只能通过消息中的UID判断。也就是说一种消息，既可以是设备请求服务器，也可以是服务器请求设备，只要双方遵循此协议就可以。

# 常用规格定义
```
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
```
# 枚举定义
```
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
```
# 常用数据结构
```
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
```


# 用户数据包
以上是原子数据定义，真正用户做业务时，可以根据硬件功能的实际需要对多种数据进行组合，例如：

|电量            | 温湿度           | 位置                               | 
| ----          | ------           | ------                            | 
| power_struct  |  temp_humi_struct| wifi_info_struct+cell_info_struct |
