/**
*******************************************************************************
 @file nema_parse.c
 @details Encoding:GB2312 nmea报文解析
 @author yugy
 @version 1.0.0
 @date 2021.3.22
*******************************************************************************
*/ 
#include "nema_parse.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>


nema_receiver_t g_nema_rcv;
nema_satellite_stat_t g_nema_satellite_work_stat;
nema_position_info_t  g_nema_position_info;

uint16_t search_near_char(uint8_t *_buf,uint16_t _len,uint8_t _char)
{
	uint16_t m_len;

	for(m_len = 0; m_len < _len; m_len++)
	{
		if(_buf[m_len] == _char)
			return m_len+1;
	}
	return _len;
}

/**
*******************************************************************************
 @brief 纬度定位数据转换
 
 @param _PosData 纬度报文数据
 @return  返回纬度为百万分之-的定位数据
*******************************************************************************
*/
uint32_t pos_lat_iconvert2(uint8_t *_PosData)
{
	uint32_t aLati;

	aLati = (_PosData[2]-0x30)*10000000+(_PosData[3]-0x30)*1000000+(_PosData[5]-0x30)*100000
	 +(_PosData[6]-0x30)*10000+(_PosData[7]-0x30)*1000+(_PosData[8]-0x30)*100;
	aLati = aLati/60+(_PosData[0]-0x30)*10000000+(_PosData[1]-0x30)*1000000;
	return aLati;
}

/**
*******************************************************************************
 @brief 经度定位数据转换
 
 @param _PosData 经度报文数据
 @return  返回经度为百万分之-的定位数据
*******************************************************************************
*/
uint32_t pos_long_iconvert2(uint8_t *_PosData)
{
	uint32_t aLongi;

	aLongi = (_PosData[3]-0x30)*10000000+(_PosData[4]-0x30)*1000000+(_PosData[6]-0x30)*100000
	 +(_PosData[7]-0x30)*10000+(_PosData[8]-0x30)*1000+(_PosData[9]-0x30)*100;
	aLongi = aLongi/60+(_PosData[0]-0x30)*100000000+(_PosData[1]-0x30)*10000000+(_PosData[2]-0x30)*1000000;
	return aLongi;
}

/**
*******************************************************************************
 @brief 纬度定位数据转换,
 
 @param _PosData 纬度的字符串,ddmm.mmmm
 @return  返回纬度为0.001秒的定位数据
*******************************************************************************
*/
uint32_t pos_lat_iconvert(uint8_t *_PosData)
{
	uint32_t aLati;

	aLati = ((_PosData[0]-0x30)*10+(_PosData[1]-0x30))*3600*1000;
	aLati = aLati + ((_PosData[2]-0x30)*10*10000+(_PosData[3]-0x30)*10000+(_PosData[5]-0x30)*1000
	 +(_PosData[6]-0x30)*100+(_PosData[7]-0x30)*10+(_PosData[8]-0x30))*6;
	return aLati;
}

/**
*******************************************************************************
 @brief 经度定位数据转换,
 
 @param _PosData 经度的字符串,ddmm.mmmm
 @return  返回经度为0.001秒的定位数据
*******************************************************************************
*/
uint32_t pos_long_iconvert(uint8_t *_PosData)
{
	uint32_t aLongi;

	aLongi  = ((_PosData[0]-0x30)*100+(_PosData[1]-0x30)*10+(_PosData[2]-0x30))*3600*1000;
	aLongi +=  ((_PosData[3]-0x30)*10*10000+(_PosData[4]-0x30)*10000+(_PosData[6]-0x30)*1000
	 +(_PosData[7]-0x30)*100+(_PosData[8]-0x30)*10+(_PosData[9]-0x30))*6;

	return aLongi;
}

/**
*******************************************************************************
 @brief 解析GGA前缀报文数据
 @detail 通用GPS/GLONASS/Galileo/BDS，作用结果为计算海拔高度
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
static void nema_parse_gga(uint8_t *_buff,uint16_t _len)
{
	uint16_t m_ptr,m_i,m_p,m_temp,m_flag = 0;
	m_ptr = 0;
	for(m_i = 0; m_i < 7; m_i++)
	{
		m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
		if(m_ptr >= _len)
			return;
	}
	m_p = 0;
	while(1)
	{
		if(_buff[m_ptr] == ',')
		{
			break;
		}
		if((_buff[m_ptr] >= '0') && (_buff[m_ptr] <= '9'))
		{
			m_p *= 10;
			m_p += (_buff[m_ptr]-'0');
		}
		m_ptr++;
		if(m_ptr >= _len)
			break;
	}
	g_nema_satellite_work_stat.total_pos_satellite = m_p;
	if(m_ptr >= _len)
		return;
	m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
	if(m_ptr >= _len)
		return;
	m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
	if(m_ptr >= _len)
		return;
	m_p = 0;
	while(1)
	{	
		if((_buff[m_ptr] == ',') || (_buff[m_ptr] == '.'))
		{
			break;
		}
		if((_buff[m_ptr] >= '0') && (_buff[m_ptr] <= '9'))
		{
			m_p *= 10;
			m_p += (_buff[m_ptr]-'0');
		}
		m_ptr++;
		if(m_ptr >= _len)
			break;
	}
	g_nema_satellite_work_stat.altitude = m_p;
	
	m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
	if(m_ptr >= _len)
		return;
	m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
	if(m_ptr >= _len)
		return;
	m_temp = 0;
	while(1)
	{
		if((_buff[m_ptr] == '-'))
			m_flag = 1;
		if((_buff[m_ptr] == ',')  || (_buff[m_ptr] == '.'))
		{
			break;
		}
		if((_buff[m_ptr] >= '0') && (_buff[m_ptr] <= '9'))
		{
			m_temp *= 10;
			m_temp += (_buff[m_ptr]-'0');
		}
		m_ptr++;
		if(m_ptr >= _len)
			break;
	}
	if(m_flag == 1)
		g_nema_satellite_work_stat.altitude -= m_temp;
	else
		g_nema_satellite_work_stat.altitude += m_temp;
}

/**
*******************************************************************************
 @brief 解析$GNGSA前缀报文数据，获取位置/水平/垂直 精度因子
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_gsa(uint8_t *_buff,uint16_t _len)
{
   uint16_t m_ptr= 0,tmp = 0; 
   uint8_t is_position = 0,i=0;
   nema_gsa_info_t *gsa_info = &g_nema_position_info.gsa_info_buf[g_nema_position_info.buf_head]; 
   memset((uint8_t*)gsa_info,0,sizeof (nema_gsa_info_t));
   uint8_t strTmp[6]={0};
   
   m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
	if(m_ptr >= _len)
		return;
   if((gsa_info->positioningMode = _buff[m_ptr++]) == 'A')
   {
       m_ptr++; //,
       is_position = _buff[m_ptr++]; //1
       m_ptr++; //,
       if(is_position !='1')
       {
           for(i=0;i<12;i++)
           {
                m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
                if(m_ptr >= _len)
                    return;
           }
           if(_buff[m_ptr]!=',')
           {
                tmp = 0;
                memset(strTmp,0x00,sizeof(strTmp));
                while(1)
                {
                    if((_buff[m_ptr] == ',') || (tmp >=sizeof(strTmp)))
                        break;
                    strTmp[tmp++] += _buff[m_ptr++];
                } 
                gsa_info->PDOP = atof((char *)strTmp)*10;
                
                m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
                if(m_ptr >= _len)
                    return;
                tmp = 0;
                memset(strTmp,0x00,sizeof(strTmp));
                while(1)
                {
                    if(_buff[m_ptr] == ',')
                        break;
                    strTmp[tmp++] += _buff[m_ptr++];
                }
                gsa_info->HDOP = atof((char *)strTmp)*10;
                
                m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
                if(m_ptr >= _len)
                    return;
                tmp = 0;
                memset(strTmp,0x00,sizeof(strTmp));
                while(1)
                {
                    if(_buff[m_ptr] == ',' || _buff[m_ptr] == '*' || tmp>=5)
                        break;
                    strTmp[tmp++] += _buff[m_ptr++];
                }
                if(tmp<5)
                    gsa_info->VDOP = atof((char *)strTmp)*10;
           }
       }
   }       
}

/**
*******************************************************************************
 @brief 解析$GPGSV前缀报文数据
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_gpgsv(uint8_t *_buff,uint16_t _len)
{
	uint16_t aPtr,t2, ai,index;

	aPtr  = 0;
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
	//t1 = atol((char *)&_buff[aPtr]);	/**>语句总数*/
	
    aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
	t2 = atol((char *)&_buff[aPtr]);	/**>语句号*/
    
    
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	g_nema_satellite_work_stat.total_gps_satellite = atol((char *)&_buff[aPtr]);

    ai = 0;
    for (ai = 0; ai < 4; ai++)
	{
		index = ((t2 - 1) * 4) + ai;
		if (index >= _countof(g_nema_satellite_work_stat.gps_satellite_info))
			break;

		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.gps_satellite_info[index].prn = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.gps_satellite_info[index].prn = atoi((char *)&_buff[aPtr]);

		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.gps_satellite_info[index].elevation = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.gps_satellite_info[index].elevation = atoi((char *)&_buff[aPtr]);



		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.gps_satellite_info[index].azimuthrange = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.gps_satellite_info[index].azimuthrange = atoi((char *)&_buff[aPtr]);



		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.gps_satellite_info[index].snr = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.gps_satellite_info[index].snr = atoi((char *)&_buff[aPtr]);

	}
}

/**
*******************************************************************************
 @brief 解析$BDGSV前缀报文数据
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_bdgsv(uint8_t *_buff,uint16_t _len)
{
	uint16_t aPtr,ai, t2,index;
	// 提取卫星数
	aPtr  = 0;
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
	//t1 = atol((char *)&_buff[aPtr]);
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
	t2 = atol((char *)&_buff[aPtr]);
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	g_nema_satellite_work_stat.total_bd_satellite = atol((char *)&_buff[aPtr]);


    ai = 0;
    for (ai = 0; ai < 4; ai++)
	{
		index = ((t2 - 1) * 4) + ai;
		if (index >= _countof(g_nema_satellite_work_stat.bdsatellite_info))
			break;

		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.bdsatellite_info[index].prn = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.bdsatellite_info[index].prn = atoi((char *)&_buff[aPtr]);

		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.bdsatellite_info[index].elevation = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.bdsatellite_info[index].elevation = atoi((char *)&_buff[aPtr]);



		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.bdsatellite_info[index].azimuthrange = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.bdsatellite_info[index].azimuthrange = atoi((char *)&_buff[aPtr]);



		aPtr += search_near_char(&_buff[aPtr], _len - aPtr, ',');
		if (aPtr >= _len)
			break;
		else if (_buff[aPtr] == ',')
		{
			g_nema_satellite_work_stat.bdsatellite_info[index].snr = 0;
			//aPtr--;
		}
		else
			g_nema_satellite_work_stat.bdsatellite_info[index].snr = atoi((char *)&_buff[aPtr]);

	}

//	t1 = t1;
//	t2 = t2;
}

/**
*******************************************************************************
 @brief 解析$--GSV前缀报文数据
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_rmc(uint8_t *_buff,uint16_t _len)
{
	static uint8_t sCount = 0,i=0;
	uint16_t aPtr= 0;
	nema_rmc_info_t *nema_rmc_info;

	//uint8_t m_yy,m_mm,m_dd,m_h,m_m,m_s;
	struct tm time_data;
	memset(&time_data, 0, sizeof(struct tm));

	nema_rmc_info = &g_nema_position_info.rmc_info_buf[g_nema_position_info.buf_head];
	//memset ((uint8_t*)nema_rmc_info,0,sizeof (nema_rmc_info));
	aPtr = 0;

	//parse_nmea_rmc((char*)_buff);
	g_nema_satellite_work_stat.work_stat = GNSS_DEV_WORK_NORMAL;
	g_nema_satellite_work_stat.Power_timer = 0;
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
	aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
    nema_rmc_info->posflags |=  NEMA_F_ISCONNECT;
    //nema_rmc_info->posflags |=  NEMA_F_ISCONNECT;
	if(_buff[aPtr] == 'A') /**>数据定位 */
	{
		if (sCount < 3) /**>过滤前面3条报文 */
		{
			sCount++;
			return;
		}
        
		nema_rmc_info->posflags |=  NEMA_F_VALIDMASK;
		aPtr = 0;
		/**>标识码 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;

		/**>时间 */
		if(_buff[aPtr+6] != '.')
			return;
		time_data.tm_hour = (_buff[aPtr]-'0')*10 + (_buff[aPtr+1]-'0');
		time_data.tm_min = (_buff[aPtr+2]-'0')*10 + (_buff[aPtr+3]-'0');
		time_data.tm_sec = (_buff[aPtr+4]-'0')*10 + (_buff[aPtr+5]-'0');

		/**> 定位状态 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;

		/**>纬度 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
        //memset(nema_rmc_info->latiCh,0x00,sizeof(nema_rmc_info->latiCh));
        strncpy((char *)nema_rmc_info->latiCh, (char *)&_buff[aPtr], 
				sizeof(nema_rmc_info->latiCh));
        nema_rmc_info->latiCh[sizeof(nema_rmc_info->latiCh)-1]=0;
		nema_rmc_info->lati  = pos_lat_iconvert(&_buff[aPtr]);
		nema_rmc_info->lati2 = pos_lat_iconvert2(&_buff[aPtr]);
		/**> 纬度半球 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
		if(_buff[aPtr] == 'S')
		{
			nema_rmc_info->lati  = ~nema_rmc_info->lati+1;
			nema_rmc_info->lati |= 0x80000000;
			nema_rmc_info->lati2  = ~nema_rmc_info->lati2+1;
			nema_rmc_info->lati2 |= 0x80000000;
		}

		/**>经度 */
		
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
        //memset(nema_rmc_info->longiCh,0x00,sizeof(nema_rmc_info->longiCh));
        strncpy((char *)nema_rmc_info->longiCh, (char *)&_buff[aPtr], 
				sizeof(nema_rmc_info->longiCh));

        nema_rmc_info->longiCh[sizeof(nema_rmc_info->longiCh)-1]=0;
		nema_rmc_info->longi  = pos_long_iconvert(&_buff[aPtr]);
		nema_rmc_info->longi2 = pos_long_iconvert2(&_buff[aPtr]);

		/**> 经度半球*/
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
		if(_buff[aPtr] == 'W')
		{
			nema_rmc_info->longi  = ~nema_rmc_info->longi+1;
			nema_rmc_info->longi |= 0x80000000;
			nema_rmc_info->longi2  = ~nema_rmc_info->longi2+1;
			nema_rmc_info->longi2 |= 0x80000000;
		}

		/**> 地面速率 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
		nema_rmc_info->speed = 0;
        i=0;
        //memset(nema_rmc_info->speedCh,0x00,sizeof(nema_rmc_info->speedCh));
		while(1)
		{
			if(_buff[aPtr] == ',')
			{
				nema_rmc_info->speed *= 100; /**> 扩大一倍*/
				nema_rmc_info->speedCh[i] = 0;
				break;
			}
			if(_buff[aPtr] == '.')
			{
				aPtr++;
                nema_rmc_info->speedCh[i] = _buff[aPtr];
                i++;
				nema_rmc_info->speed *= 100; /**>扩大一倍 */
				if(_buff[aPtr] == ',')
					break;
				if((_buff[aPtr] >= '0') && (_buff[aPtr] <= '9'))
					nema_rmc_info->speed += (_buff[aPtr] - '0')*10;
				aPtr++;
				if(_buff[aPtr] == ',')
					break;
				if((_buff[aPtr] >= '0') && (_buff[aPtr] <= '9'))
					nema_rmc_info->speed += (_buff[aPtr] - '0');
				break;
			}
			if((_buff[aPtr] >= '0') && (_buff[aPtr] <= '9'))
			{
				nema_rmc_info->speed *= 10;
				nema_rmc_info->speed += (_buff[aPtr]-'0');
                nema_rmc_info->speedCh[i] = _buff[aPtr];
                i++;
			}
			aPtr++;
			if(aPtr >= _len)
				break;
		}

		nema_rmc_info->direct = 0;
		/**> 地面航向 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
        i=0;
        //memset(nema_rmc_info->directCh,0x00,sizeof(nema_rmc_info->directCh));
		while(1)
		{
			if(_buff[aPtr] == ',')
			{
			    nema_rmc_info->directCh[i] = 0;
				break;
			}
            if(_buff[aPtr] == '.')
            {
                nema_rmc_info->directCh[i] = _buff[aPtr];
                i++;
                aPtr++;
                nema_rmc_info->directCh[i] = _buff[aPtr];
                break;
            }
			if((_buff[aPtr] >= '0') && (_buff[aPtr] <= '9'))
			{
				nema_rmc_info->direct *= 10;
				nema_rmc_info->direct += (_buff[aPtr]-'0');
                nema_rmc_info->directCh[i] = _buff[aPtr];
                i++;
			}
			aPtr++;
			if(aPtr >= _len)
				break;
		}

		/**> 日期 */
		aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
		if(aPtr >= _len)
			return;
		time_data.tm_mday = (_buff[aPtr]-'0')*10   + (_buff[aPtr+1]-'0');
		time_data.tm_mon  = (_buff[aPtr+2]-'0')*10 + (_buff[aPtr+3]-'0') - 1;
		time_data.tm_year = (_buff[aPtr+4]-'0')*10 + (_buff[aPtr+5]-'0') + 100;
		nema_rmc_info->SecondTimer = mktime(&time_data) - 339973888;
	}
	else
	{
		g_nema_satellite_work_stat.total_bd_satellite  = 0; 
		g_nema_satellite_work_stat.total_gps_satellite = 0; 
		nema_rmc_info->SecondTimer = 0;
		nema_rmc_info->posflags &=  ~NEMA_F_VALIDMASK;;
		sCount++;	
	}
}

/**
*******************************************************************************
 @brief 解析$GNVTG前缀报文数据
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_vtg(uint8_t *_buff,uint16_t _len)
{
    uint16_t aPtr= 0,tmp = 0;
    nema_vtg_info_t *atgm332d_VtgInfo= &g_nema_position_info.vtg_info_buf[g_nema_position_info.buf_head];
    memset ((uint8_t*)atgm332d_VtgInfo,0,sizeof (nema_vtg_info_t));
    uint8_t strTmp[6]={0};


    aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
	if(aPtr >= _len)
		return;
    if(_buff[aPtr]!=',')
    {
        tmp = 0;
        memset(strTmp,0x00,sizeof(strTmp));
        while(1)
        {
            if(_buff[aPtr] == ',')
                break;
            strTmp[tmp++] += _buff[aPtr++];
        }
        atgm332d_VtgInfo->actualNorth = atof((char *)strTmp)*100;
        
        aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
        if(aPtr >= _len)
            return;
        if(_buff[aPtr++]=='T')
        {
            aPtr++;
            tmp = 0;
            memset(strTmp,0x00,sizeof(strTmp));
            while(1)
            {
                if(_buff[aPtr] == ',')
                    break;
                strTmp[tmp++] += _buff[aPtr++];
            }
            atgm332d_VtgInfo->magneticNoth = atof((char *)strTmp)*100;        
        }
        
        aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
        if(aPtr >= _len)
            return;
        if(_buff[aPtr++]=='M')
        {
            aPtr++;
            tmp = 0;
            memset(strTmp,0x00,sizeof(strTmp));
            while(1)
            {
                if(_buff[aPtr] == ',')
                    break;
                strTmp[tmp++] = _buff[aPtr++];
                //if(_buff[aPtr] == '.')
                //    aPtr++;
            }
            atgm332d_VtgInfo->nauticalmile = atof((char *)strTmp)*100; 
        }
        
        aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
        if(aPtr >= _len)
            return;
        if(_buff[aPtr++]=='N')
        {
            aPtr++;
            tmp = 0;
            memset(strTmp, 0x00, sizeof(strTmp));
            while(1)
            {
                if(_buff[aPtr] == ',')
                    break;
                strTmp[tmp++] += _buff[aPtr++];
            }
            atgm332d_VtgInfo->kilometre = atof((char *)strTmp)*100; 
        }
        
        aPtr += search_near_char(&_buff[aPtr],_len-aPtr,',');
        if(aPtr >= _len)
            return;
        if(_buff[aPtr++]=='K')
        {
           aPtr++;
           atgm332d_VtgInfo->modeIndication =  _buff[aPtr++];
        }
    } 
}

/**
*******************************************************************************
 @brief 解析$GPTXT前缀报文数据
 
 @param _buff 报文数据
 @param _len 报文长度
*******************************************************************************
*/
void nema_parse_txt(uint8_t *_buff,uint16_t _len)
{
	// NEMA_LOG("deal TXT\n");
	uint32_t m_ptr,m_i;
	m_ptr = 0;
	for(m_i = 0; m_i < 4; m_i++)
	{
		m_ptr += search_near_char(&_buff[m_ptr],_len-m_ptr,',');
		if(m_ptr >= _len)
			return;
	}
	if(memcmp(&_buff[m_ptr],"ANTENNA OPEN",12) == 0)
	{
		g_nema_satellite_work_stat.ant_stat = GNSS_DEV_ANT_OPENCIRCUIT;
	}
	else if(memcmp(&_buff[m_ptr],"ANTENNA SHORT",13) == 0)
	{
		g_nema_satellite_work_stat.ant_stat = GNSS_DEV_ANT_SHORTCIRCUIT;
	}
	else if(memcmp(&_buff[m_ptr],"ANTENNA OK",10) == 0)
	{
		g_nema_satellite_work_stat.ant_stat = GNSS_DEV_ANT_NORMAL;
	}

	if(g_nema_position_info.buf_count < NEMA_INFO_BUFNUM_MAX)
	{
		g_nema_position_info.buf_count++;
	}
	else
	{
		g_nema_position_info.buf_count = NEMA_INFO_BUFNUM_MAX;
	}
	g_nema_position_info.buf_head = (g_nema_position_info.buf_head + 1) % NEMA_INFO_BUFNUM_MAX;
}

/**
*******************************************************************************
 @brief 报文解析集合
 
 @param 报文前缀
 @param 报文处理函数
*******************************************************************************
*/
const nema_parse_assemble_t nema_parse_ass[] =
{   
    {"$GPGGA",nema_parse_gga},   /**>0 */
    {"$BDGGA",nema_parse_gga},   /**>1 */
    {"$GNGGA",nema_parse_gga},   /**>2 */
    {"$GNGSA",nema_parse_gsa},   /**>3 */
    {"$GPGSV",nema_parse_gpgsv}, /**>4 */
    {"$BDGSV",nema_parse_bdgsv}, /**>5 */
    {"$GPRMC",nema_parse_rmc},   /**>6 */
    {"$BDRMC",nema_parse_rmc},   /**>7 */
    {"$GNRMC",nema_parse_rmc},   /**>8 */
    {"$GNVTG",nema_parse_vtg},   /**>9 */
    {"$GPTXT",nema_parse_txt},   /**>10*/
  
};

/**
*******************************************************************************
 @brief 上报解析失败数据
 
 @param _buf数据体
 @param _len数据长度
*******************************************************************************
*/
static void nema_error_data_cb(char *_buf,uint16_t _len)
{
//	NEMA_ERR("ERROR: can relize buf len:%d,data:%s",_len, _buf);
//    if(atgm332d_error_cb)
//        atgm332d_error_cb(_buf,_len);
}



/**
*******************************************************************************
 @brief 上报解析后数据
 
 @param 
 @param 
*******************************************************************************
*/
void nema_report()
{
	/**>暂未实现,后续打算以回调方法实现*/
}


/**
*******************************************************************************
 @brief NEMA报文解析
 
 @param buf NEMA报文
 @param buf_len NEMA报文长度
*******************************************************************************
*/
void nema_parse_data(char *buf, int buf_len)
{
	uint8_t i;
	uint16_t check;

	while(buf_len)
	{
		 /**>--接收过程中收到起始符后重新开始接收 */
		if((g_nema_rcv.state != NEMA_INPARSE_IDLE) && ((*buf) == '$'))
			g_nema_rcv.state = NEMA_INPARSE_IDLE;

		switch(g_nema_rcv.state)
		{
			case NEMA_INPARSE_IDLE:
				if((*buf) == '$') /**>---接收开标记 */
				{
					g_nema_rcv.state  = NEMA_INPARSE_REV_HEAD;
					g_nema_rcv.rcv_len = 0;
					g_nema_rcv.check  = 0;
					g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len++] = '$';
				}
                else
                    nema_error_data_cb(buf,1);
				break;

			case NEMA_INPARSE_REV_HEAD:  /**>----接收报文头*/
				if((g_nema_rcv.rcv_len > NEMA_FRAME_HEAD_LEN) || \
				 ((*buf) < 0x20) || ((*buf) > 0x7e))
				{
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
                    nema_error_data_cb(buf,1);
				}
				else
				{
					g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len] = (*buf);
					g_nema_rcv.check ^= g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len];
					g_nema_rcv.rcv_len++;
					if((*buf) == ',')
					{
						for(i = 0; i < _countof(nema_parse_ass); i++)  /**>---检测是否需要处理 */
						{
							if(memcmp (nema_parse_ass[i].frame_head, g_nema_rcv.rcvbuf, strlen((char*)nema_parse_ass[i].frame_head)) == 0)
							{
								if(strlen((char*)nema_parse_ass[i].frame_head) == (g_nema_rcv.rcv_len-1))
								{
									g_nema_rcv.frame_type = i;
									g_nema_rcv.state = NEMA_INPARSE_REV_CHACK_FLAG;
									break;
								}
							}
						}
						if(i == _countof(nema_parse_ass))
						{
							g_nema_rcv.state  = NEMA_INPARSE_IDLE;
							g_nema_rcv.rcv_len = 0;
                            nema_error_data_cb(buf,1);
						}
					}
				}
				break;

			case NEMA_INPARSE_REV_CHACK_FLAG: /*----校验码标记 */

				if((g_nema_rcv.rcv_len >= NEMA_FRAME_MAX) || \
				 ((*buf) < 0x20) || ((*buf) > 0x7e))
				{
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
                    nema_error_data_cb(buf,1);
				}
				else
				{
					g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len] = (*buf);
					if((*buf) == '*')
						g_nema_rcv.state = NEMA_INPARSE_REV_CHACK_MSB;
					else
						g_nema_rcv.check ^= g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len];
					g_nema_rcv.rcv_len++;
				}
				break;

			case  NEMA_INPARSE_REV_CHACK_MSB: /*---校验码 */
			case  NEMA_INPARSE_REV_CHACK_LSB:
				if((g_nema_rcv.rcv_len >= NEMA_FRAME_MAX) || \
				 ((((*buf) < '0') || ((*buf) > '9')) && (((*buf) < 'A') || ((*buf) > 'F'))))
				{
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
				}
				else
				{
					g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len] = (*buf);
					g_nema_rcv.rcv_len++;
				}
				if(g_nema_rcv.state == NEMA_INPARSE_REV_CHACK_MSB)
					g_nema_rcv.state = NEMA_INPARSE_REV_CHACK_LSB;
				else
					g_nema_rcv.state = NEMA_INPARSE_REV_CR;
				break;
			case NEMA_INPARSE_REV_CR:
				if((*buf) != '\r')
				{
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
                    nema_error_data_cb(buf,1);
				}
				else
					g_nema_rcv.state = NEMA_INPARSE_REV_LF;
				break;

			case NEMA_INPARSE_REV_LF:
				if((*buf) != '\n')
				{
                    g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len] = (*buf);
					g_nema_rcv.rcv_len++;
                    
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
                    nema_error_data_cb(buf,1);
				}
				else
				{
					if((g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-1] >= '0') && \
					 (g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-1] <= '9'))
						check = g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-1] - '0';
					else
						check = g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-1] - 'A' + 0x0A;

					if((g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-2] >= '0') && \
					 (g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-2] <= '9'))
						check |= (g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-2] - '0') << 4;
					else
						check |= (g_nema_rcv.rcvbuf[g_nema_rcv.rcv_len-2] - 'A' + 0x0A) << 4;

					if(check == g_nema_rcv.check) /*---调用解析函数 */
                    {
                        nema_parse_ass[g_nema_rcv.frame_type].frame_Parse(g_nema_rcv.rcvbuf,g_nema_rcv.rcv_len);
                        
                    }
                    else
                        nema_error_data_cb((char *)g_nema_rcv.rcvbuf,g_nema_rcv.rcv_len);
						
					g_nema_rcv.state  = NEMA_INPARSE_IDLE;
					g_nema_rcv.rcv_len = 0;
				}
				break;
		}
		if(buf_len != 0)
			buf_len--;
		buf++;
	}
}
#if 0
void show_result(void)
{
	uint8_t    gps_utc[8];
	uint8_t    gps_speed[2];
	uint8_t    gps_accuracy[1];
	uint8_t    gps_num[1];
	uint8_t    gps_alt[8];
	uint8_t    gps_bearing[2];
	uint8_t    gps_long[8];
	uint8_t    gps_lat[8];

	nema_rmc_info_t *RMC_PosInfo = &g_nema_position_info.rmc_info_buf[g_nema_position_info.buf_head];
	nema_gsa_info_t *GSA_PosInfo = &g_nema_position_info.gsa_info_buf[g_nema_position_info.buf_head];

	sprintf(gps_utc, "%x", RMC_PosInfo->SecondTimer);
	NEMA_LOG("time: %d--%s\n", RMC_PosInfo->SecondTimer, gps_utc);
	//速率
	RMC_PosInfo->speed /= 100;
	sprintf(gps_speed, "%x", RMC_PosInfo->speed);
	NEMA_LOG("speed: 0x%x--%s\n",RMC_PosInfo->speed, gps_speed);
	//精度
	sprintf(gps_accuracy, "%x", GSA_PosInfo->PDOP);
	NEMA_LOG("accuracy: 0x%x--%s\n",GSA_PosInfo->PDOP, gps_accuracy);
	//航向角
	sprintf(gps_bearing, "%x", RMC_PosInfo->direct);
	NEMA_LOG("bearing: 0x%x--%s\n",RMC_PosInfo->direct, gps_bearing);
	//经度
	sprintf(gps_long, "%x", RMC_PosInfo->longi2);
	NEMA_LOG("longi: %d--%s\n",RMC_PosInfo->longi2, gps_long);
	//纬度
	sprintf(gps_lat, "%x", RMC_PosInfo->lati2);
	NEMA_LOG("lati: %d--%s\n",RMC_PosInfo->lati2, gps_lat);
	//卫星数
	sprintf(gps_num, "%x", g_nema_satellite_work_stat.total_pos_satellite);
	NEMA_LOG("start num:%d--%s\n", g_nema_satellite_work_stat.total_pos_satellite, gps_num);
	//海拔高度
	sprintf(gps_alt, "%x", g_nema_satellite_work_stat.altitude);
	NEMA_LOG("alt: 0x%x--%s\n", g_nema_satellite_work_stat.altitude, gps_alt);
}

#endif

