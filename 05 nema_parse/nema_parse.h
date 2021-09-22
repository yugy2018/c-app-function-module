/**
*******************************************************************************
 @file nema_parse.h
 @details Encoding:GB2312
 @note
 @author yugy
 @version 1.0.0
 @date 2021.3.22
*******************************************************************************
*/ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NEMA_PARSE_H
#define __NEMA_PARSE_H


/* Includes ------------------------------------------------------------------*/
#include<stdint.h>
#include<stdio.h>

/* Define --------------------------------------------------------------------*/
#define NEMA_ERR    printf/**<*/
#define NEMA_LOG    printf/**<*/

#define NEMA_FRAME_MAX         (256) /**>一帧报文最大长度 */
#define NEMA_FRAME_HEAD_LEN     (10) /**>报文帧头标识长度 */

#define NEMA_INFO_BUFNUM_MAX     (1) 

#define GPS_SATELITE_INFO_MAX_LEN    (16)

#define _countof(_array) (sizeof(_array) / sizeof(_array[0]))


/**<Typedef -------------------------------------------------------------------*/
/**<报文解析调度用*/
typedef struct
{
	uint8_t frame_head[NEMA_FRAME_HEAD_LEN];/**>报文帧前缀*/
	void (*frame_Parse)(uint8_t *_buff,uint16_t _len);/**>解析函数*/
}nema_parse_assemble_t;

typedef struct NEMA_RECEIVER
{
	uint8_t rcvbuf[NEMA_FRAME_MAX]; /**>数据缓存 */
	uint8_t state;                  /**>处理状态 */
	uint8_t frame_type;             /**>报文帧类型 */
	uint16_t check;                 /**>校验 */
//	uint16_t tState;                /**> */
	uint16_t rcv_len;               /**>报文缓存长度 */
}nema_receiver_t;

typedef struct NEMA_SATELLITE_INFO
{
	uint16_t prn;          /**>PRN码 */
	uint16_t azimuthrange; /**>水平方位角(000～359度) */
	uint8_t elevation;     /**>卫星仰角(00～90度) */
	uint8_t snr;           /**>信噪比(00～99dB) */
}nema_satellite_info_t;

typedef struct NEMA_SATELLITE_STAT
{
	uint8_t total_gps_satellite; /**>可视GPS卫星数 */
	nema_satellite_info_t gps_satellite_info[GPS_SATELITE_INFO_MAX_LEN];    /**>各个GPS可视卫星信息 */
	uint8_t total_bd_satellite;  /**>可视北斗卫星数 */
	nema_satellite_info_t bdsatellite_info[GPS_SATELITE_INFO_MAX_LEN];     /**>各个BD可视卫星信息 */
	uint8_t total_pos_satellite; /**>参与定位卫星数 */
 	uint8_t ant_stat;            /**>天线状态 */
 	uint8_t work_stat;           /**>模块运行状态 */
 	uint8_t power_stat;          /**>电源状态 1:使能 0:关闭 */
 	uint16_t Power_timer;        /**>电源启动计时 */
	uint16_t altitude;           /**>海拔高度，单位米*/
}nema_satellite_stat_t;

/**>-GNSS接收数 */
typedef struct NEMA_RMC_INFO
{
    uint32_t SecondTimer;   /**> -时间秒计数。相对于2000年1月1日的溢出时间，最多2068年1月18号 */
    uint8_t  longiCh[11+1]; /**> -经度，字符串dddmm.mmmm */
    uint32_t longi;         /**> -经度，单位0.001秒，误差约0.03米。180度＝0x269fb200；360度＝0x4D3F6400。 */
    /**> 东经为正；西经为负，在原有西经值的基础上取反加1。 */
    uint8_t  latiCh[10+1];  /**> -维度，字符串ddmm.mmmm */
    uint32_t lati;          /**> -纬度，单位0.001秒，误差约0.03米。180度＝0x269fb200；90度＝0x134FD900。 */
    /**> 北纬为正；南纬为负，在原有南纬值的基础上取反加1。 */
    uint32_t longi2;        /**> -经度，单位为百万分一度 */
    uint32_t lati2;         /**> -纬度，单位为百万分一度 */
    uint8_t  speedCh[5+1];  /**> -速度。单位0.1节＝0.1海里/小时,最大999.9海里/小时 */
    uint16_t speed;         /**> -速度。单位0.01节＝0.01海里/小时,最大655海里/小时 */
    uint8_t  directCh[5+1]; /**> -方向。字符串单位0.1度。 */
    uint16_t direct;        /**> -方向。单位1度。 */
    uint8_t  posflags;      /**> -标记：有效性,经纬度方向。参见【IPos_FLAGS_ENUM】 */
    
}nema_rmc_info_t;

typedef struct NEMA_VTG_INFO
{
    uint32_t actualNorth;    /**> -以真北为参考基准的地面航向 放大1000倍 */
    uint32_t magneticNoth ;  /**> -以磁北为参考基准的地面航向 放大1000倍 */
    uint32_t nauticalmile;   /**> -地面速率,单位节            放大1000倍 */
    uint32_t kilometre;      /**> -地面速率,单位公里/小时     放大1000倍 */
    uint8_t  modeIndication ;/**> -模式指示——A=自主定位，D=差分，E=估算，N=数据无效 */ 
    
}nema_vtg_info_t;

typedef struct NEMA_GSA_INFO
{
    uint8_t positioningMode; /**> -定位模式 -模式 ——M = 手动， A = 自动 */
    uint32_t isPos ;         /**> -定位类型——1 = 未定位， 2 = 二维定位， 3 = 三维定位 */
    uint32_t PDOP;           /**> -PDOP位置精度因子 */
    uint32_t HDOP;           /**> -HDOP水平精度因子 */
    uint32_t VDOP ;          /**> -VDOP垂直精度因子 */
}nema_gsa_info_t;

/**>报文缓存机制 */
typedef struct NEMA_POSITION_INFO
{
    nema_rmc_info_t rmc_info_buf[NEMA_INFO_BUFNUM_MAX];
    nema_vtg_info_t vtg_info_buf[NEMA_INFO_BUFNUM_MAX];
    nema_gsa_info_t gsa_info_buf[NEMA_INFO_BUFNUM_MAX];
    uint8_t buf_head;
    uint8_t buf_count;
}nema_position_info_t;



/**<报文接收流程状态*/
enum nema_handle_section_e
{
	NEMA_INPARSE_IDLE,
	NEMA_INPARSE_REV_HEAD,
	NEMA_INPARSE_REV_CHACK_FLAG,
	NEMA_INPARSE_REV_CHACK_MSB,
	NEMA_INPARSE_REV_CHACK_LSB,
	NEMA_INPARSE_REV_CR,
	NEMA_INPARSE_REV_LF
};

enum nema_gnss_work_stat_e
{
	GNSS_DEV_WORK_UNKNOWN = 0,
	GNSS_DEV_WORK_NORMAL,     
	GNSS_DEV_WORK_FAULT,      
};

/**<标记：有效性,经纬度方向 */
enum nema_rmc_flags_e
{
	NEMA_F_VALIDMASK = 0X80, /**>有效性标记。1－有效:高精度 */
	NEMA_F_LATIMASK  = 0X40, /**>纬度标记 */
	NEMA_F_LONGIMASK = 0X20, /**>经度标记 */
	NEMA_F_ISCONNECT = 0x01, /**>报文是否有效 */
	//NEMA_F_BASEVALID=0X10, /**>基本有效标记。2D有效 */
};

/**<天线状态 */
enum nema_ant_stat_e
{
	GNSS_DEV_ANT_UNKNOWN = 0,  
	GNSS_DEV_ANT_NORMAL,       /**<天线正常*/
	GNSS_DEV_ANT_SHORTCIRCUIT, /**<天线短路*/
	GNSS_DEV_ANT_OPENCIRCUIT,  /**<天线开路*/
};


/* Functions -----------------------------------------------------------------*/
void nema_parse_data(char *buf, int buf_len);
//void show_result(void);


#endif  /**>end of __NEMA_PARSE_H */
