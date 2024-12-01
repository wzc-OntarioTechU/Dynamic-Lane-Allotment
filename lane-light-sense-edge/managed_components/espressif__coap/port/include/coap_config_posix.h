/*
 * libcoap configure implementation for ESP32 platform.
 *
 * Uses libcoap software implementation for failover when concurrent
 * configure operations are in use.
 *
 * coap.h -- main header file for CoAP stack of libcoap
 *
 * Copyright (C) 2010-2012,2015-2024 Olaf Bergmann <bergmann@tzi.org>
 *               2015 Carsten Schoenert <c.schoenert@t-online.de>
 *
 * Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef COAP_CONFIG_POSIX_H_
#define COAP_CONFIG_POSIX_H_

#ifdef WITH_POSIX

#include <sys/socket.h>
#include <sys/un.h>
#include <net/if.h>
#include "lwip/init.h"

#define HAVE_SYS_SOCKET_H
#define HAVE_MALLOC
#define HAVE_ARPA_INET_H
#define HAVE_TIME_H
#define HAVE_NETDB_H
#define HAVE_NETINET_IN_H
#define HAVE_STRUCT_CMSGHDR
#define HAVE_PTHREAD_H
#define HAVE_PTHREAD_MUTEX_LOCK
#define HAVE_GETRANDOM

#define ipi_spec_dst ipi_addr
struct in6_pktinfo {
    struct in6_addr ipi6_addr;        /* src/dst IPv6 address */
    unsigned int ipi6_ifindex;        /* send/recv interface index */
};

#if LWIP_VERSION < 0x02020000
#define IN6_IS_ADDR_V4MAPPED(a) \
        ((((__const uint32_t *) (a))[0] == 0)                                 \
         && (((__const uint32_t *) (a))[1] == 0)                              \
         && (((__const uint32_t *) (a))[2] == htonl (0xffff)))
#endif // LWIP_VERSION < 0x02020000

/* As not defined, just need to define is as something innocuous */
#define IPV6_PKTINFO IPV6_CHECKSUM

#define PACKAGE_NAME "libcoap-posix"
#define PACKAGE_VERSION "4.3.5"

#ifdef CONFIG_MBEDTLS_TLS_ENABLED
#define COAP_WITH_LIBMBEDTLS
#endif /* CONFIG_MBEDTLS_TLS_ENABLED */

#define COAP_DEFAULT_MAX_PDU_RX_SIZE CONFIG_LWIP_TCP_MSL

#define COAP_CONSTRAINED_STACK 1
#define ESPIDF_VERSION

#define gai_strerror(x) "gai_strerror() not supported"

#endif /* WITH_POSIX */
#endif /* COAP_CONFIG_POSIX_H_ */
