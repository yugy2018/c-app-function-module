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

#define NEMA_FRAME_MAX         (256) /**>һ֡������󳤶� */
#define NEMA_FRAME_HEAD_LEN     (10) /**>����֡ͷ��ʶ���� */

#define NEMA_INFO_BUFNUM_MAX     (1) 

#define GPS_SATELITE_INFO_MAX_LEN    (16)

#define _countof(_array) (sizeof(_array) / sizeof(_array[0]))


/**<Typedef -------------------------------------------------------------------*/
/**<���Ľ���������*/
typedef struct
{
	uint8_t frame_head[NEMA_FRAME_HEAD_LEN];/**>����֡ǰ׺*/
	void (*frame_Parse)(uint8_t *_buff,uint16_t _len);/**>��������*/
}nema_parse_assemble_t;

typedef struct NEMA_RECEIVER
{
	uint8_t rcvbuf[NEMA_FRAME_MAX]; /**>���ݻ��� */
	uint8_t state;                  /**>����״̬ */
	uint8_t frame_type;             /**>����֡���� */
	uint16_t check;                 /**>У�� */
//	uint16_t tState;                /**> */
	uint16_t rcv_len;               /**>���Ļ��泤�� */
}nema_receiver_t;

typedef struct NEMA_SATELLITE_INFO
{
	uint16_t prn;          /**>PRN�� */
	uint16_t azimuthrange; /**>ˮƽ��λ��(000��359��) */
	uint8_t elevation;     /**>��������(00��90��) */
	uint8_t snr;           /**>�����(00��99dB) */
}nema_satellite_info_t;

typedef struct NEMA_SATELLITE_STAT
{
	uint8_t total_gps_satellite; /**>����GPS������ */
	nema_satellite_info_t gps_satellite_info[GPS_SATELITE_INFO_MAX_LEN];    /**>����GPS����������Ϣ */
	uint8_t total_bd_satellite;  /**>���ӱ��������� */
	nema_satellite_info_t bdsatellite_info[GPS_SATELITE_INFO_MAX_LEN];     /**>����BD����������Ϣ */
	uint8_t total_pos_satellite; /**>���붨λ������ */
 	uint8_t ant_stat;            /**>����״̬ */
 	uint8_t work_stat;           /**>ģ������״̬ */
 	uint8_t power_stat;          /**>��Դ״̬ 1:ʹ�� 0:�ر� */
 	uint16_t Power_timer;        /**>��Դ������ʱ */
	uint16_t altitude;           /**>���θ߶ȣ���λ��*/
}nema_satellite_stat_t;

/**>-GNSS������ */
typedef struct NEMA_RMC_INFO
{
    uint32_t SecondTimer;   /**> -ʱ��������������2000��1��1�յ����ʱ�䣬���2068��1��18�� */
    uint8_t  longiCh[11+1]; /**> -���ȣ��ַ���dddmm.mmmm */
    uint32_t longi;         /**> -���ȣ���λ0.001�룬���Լ0.03�ס�180�ȣ�0x269fb200��360�ȣ�0x4D3F6400�� */
    /**> ����Ϊ��������Ϊ������ԭ������ֵ�Ļ�����ȡ����1�� */
    uint8_t  latiCh[10+1];  /**> -ά�ȣ��ַ���ddmm.mmmm */
    uint32_t lati;          /**> -γ�ȣ���λ0.001�룬���Լ0.03�ס�180�ȣ�0x269fb200��90�ȣ�0x134FD900�� */
    /**> ��γΪ������γΪ������ԭ����γֵ�Ļ�����ȡ����1�� */
    uint32_t longi2;        /**> -���ȣ���λΪ�����һ�� */
    uint32_t lati2;         /**> -γ�ȣ���λΪ�����һ�� */
    uint8_t  speedCh[5+1];  /**> -�ٶȡ���λ0.1�ڣ�0.1����/Сʱ,���999.9����/Сʱ */
    uint16_t speed;         /**> -�ٶȡ���λ0.01�ڣ�0.01����/Сʱ,���655����/Сʱ */
    uint8_t  directCh[5+1]; /**> -�����ַ�����λ0.1�ȡ� */
    uint16_t direct;        /**> -���򡣵�λ1�ȡ� */
    uint8_t  posflags;      /**> -��ǣ���Ч��,��γ�ȷ��򡣲μ���IPos_FLAGS_ENUM�� */
    
}nema_rmc_info_t;

typedef struct NEMA_VTG_INFO
{
    uint32_t actualNorth;    /**> -���汱Ϊ�ο���׼�ĵ��溽�� �Ŵ�1000�� */
    uint32_t magneticNoth ;  /**> -�Դű�Ϊ�ο���׼�ĵ��溽�� �Ŵ�1000�� */
    uint32_t nauticalmile;   /**> -��������,��λ��            �Ŵ�1000�� */
    uint32_t kilometre;      /**> -��������,��λ����/Сʱ     �Ŵ�1000�� */
    uint8_t  modeIndication ;/**> -ģʽָʾ����A=������λ��D=��֣�E=���㣬N=������Ч */ 
    
}nema_vtg_info_t;

typedef struct NEMA_GSA_INFO
{
    uint8_t positioningMode; /**> -��λģʽ -ģʽ ����M = �ֶ��� A = �Զ� */
    uint32_t isPos ;         /**> -��λ���͡���1 = δ��λ�� 2 = ��ά��λ�� 3 = ��ά��λ */
    uint32_t PDOP;           /**> -PDOPλ�þ������� */
    uint32_t HDOP;           /**> -HDOPˮƽ�������� */
    uint32_t VDOP ;          /**> -VDOP��ֱ�������� */
}nema_gsa_info_t;

/**>���Ļ������ */
typedef struct NEMA_POSITION_INFO
{
    nema_rmc_info_t rmc_info_buf[NEMA_INFO_BUFNUM_MAX];
    nema_vtg_info_t vtg_info_buf[NEMA_INFO_BUFNUM_MAX];
    nema_gsa_info_t gsa_info_buf[NEMA_INFO_BUFNUM_MAX];
    uint8_t buf_head;
    uint8_t buf_count;
}nema_position_info_t;



/**<���Ľ�������״̬*/
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

/**<��ǣ���Ч��,��γ�ȷ��� */
enum nema_rmc_flags_e
{
	NEMA_F_VALIDMASK = 0X80, /**>��Ч�Ա�ǡ�1����Ч:�߾��� */
	NEMA_F_LATIMASK  = 0X40, /**>γ�ȱ�� */
	NEMA_F_LONGIMASK = 0X20, /**>���ȱ�� */
	NEMA_F_ISCONNECT = 0x01, /**>�����Ƿ���Ч */
	//NEMA_F_BASEVALID=0X10, /**>������Ч��ǡ�2D��Ч */
};

/**<����״̬ */
enum nema_ant_stat_e
{
	GNSS_DEV_ANT_UNKNOWN = 0,  
	GNSS_DEV_ANT_NORMAL,       /**<��������*/
	GNSS_DEV_ANT_SHORTCIRCUIT, /**<���߶�·*/
	GNSS_DEV_ANT_OPENCIRCUIT,  /**<���߿�·*/
};


/* Functions -----------------------------------------------------------------*/
void nema_parse_data(char *buf, int buf_len);
//void show_result(void);


#endif  /**>end of __NEMA_PARSE_H */
