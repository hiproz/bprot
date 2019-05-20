# 目标
此文档旨在建议一种简单，高效，可扩展的使用于硬件和服务器之间的二进制协议。

# 协议包结构

请求消息：

| 字段     | 起始符 | 校验 | 内容长度 | 消息类型  | 请求序列好 | 时间   | 发送方ID | 消息内容 | 结束符  | 
| ----    | -----  | ----| ----    | ----      | ----        | ----  | ----    | ----    | ----   |
| 字节长度 | 2      | 2   | 2       | 2         | 4           | 6     | 8       | N       | 2      |

响应消息：

同请求消息格式，只是内容不一样而已。通用响应内容：

```
uint32_t ret_code; //响应码
uint32_t cfg_verno;//配置版本号，可选

```

1.	起始符：0xFFFF
2.	校验：从长度到内容结束，不包含结束符。采用crc16校验算法
3.	长度：内容的长度
4.	消息类型：消息类型，建议0x0101，高字节主类型，低字节子类型，支持一类消息下的，细微调整和变更
6.	请求序列号（SN）：请求方开机从0开始，递增。响应方将请求消息的序列号原样返回。
7.	时间：年月日时分秒 ，6个字节
8.	消息内容：每个消息类型有自己独立的数据内容，见下面的消息结构
9.	结束符：0xEEEE

# 约定

1.	响应消息的消息类型=请求消息的消息类型+0x10000000
2.	每个数据包需要携带发送方的UID（unique ID），对于硬件UID可以是IMEI，MAC，生产序列号等。对于服务器，UID建议用IP（byte数组）
3.	响应中的sn是将请求中的sn原样返回。
4.	时间为发送方本地RTC时钟，如果本地没有RTC，可以留空
5.	每一种消息，发送方既可以是终端，也可以是服务器，只能通过消息中的UID判断。也就是说一种消息，既可以是设备请求服务器，也可以是服务器请求设备，只要双方遵循此协议就可以。
6.	在混合位置包消息中，位置数据出现的顺序GPS>WIFI>基站
7.	为了保证数据效率，在转化成字节流时，只转化有效的内容，避免用数组最大规格。

# 数据类型规格

我们对常用的数据类型做规格定义，统一类型长度。


# 消息类型

|消息类型  |    主值   | 说明               |  
| ----    | ------    | ----     | 
|  注册包   |  0x0101     |  none    |  
|  登录包   |  0x0201     |  none    | 
|  心跳包   |  0x0301     |  none    | 
|  gps位置   |  0x0401     |  none    | 
|  wifi位置   |  0x0402     |  none    | 
|  lbs位置   |  0x0404     |  none    | 
|  wifi+lbs   |  0x0406     |  子类型是与的逻辑    | 
|  配置请求   |  0x0501     |  none    | 

# 消息内容

因为消息类型的结构多样化，markdown不支持复杂表格，所以消息内容的字节结构将通过c语言风格的结构体定义来说明。

## 注册包 0x0101
## 登录包 0x0201
## 心跳包 0x0301

## 位置包
我们的位置包含GPS，wifi，lbs三类位置数据，每个位置类型都是可选的，至少包含一种，最多3种。

|GPS信息  | wifi信息   | lbs信息 |  
| ----   | ------    | ----     | 
|  可选   |  可选     |  可选    |  

### GPS包 0x0401
#### 请求结构 0x0401

```
typedef struct {
  uint8_t sat_num;
  uint8_t loc_prop;
  uint32_t lat;
  uint32_t lng;
  uint16_t speed;
} bp_gps_struct;

typedef struct {
  uint8_t loc_num;
  bp_gps_struct gps[BPK_MAX_GPS_LOC_NUM];
} bp_gps_req_struct;
```
字段说明：

loc_num：位置包的个数，支持多包传输。

sat_num：当前包中的定位结果的有效gps数。

loc_prop：位置属性，通过bit表示：

| 7     | 6    |   5 |    4 |    3 | 2    |1     | 0    |  
| ----  | -    | ----| ---- | ---- | ---- | ---- | ---- | 
|   NC  |  NC  |  NC |  NC  |见描述 |见描述 |东/西 |南/北  | 

0bit:  0  南纬 1 北纬

1bit:  0  西经 1东经 

3:2bit： 

00：实时 

01：差分 

10：估算 

11：无效 

如果是实时、差分或者估算，说明gps已经成功定位。 

lat：纬度的 uint 表示值。纬度值为度分的整数表示 ddmm.mmmm×10000。实际数值为上报数据除以10000 ，得到纬度的度分表示法。

lng：经度度的 uint 表示值，转换方法同纬度。 

speed：单位 km/h，上报上来的是乘以100以后的整数值, 除以100得到实际值。 

#### 响应结构 0x8401

```
uint32_t ret_code; //响应码
uint32_t cfg_verno;//配置版本号，可选

```

响应内容最大支持4字节uint型：

0x00: 成功

0x01：格式错误
0x02：校验错误

...
0x10000001:服务器错误，除了定义的设备错误，其他都是服务器错误。这里可以直接返回服务器的错误码。

#### 数据样例

none
### WIFI包 0x0402
#### 请求结构

```
typedef struct {
  int8_t ssid[BP_MAX_SSID_LEN];
  uint8_t mac[BP_MAX_MAC_BYTE_LEN];
  int16_t strength;
} bp_ap_info_struct;

typedef struct {
  uint8_t ap_num;
  bp_ap_info_struct ap_list[BP_MAX_SCAN_AP_NUM];
} bp_wifi_req_struct;
```
#### 响应结构
#### 数据样例

none
### 基站包 0x0404
#### 请求结构

```
typedef struct {
  int8_t cell_str[20]; // 基站mcc mnc那些基站串，用逗号连接，含结束符
  int16_t strength;
} bp_cell_info_struct;

typedef struct {
  uint8_t cell_num;
  uint8_t cell_type; // 基站类型:0 移动 1 联通 2 电信
  bp_cell_info_struct cell_list[BP_MAX_CELL_NUM];
} bp_cell_req_struct;
```

#### 响应结构
#### 数据样例

none

## 配置请求 0x0501
### 请求结构

null
### 响应结构

待定
### 数据样例

none
