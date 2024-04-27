/**
 * @file endian.h
 * @brief 字节序操作函数(大端/小端)
 * @author tcs.yin
 * @email 564628276@qq.com
 * @date 2019-06-01
 * @copyright Copyright (c) 2019年 tcs.yin All rights reserved (www.tcs.top)
 */
extern "C++" {
#ifndef __TCS_ENDIAN_H__
#define __TCS_ENDIAN_H__

#define TCS_LITTLE_ENDIAN 1
#define TCS_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>
#include <iostream>

    namespace tcs {
        /**
         * @brief 8字节类型的字节序转化
         */
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
            byteswap(T value) {
            return (T)bswap_64((uint64_t)value);
        }

        /**
         * @brief 4字节类型的字节序转化
         */
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
            byteswap(T value) {
            return (T)bswap_32((uint32_t)value);
        }

        /**
         * @brief 2字节类型的字节序转化
         */
        template<class T>
        typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
            byteswap(T value) {
            return (T)bswap_16((uint16_t)value);
        }

#if BYTE_ORDER == BIG_ENDIAN
#define TCS_BYTE_ORDER TCS_BIG_ENDIAN
#else
#define TCS_BYTE_ORDER TCS_LITTLE_ENDIAN
#endif

#if TCS_BYTE_ORDER == TCS_BIG_ENDIAN

        /**
         * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
         */
        template<class T>
        T byteswapOnLittleEndian(T t) {
            return t;
        }

        /**
         * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
         */
        template<class T>
        T byteswapOnBigEndian(T t) {
            return byteswap(t);
        }
#else

        /**
         * @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
         */
        template<class T>
        T byteswapOnLittleEndian(T t) {
            return byteswap(t);
        }

        /**
         * @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
         */
        template<class T>
        T byteswapOnBigEndian(T t) {
            return t;
        }
#endif

    }

#endif

}
