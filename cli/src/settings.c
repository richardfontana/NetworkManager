/* nmcli - command-line tool to control NetworkManager
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2010 - 2012 Red Hat, Inc.
 */

#include "config.h"

#include "net/if_arp.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <libnm-util/nm-utils.h>

#include "utils.h"
#include "settings.h"


/* Helper macro to define fields */
#define SETTING_FIELD(setting, width) { setting, N_(setting), width, NULL, 0 }

/* Available fields for NM_SETTING_CONNECTION_SETTING_NAME */
static NmcOutputField nmc_fields_setting_connection[] = {
	SETTING_FIELD ("name",  15),                              /* 0 */
	SETTING_FIELD (NM_SETTING_CONNECTION_ID, 25),             /* 1 */
	SETTING_FIELD (NM_SETTING_CONNECTION_UUID, 38),           /* 2 */
	SETTING_FIELD (NM_SETTING_CONNECTION_INTERFACE_NAME, 20), /* 3 */
	SETTING_FIELD (NM_SETTING_CONNECTION_TYPE, 17),           /* 4 */
	SETTING_FIELD (NM_SETTING_CONNECTION_AUTOCONNECT, 13),    /* 5 */
	SETTING_FIELD (NM_SETTING_CONNECTION_TIMESTAMP, 10),      /* 6 */
	SETTING_FIELD (NM_SETTING_CONNECTION_READ_ONLY, 10),      /* 7 */
	SETTING_FIELD (NM_SETTING_CONNECTION_PERMISSIONS, 30),    /* 8 */
	SETTING_FIELD (NM_SETTING_CONNECTION_ZONE, 10),           /* 9 */
	SETTING_FIELD (NM_SETTING_CONNECTION_MASTER, 20),         /* 10 */
	SETTING_FIELD (NM_SETTING_CONNECTION_SLAVE_TYPE, 20),     /* 11 */
	SETTING_FIELD (NM_SETTING_CONNECTION_SECONDARIES, 40),    /* 12 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_CONNECTION_ALL     "name"","\
                                              NM_SETTING_CONNECTION_ID","\
                                              NM_SETTING_CONNECTION_UUID","\
                                              NM_SETTING_CONNECTION_INTERFACE_NAME","\
                                              NM_SETTING_CONNECTION_TYPE","\
                                              NM_SETTING_CONNECTION_AUTOCONNECT","\
                                              NM_SETTING_CONNECTION_TIMESTAMP","\
                                              NM_SETTING_CONNECTION_READ_ONLY","\
                                              NM_SETTING_CONNECTION_PERMISSIONS","\
                                              NM_SETTING_CONNECTION_ZONE","\
                                              NM_SETTING_CONNECTION_MASTER","\
                                              NM_SETTING_CONNECTION_SLAVE_TYPE","\
                                              NM_SETTING_CONNECTION_SECONDARIES
#define NMC_FIELDS_SETTING_CONNECTION_COMMON  NMC_FIELDS_SETTING_CONNECTION_ALL

/* Available fields for NM_SETTING_WIRED_SETTING_NAME */
static NmcOutputField nmc_fields_setting_wired[] = {
	SETTING_FIELD ("name",  17),                                  /* 0 */
	SETTING_FIELD (NM_SETTING_WIRED_PORT, 8),                     /* 1 */
	SETTING_FIELD (NM_SETTING_WIRED_SPEED, 10),                   /* 2 */
	SETTING_FIELD (NM_SETTING_WIRED_DUPLEX, 10),                  /* 3 */
	SETTING_FIELD (NM_SETTING_WIRED_AUTO_NEGOTIATE, 15),          /* 4 */
	SETTING_FIELD (NM_SETTING_WIRED_MAC_ADDRESS, 19),             /* 5 */
	SETTING_FIELD (NM_SETTING_WIRED_CLONED_MAC_ADDRESS, 19),      /* 6 */
	SETTING_FIELD (NM_SETTING_WIRED_MAC_ADDRESS_BLACKLIST, 39),   /* 7 */
	SETTING_FIELD (NM_SETTING_WIRED_MTU, 6),                      /* 8 */
	SETTING_FIELD (NM_SETTING_WIRED_S390_SUBCHANNELS, 20),        /* 9 */
	SETTING_FIELD (NM_SETTING_WIRED_S390_NETTYPE, 15),            /* 10 */
	SETTING_FIELD (NM_SETTING_WIRED_S390_OPTIONS, 20),            /* 11 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_WIRED_ALL     "name"","\
                                         NM_SETTING_WIRED_PORT","\
                                         NM_SETTING_WIRED_SPEED","\
                                         NM_SETTING_WIRED_DUPLEX","\
                                         NM_SETTING_WIRED_AUTO_NEGOTIATE","\
                                         NM_SETTING_WIRED_MAC_ADDRESS","\
                                         NM_SETTING_WIRED_CLONED_MAC_ADDRESS","\
                                         NM_SETTING_WIRED_MAC_ADDRESS_BLACKLIST","\
                                         NM_SETTING_WIRED_MTU","\
                                         NM_SETTING_WIRED_S390_SUBCHANNELS","\
                                         NM_SETTING_WIRED_S390_NETTYPE","\
                                         NM_SETTING_WIRED_S390_OPTIONS
#define NMC_FIELDS_SETTING_WIRED_COMMON  NMC_FIELDS_SETTING_WIRED_ALL

/* Available fields for NM_SETTING_802_1X_SETTING_NAME */
static NmcOutputField nmc_fields_setting_8021X[] = {
	SETTING_FIELD ("name", 10),                                              /* 0 */
	SETTING_FIELD (NM_SETTING_802_1X_EAP, 10),                               /* 1 */
	SETTING_FIELD (NM_SETTING_802_1X_IDENTITY, 15),                          /* 2 */
	SETTING_FIELD (NM_SETTING_802_1X_ANONYMOUS_IDENTITY, 15),                /* 3 */
	SETTING_FIELD (NM_SETTING_802_1X_PAC_FILE, 15),                          /* 4 */
	SETTING_FIELD (NM_SETTING_802_1X_CA_CERT, 10),                           /* 5 */
	SETTING_FIELD (NM_SETTING_802_1X_CA_PATH, 10),                           /* 6 */
	SETTING_FIELD (NM_SETTING_802_1X_SUBJECT_MATCH, 10),                     /* 7 */
	SETTING_FIELD (NM_SETTING_802_1X_ALTSUBJECT_MATCHES, 10),                /* 8 */
	SETTING_FIELD (NM_SETTING_802_1X_CLIENT_CERT, 10),                       /* 9 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE1_PEAPVER, 10),                    /* 10 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE1_PEAPLABEL, 10),                  /* 11 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE1_FAST_PROVISIONING, 10),          /* 12 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_AUTH, 10),                       /* 13 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_AUTHEAP, 10),                    /* 14 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_CA_CERT, 20),                    /* 15 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_CA_PATH, 20),                    /* 16 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_SUBJECT_MATCH, 10),              /* 17 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_ALTSUBJECT_MATCHES, 10),         /* 18 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_CLIENT_CERT, 20),                /* 19 */
	SETTING_FIELD (NM_SETTING_802_1X_PASSWORD, 10),                          /* 20 */
	SETTING_FIELD (NM_SETTING_802_1X_PASSWORD_FLAGS, 20),                    /* 21 */
	SETTING_FIELD (NM_SETTING_802_1X_PASSWORD_RAW, 20),                      /* 22 */
	SETTING_FIELD (NM_SETTING_802_1X_PASSWORD_RAW_FLAGS, 20),                /* 23 */
	SETTING_FIELD (NM_SETTING_802_1X_PRIVATE_KEY, 15),                       /* 24 */
	SETTING_FIELD (NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD, 20),              /* 25 */
	SETTING_FIELD (NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD_FLAGS, 20),        /* 26 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_PRIVATE_KEY, 20),                /* 27 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD, 20),       /* 28 */
	SETTING_FIELD (NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD_FLAGS, 20), /* 29 */
	SETTING_FIELD (NM_SETTING_802_1X_PIN, 8),                                /* 30 */
	SETTING_FIELD (NM_SETTING_802_1X_PIN_FLAGS, 20),                         /* 31 */
	SETTING_FIELD (NM_SETTING_802_1X_SYSTEM_CA_CERTS, 17),                   /* 32 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_802_1X_ALL     "name"","\
                                          NM_SETTING_802_1X_EAP","\
                                          NM_SETTING_802_1X_IDENTITY","\
                                          NM_SETTING_802_1X_ANONYMOUS_IDENTITY","\
                                          NM_SETTING_802_1X_PAC_FILE","\
                                          NM_SETTING_802_1X_CA_CERT","\
                                          NM_SETTING_802_1X_CA_PATH","\
                                          NM_SETTING_802_1X_SUBJECT_MATCH","\
                                          NM_SETTING_802_1X_ALTSUBJECT_MATCHES","\
                                          NM_SETTING_802_1X_CLIENT_CERT","\
                                          NM_SETTING_802_1X_PHASE1_PEAPVER","\
                                          NM_SETTING_802_1X_PHASE1_PEAPLABEL","\
                                          NM_SETTING_802_1X_PHASE1_FAST_PROVISIONING","\
                                          NM_SETTING_802_1X_PHASE2_AUTH","\
                                          NM_SETTING_802_1X_PHASE2_AUTHEAP","\
                                          NM_SETTING_802_1X_PHASE2_CA_CERT","\
                                          NM_SETTING_802_1X_PHASE2_CA_PATH","\
                                          NM_SETTING_802_1X_PHASE2_SUBJECT_MATCH","\
                                          NM_SETTING_802_1X_PHASE2_ALTSUBJECT_MATCHES","\
                                          NM_SETTING_802_1X_PHASE2_CLIENT_CERT","\
                                          NM_SETTING_802_1X_PASSWORD","\
                                          NM_SETTING_802_1X_PASSWORD_FLAGS","\
                                          NM_SETTING_802_1X_PASSWORD_RAW","\
                                          NM_SETTING_802_1X_PASSWORD_RAW_FLAGS","\
                                          NM_SETTING_802_1X_PRIVATE_KEY","\
                                          NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD","\
                                          NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD_FLAGS","\
                                          NM_SETTING_802_1X_PHASE2_PRIVATE_KEY","\
                                          NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD","\
                                          NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD_FLAGS","\
                                          NM_SETTING_802_1X_PIN","\
                                          NM_SETTING_802_1X_PIN_FLAGS","\
                                          NM_SETTING_802_1X_SYSTEM_CA_CERTS
#define NMC_FIELDS_SETTING_802_1X_COMMON  NMC_FIELDS_SETTING_802_1X_ALL

/* Available fields for NM_SETTING_WIRELESS_SETTING_NAME */
static NmcOutputField nmc_fields_setting_wireless[] = {
	SETTING_FIELD ("name", 17),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SSID, 34),                      /* 1 */
	SETTING_FIELD (NM_SETTING_WIRELESS_MODE, 15),                      /* 2 */
	SETTING_FIELD (NM_SETTING_WIRELESS_BAND, 10),                      /* 3 */
	SETTING_FIELD (NM_SETTING_WIRELESS_CHANNEL, 10),                   /* 4 */
	SETTING_FIELD (NM_SETTING_WIRELESS_BSSID, 19),                     /* 5 */
	SETTING_FIELD (NM_SETTING_WIRELESS_RATE, 10),                      /* 6 */
	SETTING_FIELD (NM_SETTING_WIRELESS_TX_POWER, 10),                  /* 7 */
	SETTING_FIELD (NM_SETTING_WIRELESS_MAC_ADDRESS, 19),               /* 8 */
	SETTING_FIELD (NM_SETTING_WIRELESS_CLONED_MAC_ADDRESS, 19),        /* 9 */
	SETTING_FIELD (NM_SETTING_WIRELESS_MAC_ADDRESS_BLACKLIST, 39),     /* 10 */
	SETTING_FIELD (NM_SETTING_WIRELESS_MTU, 6),                        /* 11 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SEEN_BSSIDS, 35),               /* 12 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SEC, 25),                       /* 13 */
	SETTING_FIELD (NM_SETTING_WIRELESS_HIDDEN, 10),                    /* 14 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_WIRELESS_ALL     "name"","\
                                            NM_SETTING_WIRELESS_SSID","\
                                            NM_SETTING_WIRELESS_MODE","\
                                            NM_SETTING_WIRELESS_BAND","\
                                            NM_SETTING_WIRELESS_CHANNEL","\
                                            NM_SETTING_WIRELESS_BSSID","\
                                            NM_SETTING_WIRELESS_RATE","\
                                            NM_SETTING_WIRELESS_TX_POWER","\
                                            NM_SETTING_WIRELESS_MAC_ADDRESS","\
                                            NM_SETTING_WIRELESS_CLONED_MAC_ADDRESS","\
                                            NM_SETTING_WIRELESS_MAC_ADDRESS_BLACKLIST","\
                                            NM_SETTING_WIRELESS_MTU","\
                                            NM_SETTING_WIRELESS_SEEN_BSSIDS","\
                                            NM_SETTING_WIRELESS_SEC","\
                                            NM_SETTING_WIRELESS_HIDDEN
#define NMC_FIELDS_SETTING_WIRELESS_COMMON  NMC_FIELDS_SETTING_WIRELESS_ALL

/* Available fields for NM_SETTING_WIRELESS_SECURITY_SETTING_NAME */
static NmcOutputField nmc_fields_setting_wireless_security[] = {
	SETTING_FIELD ("name", 25),                                           /* 0 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, 10),            /* 1 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX, 15),       /* 2 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_AUTH_ALG, 10),            /* 3 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_PROTO, 10),               /* 4 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_PAIRWISE, 10),            /* 5 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_GROUP, 10),               /* 6 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_LEAP_USERNAME, 15),       /* 7 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY0, 10),            /* 8 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY1, 10),            /* 9 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY2, 10),            /* 10 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY3, 10),            /* 11 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY_FLAGS, 20),       /* 12 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE, 15),        /* 13 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_PSK, 6),                  /* 14 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_PSK_FLAGS, 20),           /* 15 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD, 15),       /* 16 */
	SETTING_FIELD (NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD_FLAGS, 20), /* 17 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_WIRELESS_SECURITY_ALL     "name"","\
                                                     NM_SETTING_WIRELESS_SECURITY_KEY_MGMT","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX","\
                                                     NM_SETTING_WIRELESS_SECURITY_AUTH_ALG","\
                                                     NM_SETTING_WIRELESS_SECURITY_PROTO","\
                                                     NM_SETTING_WIRELESS_SECURITY_PAIRWISE","\
                                                     NM_SETTING_WIRELESS_SECURITY_GROUP","\
                                                     NM_SETTING_WIRELESS_SECURITY_LEAP_USERNAME","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY0","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY1","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY2","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY3","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY_FLAGS","\
                                                     NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE","\
                                                     NM_SETTING_WIRELESS_SECURITY_PSK","\
                                                     NM_SETTING_WIRELESS_SECURITY_PSK_FLAGS","\
                                                     NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD","\
                                                     NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD_FLAGS
#define NMC_FIELDS_SETTING_WIRELESS_SECURITY_COMMON  NMC_FIELDS_SETTING_WIRELESS_SECURITY_ALL

/* Available fields for NM_SETTING_IP4_CONFIG_SETTING_NAME */
static NmcOutputField nmc_fields_setting_ip4_config[] = {
	SETTING_FIELD ("name", 8),                                         /* 0 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_METHOD, 10),                  /* 1 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_DNS, 20),                     /* 2 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_DNS_SEARCH, 15),              /* 3 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_ADDRESSES, 20),               /* 4 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_ROUTES, 20),                  /* 5 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_IGNORE_AUTO_ROUTES, 19),      /* 6 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_IGNORE_AUTO_DNS, 16),         /* 7 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_DHCP_CLIENT_ID, 15),          /* 8 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_DHCP_SEND_HOSTNAME, 19),      /* 9 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_DHCP_HOSTNAME, 14),           /* 10 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_NEVER_DEFAULT, 15),           /* 11 */
	SETTING_FIELD (NM_SETTING_IP4_CONFIG_MAY_FAIL, 12),                /* 12 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_IP4_CONFIG_ALL     "name"","\
                                              NM_SETTING_IP4_CONFIG_METHOD","\
                                              NM_SETTING_IP4_CONFIG_DNS","\
                                              NM_SETTING_IP4_CONFIG_DNS_SEARCH","\
                                              NM_SETTING_IP4_CONFIG_ADDRESSES","\
                                              NM_SETTING_IP4_CONFIG_ROUTES","\
                                              NM_SETTING_IP4_CONFIG_IGNORE_AUTO_ROUTES","\
                                              NM_SETTING_IP4_CONFIG_IGNORE_AUTO_DNS","\
                                              NM_SETTING_IP4_CONFIG_DHCP_CLIENT_ID","\
                                              NM_SETTING_IP4_CONFIG_DHCP_SEND_HOSTNAME","\
                                              NM_SETTING_IP4_CONFIG_DHCP_HOSTNAME","\
                                              NM_SETTING_IP4_CONFIG_NEVER_DEFAULT","\
                                              NM_SETTING_IP4_CONFIG_MAY_FAIL
#define NMC_FIELDS_SETTING_IP4_CONFIG_COMMON  NMC_FIELDS_SETTING_IP4_CONFIG_ALL

/* Available fields for NM_SETTING_IP6_CONFIG_SETTING_NAME */
static NmcOutputField nmc_fields_setting_ip6_config[] = {
	SETTING_FIELD ("name", 8),                                         /* 0 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_METHOD, 10),                  /* 1 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_DNS, 20),                     /* 2 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_DNS_SEARCH, 15),              /* 3 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_ADDRESSES, 20),               /* 4 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_ROUTES, 20),                  /* 5 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_IGNORE_AUTO_ROUTES, 19),      /* 6 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_IGNORE_AUTO_DNS, 16),         /* 7 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_NEVER_DEFAULT, 15),           /* 8 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_MAY_FAIL, 12),                /* 9 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_IP6_PRIVACY, 15),             /* 10 */
	SETTING_FIELD (NM_SETTING_IP6_CONFIG_DHCP_HOSTNAME, 14),           /* 11 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_IP6_CONFIG_ALL     "name"","\
                                              NM_SETTING_IP6_CONFIG_METHOD","\
                                              NM_SETTING_IP6_CONFIG_DNS","\
                                              NM_SETTING_IP6_CONFIG_DNS_SEARCH","\
                                              NM_SETTING_IP6_CONFIG_ADDRESSES","\
                                              NM_SETTING_IP6_CONFIG_ROUTES","\
                                              NM_SETTING_IP6_CONFIG_IGNORE_AUTO_ROUTES","\
                                              NM_SETTING_IP6_CONFIG_IGNORE_AUTO_DNS","\
                                              NM_SETTING_IP6_CONFIG_NEVER_DEFAULT","\
                                              NM_SETTING_IP6_CONFIG_MAY_FAIL","\
                                              NM_SETTING_IP6_CONFIG_IP6_PRIVACY","\
                                              NM_SETTING_IP6_CONFIG_DHCP_HOSTNAME
#define NMC_FIELDS_SETTING_IP6_CONFIG_COMMON  NMC_FIELDS_SETTING_IP4_CONFIG_ALL

/* Available fields for NM_SETTING_SERIAL_SETTING_NAME */
static NmcOutputField nmc_fields_setting_serial[] = {
	SETTING_FIELD ("name", 10),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_SERIAL_BAUD, 10),                        /* 1 */
	SETTING_FIELD (NM_SETTING_SERIAL_BITS, 10),                        /* 2 */
	SETTING_FIELD (NM_SETTING_SERIAL_PARITY, 10),                      /* 3 */
	SETTING_FIELD (NM_SETTING_SERIAL_STOPBITS, 10),                    /* 4 */
	SETTING_FIELD (NM_SETTING_SERIAL_SEND_DELAY, 12),                  /* 5 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_SERIAL_ALL     "name"","\
                                          NM_SETTING_SERIAL_BAUD","\
                                          NM_SETTING_SERIAL_BITS","\
                                          NM_SETTING_SERIAL_PARITY","\
                                          NM_SETTING_SERIAL_STOPBITS","\
                                          NM_SETTING_SERIAL_SEND_DELAY
#define NMC_FIELDS_SETTING_SERIAL_COMMON  NMC_FIELDS_SETTING_SERIAL_ALL

/* Available fields for NM_SETTING_PPP_SETTING_NAME */
static NmcOutputField nmc_fields_setting_ppp[] = {
	SETTING_FIELD ("name", 10),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_PPP_NOAUTH, 10),                         /* 1 */
	SETTING_FIELD (NM_SETTING_PPP_REFUSE_EAP, 10),                     /* 2 */
	SETTING_FIELD (NM_SETTING_PPP_REFUSE_PAP, 10),                     /* 3 */
	SETTING_FIELD (NM_SETTING_PPP_REFUSE_CHAP, 10),                    /* 4 */
	SETTING_FIELD (NM_SETTING_PPP_REFUSE_MSCHAP, 10),                  /* 5 */
	SETTING_FIELD (NM_SETTING_PPP_REFUSE_MSCHAPV2, 10),                /* 6 */
	SETTING_FIELD (NM_SETTING_PPP_NOBSDCOMP, 10),                      /* 7 */
	SETTING_FIELD (NM_SETTING_PPP_NODEFLATE, 10),                      /* 8 */
	SETTING_FIELD (NM_SETTING_PPP_NO_VJ_COMP, 10),                     /* 9 */
	SETTING_FIELD (NM_SETTING_PPP_REQUIRE_MPPE, 10),                   /* 10 */
	SETTING_FIELD (NM_SETTING_PPP_REQUIRE_MPPE_128, 10),               /* 11 */
	SETTING_FIELD (NM_SETTING_PPP_MPPE_STATEFUL, 10),                  /* 12 */
	SETTING_FIELD (NM_SETTING_PPP_CRTSCTS, 10),                        /* 13 */
	SETTING_FIELD (NM_SETTING_PPP_BAUD, 10),                           /* 14 */
	SETTING_FIELD (NM_SETTING_PPP_MRU, 10),                            /* 15 */
	SETTING_FIELD (NM_SETTING_PPP_MTU, 10),                            /* 16 */
	SETTING_FIELD (NM_SETTING_PPP_LCP_ECHO_FAILURE, 17),               /* 17 */
	SETTING_FIELD (NM_SETTING_PPP_LCP_ECHO_INTERVAL, 18),              /* 18 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_PPP_ALL     "name"","\
                                       NM_SETTING_PPP_NOAUTH","\
                                       NM_SETTING_PPP_REFUSE_EAP","\
                                       NM_SETTING_PPP_REFUSE_PAP","\
                                       NM_SETTING_PPP_REFUSE_CHAP","\
                                       NM_SETTING_PPP_REFUSE_MSCHAP","\
                                       NM_SETTING_PPP_REFUSE_MSCHAPV2","\
                                       NM_SETTING_PPP_NOBSDCOMP","\
                                       NM_SETTING_PPP_NODEFLATE","\
                                       NM_SETTING_PPP_NO_VJ_COMP","\
                                       NM_SETTING_PPP_REQUIRE_MPPE","\
                                       NM_SETTING_PPP_REQUIRE_MPPE_128","\
                                       NM_SETTING_PPP_MPPE_STATEFUL","\
                                       NM_SETTING_PPP_CRTSCTS","\
                                       NM_SETTING_PPP_BAUD","\
                                       NM_SETTING_PPP_MRU","\
                                       NM_SETTING_PPP_MTU","\
                                       NM_SETTING_PPP_LCP_ECHO_FAILURE","\
                                       NM_SETTING_PPP_LCP_ECHO_INTERVAL
#define NMC_FIELDS_SETTING_PPP_COMMON  NMC_FIELDS_SETTING_PPP_ALL

/* Available fields for NM_SETTING_PPPOE_SETTING_NAME */
static NmcOutputField nmc_fields_setting_pppoe[] = {
	SETTING_FIELD ("name", 10),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_PPPOE_SERVICE, 12),                      /* 1 */
	SETTING_FIELD (NM_SETTING_PPPOE_USERNAME, 15),                     /* 2 */
	SETTING_FIELD (NM_SETTING_PPPOE_PASSWORD, 15),                     /* 3 */
	SETTING_FIELD (NM_SETTING_PPPOE_PASSWORD_FLAGS, 20),               /* 4 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_PPPOE_ALL     "name"","\
                                         NM_SETTING_PPPOE_SERVICE","\
                                         NM_SETTING_PPPOE_USERNAME","\
                                         NM_SETTING_PPPOE_PASSWORD","\
                                         NM_SETTING_PPPOE_PASSWORD_FLAGS
#define NMC_FIELDS_SETTING_PPPOE_COMMON  NMC_FIELDS_SETTING_PPPOE_ALL

/* Available fields for NM_SETTING_ADSL_SETTING_NAME */
static NmcOutputField nmc_fields_setting_adsl[] = {
	SETTING_FIELD ("name", 10),                                /* 0 */
	SETTING_FIELD (NM_SETTING_ADSL_USERNAME, 15),              /* 1 */
	SETTING_FIELD (NM_SETTING_ADSL_PASSWORD, 15),              /* 2 */
	SETTING_FIELD (NM_SETTING_ADSL_PASSWORD_FLAGS, 20),        /* 3 */
	SETTING_FIELD (NM_SETTING_ADSL_PROTOCOL, 10),              /* 4 */
	SETTING_FIELD (NM_SETTING_ADSL_ENCAPSULATION, 10),         /* 5 */
	SETTING_FIELD (NM_SETTING_ADSL_VPI, 10),                   /* 6 */
	SETTING_FIELD (NM_SETTING_ADSL_VCI, 10),                   /* 7 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_ADSL_ALL     "name"","\
                                        NM_SETTING_ADSL_USERNAME","\
                                        NM_SETTING_ADSL_PASSWORD","\
                                        NM_SETTING_ADSL_PASSWORD_FLAGS","\
                                        NM_SETTING_ADSL_PROTOCOL","\
                                        NM_SETTING_ADSL_ENCAPSULATION","\
                                        NM_SETTING_ADSL_VPI","\
                                        NM_SETTING_ADSL_VCI
#define NMC_FIELDS_SETTING_ADSL_COMMON  NMC_FIELDS_SETTING_ADSL_ALL

/* Available fields for NM_SETTING_GSM_SETTING_NAME */
static NmcOutputField nmc_fields_setting_gsm[] = {
	SETTING_FIELD ("name", 10),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_GSM_NUMBER, 10),                         /* 1 */
	SETTING_FIELD (NM_SETTING_GSM_USERNAME, 15),                       /* 2 */
	SETTING_FIELD (NM_SETTING_GSM_PASSWORD, 15),                       /* 3 */
	SETTING_FIELD (NM_SETTING_GSM_PASSWORD_FLAGS, 20),                 /* 4 */
	SETTING_FIELD (NM_SETTING_GSM_APN, 25),                            /* 5 */
	SETTING_FIELD (NM_SETTING_GSM_NETWORK_ID, 12),                     /* 6 */
	SETTING_FIELD (NM_SETTING_GSM_NETWORK_TYPE, 15),                   /* 7 */
	SETTING_FIELD (NM_SETTING_GSM_ALLOWED_BANDS, 15),                  /* 8 */
	SETTING_FIELD (NM_SETTING_GSM_PIN, 10),                            /* 9 */
	SETTING_FIELD (NM_SETTING_GSM_PIN_FLAGS, 20),                      /* 10 */
	SETTING_FIELD (NM_SETTING_GSM_HOME_ONLY, 10),                      /* 11 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_GSM_ALL     "name"","\
                                       NM_SETTING_GSM_NUMBER","\
                                       NM_SETTING_GSM_USERNAME","\
                                       NM_SETTING_GSM_PASSWORD","\
                                       NM_SETTING_GSM_PASSWORD_FLAGS","\
                                       NM_SETTING_GSM_APN","\
                                       NM_SETTING_GSM_NETWORK_ID","\
                                       NM_SETTING_GSM_NETWORK_TYPE","\
                                       NM_SETTING_GSM_ALLOWED_BANDS","\
                                       NM_SETTING_GSM_PIN","\
                                       NM_SETTING_GSM_PIN_FLAGS","\
                                       NM_SETTING_GSM_HOME_ONLY
#define NMC_FIELDS_SETTING_GSM_COMMON  NMC_FIELDS_SETTING_GSM_ALL

/* Available fields for NM_SETTING_CDMA_SETTING_NAME */
static NmcOutputField nmc_fields_setting_cdma[] = {
	SETTING_FIELD ("name", 10),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_CDMA_NUMBER, 15),                        /* 1 */
	SETTING_FIELD (NM_SETTING_CDMA_USERNAME, 15),                      /* 2 */
	SETTING_FIELD (NM_SETTING_CDMA_PASSWORD, 15),                      /* 3 */
	SETTING_FIELD (NM_SETTING_CDMA_PASSWORD_FLAGS, 20),                /* 4 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_CDMA_ALL     "name"","\
                                        NM_SETTING_CDMA_NUMBER","\
                                        NM_SETTING_CDMA_USERNAME","\
                                        NM_SETTING_CDMA_PASSWORD","\
                                        NM_SETTING_CDMA_PASSWORD_FLAGS
#define NMC_FIELDS_SETTING_CDMA_COMMON  NMC_FIELDS_SETTING_CDMA_ALL

/* Available fields for NM_SETTING_BLUETOOTH_SETTING_NAME */
static NmcOutputField nmc_fields_setting_bluetooth[] = {
	SETTING_FIELD ("name", 11),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_BLUETOOTH_BDADDR, 19),                   /* 1 */
	SETTING_FIELD (NM_SETTING_BLUETOOTH_TYPE, 10),                     /* 2 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_BLUETOOTH_ALL     "name"","\
                                             NM_SETTING_BLUETOOTH_BDADDR","\
                                             NM_SETTING_BLUETOOTH_TYPE
#define NMC_FIELDS_SETTING_BLUETOOTH_COMMON  NMC_FIELDS_SETTING_BLUETOOTH_ALL

/* Available fields for NM_SETTING_OLPC_MESH_SETTING_NAME */
static NmcOutputField nmc_fields_setting_olpc_mesh[] = {
	SETTING_FIELD ("name", 18),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_OLPC_MESH_SSID, 34),                     /* 1 */
	SETTING_FIELD (NM_SETTING_OLPC_MESH_CHANNEL, 12),                  /* 2 */
	SETTING_FIELD (NM_SETTING_OLPC_MESH_DHCP_ANYCAST_ADDRESS, 17),     /* 3 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_OLPC_MESH_ALL     "name"","\
                                             NM_SETTING_OLPC_MESH_SSID","\
                                             NM_SETTING_OLPC_MESH_CHANNEL","\
                                             NM_SETTING_OLPC_MESH_DHCP_ANYCAST_ADDRESS
#define NMC_FIELDS_SETTING_OLPC_MESH_COMMON  NMC_FIELDS_SETTING_OLPC_MESH_ALL

/* Available fields for NM_SETTING_VPN_SETTING_NAME */
static NmcOutputField nmc_fields_setting_vpn[] = {
	SETTING_FIELD ("name", 6),                                         /* 0 */
	SETTING_FIELD (NM_SETTING_VPN_SERVICE_TYPE, 40),                   /* 1 */
	SETTING_FIELD (NM_SETTING_VPN_USER_NAME, 12),                      /* 2 */
	SETTING_FIELD (NM_SETTING_VPN_DATA, 30),                           /* 3 */
	SETTING_FIELD (NM_SETTING_VPN_SECRETS, 15),                        /* 4 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_VPN_ALL     "name"","\
                                       NM_SETTING_VPN_SERVICE_TYPE","\
                                       NM_SETTING_VPN_USER_NAME","\
                                       NM_SETTING_VPN_DATA","\
                                       NM_SETTING_VPN_SECRETS
#define NMC_FIELDS_SETTING_VPN_COMMON  NMC_FIELDS_SETTING_VPN_ALL

/* Available fields for NM_SETTING_WIMAX_SETTING_NAME */
static NmcOutputField nmc_fields_setting_wimax[] = {
	SETTING_FIELD ("name", 6),                                         /* 0 */
	SETTING_FIELD (NM_SETTING_WIMAX_MAC_ADDRESS, 19),                  /* 1 */
	SETTING_FIELD (NM_SETTING_WIMAX_NETWORK_NAME, 40),                 /* 2 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_WIMAX_ALL     "name"","\
                                         NM_SETTING_WIMAX_MAC_ADDRESS","\
                                         NM_SETTING_WIMAX_NETWORK_NAME
#define NMC_FIELDS_SETTING_WIMAX_COMMON  NMC_FIELDS_SETTING_WIMAX_ALL

/* Available fields for NM_SETTING_INFINIBAND_SETTING_NAME */
static NmcOutputField nmc_fields_setting_infiniband[] = {
	SETTING_FIELD ("name",  12),                                       /* 0 */
	SETTING_FIELD (NM_SETTING_INFINIBAND_MAC_ADDRESS, 61),             /* 1 */
	SETTING_FIELD (NM_SETTING_INFINIBAND_MTU, 6),                      /* 2 */
	SETTING_FIELD (NM_SETTING_INFINIBAND_TRANSPORT_MODE, 12),          /* 3 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_INFINIBAND_ALL     "name"","\
                                              NM_SETTING_INFINIBAND_MAC_ADDRESS","\
                                              NM_SETTING_INFINIBAND_MTU","\
                                              NM_SETTING_INFINIBAND_TRANSPORT_MODE
#define NMC_FIELDS_SETTING_INFINIBAND_COMMON  NMC_FIELDS_SETTING_INFINIBAND_ALL

/* Available fields for NM_SETTING_BOND_SETTING_NAME */
static NmcOutputField nmc_fields_setting_bond[] = {
	SETTING_FIELD ("name",  8),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_BOND_INTERFACE_NAME, 15),                /* 1 */
	SETTING_FIELD (NM_SETTING_BOND_OPTIONS, 30),                       /* 2 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_BOND_ALL     "name"","\
                                        NM_SETTING_BOND_INTERFACE_NAME","\
                                        NM_SETTING_BOND_OPTIONS
#define NMC_FIELDS_SETTING_BOND_COMMON  NMC_FIELDS_SETTING_BOND_ALL

/* Available fields for NM_SETTING_VLAN_SETTING_NAME */
static NmcOutputField nmc_fields_setting_vlan[] = {
	SETTING_FIELD ("name",  6),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_VLAN_INTERFACE_NAME, 15),                /* 1 */
	SETTING_FIELD (NM_SETTING_VLAN_PARENT, 8),                         /* 2 */
	SETTING_FIELD (NM_SETTING_VLAN_ID, 6),                             /* 3 */
	SETTING_FIELD (NM_SETTING_VLAN_FLAGS, 45),                         /* 4 */
	SETTING_FIELD (NM_SETTING_VLAN_INGRESS_PRIORITY_MAP, 22),          /* 5 */
	SETTING_FIELD (NM_SETTING_VLAN_EGRESS_PRIORITY_MAP, 22),           /* 6 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_VLAN_ALL     "name"","\
                                        NM_SETTING_VLAN_INTERFACE_NAME","\
                                        NM_SETTING_VLAN_PARENT","\
                                        NM_SETTING_VLAN_ID","\
                                        NM_SETTING_VLAN_FLAGS","\
                                        NM_SETTING_VLAN_INGRESS_PRIORITY_MAP","\
                                        NM_SETTING_VLAN_EGRESS_PRIORITY_MAP
#define NMC_FIELDS_SETTING_VLAN_COMMON  NMC_FIELDS_SETTING_VLAN_ALL

/* Available fields for NM_SETTING_BRIDGE_SETTING_NAME */
static NmcOutputField nmc_fields_setting_bridge[] = {
	SETTING_FIELD ("name",  8),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_BRIDGE_INTERFACE_NAME, 15),              /* 1 */
	SETTING_FIELD (NM_SETTING_BRIDGE_STP, 5),                          /* 2 */
	SETTING_FIELD (NM_SETTING_BRIDGE_PRIORITY, 6),                     /* 3 */
	SETTING_FIELD (NM_SETTING_BRIDGE_FORWARD_DELAY, 6),                /* 4 */
	SETTING_FIELD (NM_SETTING_BRIDGE_HELLO_TIME, 6),                   /* 5 */
	SETTING_FIELD (NM_SETTING_BRIDGE_MAX_AGE, 6),                      /* 6 */
	SETTING_FIELD (NM_SETTING_BRIDGE_AGEING_TIME, 6),                  /* 7 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_BRIDGE_ALL    "name"","\
                                         NM_SETTING_BRIDGE_INTERFACE_NAME","\
                                         NM_SETTING_BRIDGE_STP","\
                                         NM_SETTING_BRIDGE_PRIORITY","\
                                         NM_SETTING_BRIDGE_FORWARD_DELAY","\
                                         NM_SETTING_BRIDGE_HELLO_TIME","\
                                         NM_SETTING_BRIDGE_MAX_AGE","\
                                         NM_SETTING_BRIDGE_AGEING_TIME
#define NMC_FIELDS_SETTING_BRIDGE_COMMON NMC_FIELDS_SETTING_BRIDGE_ALL

/* Available fields for NM_SETTING_BRIDGE_PORT_SETTING_NAME */
static NmcOutputField nmc_fields_setting_bridge_port[] = {
	SETTING_FIELD ("name",  8),                                        /* 0 */
	SETTING_FIELD (NM_SETTING_BRIDGE_PORT_PRIORITY, 10),               /* 1 */
	SETTING_FIELD (NM_SETTING_BRIDGE_PORT_PATH_COST, 12),              /* 2 */
	SETTING_FIELD (NM_SETTING_BRIDGE_PORT_HAIRPIN_MODE, 15),           /* 3 */
	{NULL, NULL, 0, NULL, 0}
};
#define NMC_FIELDS_SETTING_BRIDGE_PORT_ALL    "name"","\
                                              NM_SETTING_BRIDGE_PORT_PRIORITY","\
                                              NM_SETTING_BRIDGE_PORT_PATH_COST","\
                                              NM_SETTING_BRIDGE_PORT_HAIRPIN_MODE
#define NMC_FIELDS_SETTING_BRIDGE_PORT_COMMON NMC_FIELDS_SETTING_BRIDGE_PORT_ALL

/*----------------------------------------------------------------------------*/

static char *
wep_key_type_to_string (NMWepKeyType type)
{
	switch (type) {
	case NM_WEP_KEY_TYPE_KEY:
		return g_strdup_printf (_("%d (hex-ascii-key)"), type);
	case NM_WEP_KEY_TYPE_PASSPHRASE:
		return g_strdup_printf (_("%d (104/128-bit passphrase)"), type);
	case NM_WEP_KEY_TYPE_UNKNOWN:
	default:
		return g_strdup_printf (_("%d (unknown)"), type);
	}
}

static char *
byte_array_to_string (const GByteArray *array)
{
	GString *cert = NULL;
	int i;

	if (array && array->len > 0)
		cert = g_string_new (NULL);

	for (i = 0; array && i < array->len; i++) {
		g_string_append_printf (cert, "%02X", array->data[i]);
	}

	return cert ? g_string_free (cert, FALSE) : NULL;
}

static char *
allowed_bands_to_string (guint32 bands)
{
	GString *band_str;

	if (bands == NM_SETTING_GSM_BAND_UNKNOWN)
		return g_strdup (_("0 (unknown)"));

	band_str = g_string_new (NULL);
	g_string_printf (band_str, "%d (", bands);

	if (bands & NM_SETTING_GSM_BAND_ANY)
		g_string_append (band_str, _("any, "));
	if (bands & NM_SETTING_GSM_BAND_EGSM)
		g_string_append (band_str, _("900 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_DCS)
		g_string_append (band_str, _("1800 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_PCS)
		g_string_append (band_str, _("1900 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_G850)
		g_string_append (band_str, _("850 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U2100)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 2100 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U1800)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 1800 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U17IV)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 1700/2100 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U800)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 800 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U850)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 850 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U900)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 900 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U17IX)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 1700 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U1900)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 1900 MHz, "));
	if (bands & NM_SETTING_GSM_BAND_U2600)
		g_string_append (band_str, _("WCDMA 3GPP UMTS 2600 MHz, "));

	if (band_str->str[band_str->len-1] == '(')
		g_string_assign (band_str, _("unknown"));
	else
		g_string_truncate (band_str, band_str->len-2);  /* chop off trailing ', ' */

	g_string_append_c (band_str, ')');

	return g_string_free (band_str, FALSE);
}

static char *
vlan_flags_to_string (guint32 flags)
{
	GString *flag_str;

	if (flags == 0)
		return g_strdup (_("0 (NONE)"));

	flag_str = g_string_new (NULL);
	g_string_printf (flag_str, "%d (", flags);

	if (flags & NM_VLAN_FLAG_REORDER_HEADERS)
		g_string_append (flag_str, _("REORDER_HEADERS, "));
	if (flags & NM_VLAN_FLAG_GVRP)
		g_string_append (flag_str, _("GVRP, "));
	if (flags & NM_VLAN_FLAG_LOOSE_BINDING)
		g_string_append (flag_str, _("LOOSE_BINDING, "));

	if (flag_str->str[flag_str->len-1] == '(')
		g_string_assign (flag_str, _("unknown"));
	else
		g_string_truncate (flag_str, flag_str->len-2);  /* chop off trailing ', ' */

	g_string_append_c (flag_str, ')');

	return g_string_free (flag_str, FALSE);
}

static char *
vlan_priorities_to_string (NMSettingVlan *s_vlan, NMVlanPriorityMap map)
{
	GString *priorities;
	int i;

	priorities = g_string_new (NULL);
	for (i = 0; i < nm_setting_vlan_get_num_priorities (s_vlan, map); i++) {
		guint32 from, to;

		if (nm_setting_vlan_get_priority (s_vlan, map, i, &from, &to))
			g_string_append_printf (priorities, "%d:%d,", from, to);
	}
	if (priorities->len)
		g_string_truncate (priorities, priorities->len-1);  /* chop off trailing ',' */

	return g_string_free (priorities, FALSE);
}

static char *
ip6_privacy_to_string (NMSettingIP6ConfigPrivacy ip6_privacy)
{
	switch (ip6_privacy) {
	case NM_SETTING_IP6_CONFIG_PRIVACY_DISABLED:
		return g_strdup_printf (_("%d (disabled)"), ip6_privacy);
	case NM_SETTING_IP6_CONFIG_PRIVACY_PREFER_PUBLIC_ADDR:
		return g_strdup_printf (_("%d (enabled, prefer public IP)"), ip6_privacy);
	case NM_SETTING_IP6_CONFIG_PRIVACY_PREFER_TEMP_ADDR:
		return g_strdup_printf (_("%d (enabled, prefer temporary IP)"), ip6_privacy);
	default:
		return g_strdup_printf (_("%d (unknown)"), ip6_privacy);
	}
}

static char *
secret_flags_to_string (guint32 flags)
{
	GString *flag_str;

	if (flags == 0)
		return g_strdup (_("0 (none)"));

	flag_str = g_string_new (NULL);
	g_string_printf (flag_str, "%d (", flags);

	if (flags & NM_SETTING_SECRET_FLAG_AGENT_OWNED)
		g_string_append (flag_str, _("agent-owned, "));
	if (flags & NM_SETTING_SECRET_FLAG_NOT_SAVED)
		g_string_append (flag_str, _("not saved, "));
	if (flags & NM_SETTING_SECRET_FLAG_NOT_REQUIRED)
		g_string_append (flag_str, _("not required, "));

	if (flag_str->str[flag_str->len-1] == '(')
		g_string_assign (flag_str, _("unknown"));
	else
		g_string_truncate (flag_str, flag_str->len-2);  /* chop off trailing ', ' */

	g_string_append_c (flag_str, ')');

	return g_string_free (flag_str, FALSE);
}

static void
vpn_data_item (const char *key, const char *value, gpointer user_data)
{
	GString *ret_str = (GString *) user_data;

	if (ret_str->len != 0)
		g_string_append (ret_str, ", ");

	g_string_append_printf (ret_str, "%s = %s", key, value);
}


/* === property get functions === */
#define DEFINE_GETTER(func_name, property_name) \
	static char * \
	func_name (NMSetting *setting) \
	{ \
		GValue val = G_VALUE_INIT; \
		g_value_init (&val, G_TYPE_STRING); \
		g_object_get_property (G_OBJECT (setting), property_name, &val); \
		/* Getters return allocated values, and returning the string \
		 * the GValue copied from the object without unsetting the \
		 * GValue fulfills that requirement. */ \
		return (char *) g_value_get_string (&val); \
	}

#define DEFINE_SECRET_FLAGS_GETTER(func_name, property_name) \
	static char * \
	func_name (NMSetting *setting) \
	{ \
		GValue val = G_VALUE_INIT; \
		g_value_init (&val, G_TYPE_UINT); \
		g_object_get_property (G_OBJECT (setting), property_name, &val); \
		return secret_flags_to_string (g_value_get_uint (&val)); \
	}

#define DEFINE_HWADDR_GETTER(func_name, property_name) \
	static char * \
	func_name (NMSetting *setting) \
	{ \
		GValue val = G_VALUE_INIT; \
		GArray *array; \
		char *hwaddr = NULL; \
		g_value_init (&val, DBUS_TYPE_G_UCHAR_ARRAY); \
		g_object_get_property (G_OBJECT (setting), property_name, &val); \
		array = g_value_get_boxed (&val); \
		if (array) \
			hwaddr = nm_utils_hwaddr_ntoa (array->data, nm_utils_hwaddr_type (array->len)); \
		g_value_unset (&val); \
		return hwaddr; \
	}

/* --- NM_SETTING_802_1X_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_802_1X_get_eap, NM_SETTING_802_1X_EAP)
DEFINE_GETTER (nmc_property_802_1X_get_identity, NM_SETTING_802_1X_IDENTITY)
DEFINE_GETTER (nmc_property_802_1X_get_anonymous_identity, NM_SETTING_802_1X_ANONYMOUS_IDENTITY)
DEFINE_GETTER (nmc_property_802_1X_get_pac_file, NM_SETTING_802_1X_PAC_FILE)
DEFINE_GETTER (nmc_property_802_1X_get_ca_path, NM_SETTING_802_1X_CA_PATH)
DEFINE_GETTER (nmc_property_802_1X_get_subject_match, NM_SETTING_802_1X_SUBJECT_MATCH)
DEFINE_GETTER (nmc_property_802_1X_get_altsubject_matches, NM_SETTING_802_1X_ALTSUBJECT_MATCHES)
DEFINE_GETTER (nmc_property_802_1X_get_phase1_peapver, NM_SETTING_802_1X_PHASE1_PEAPVER)
DEFINE_GETTER (nmc_property_802_1X_get_phase1_peaplabel, NM_SETTING_802_1X_PHASE1_PEAPLABEL)
DEFINE_GETTER (nmc_property_802_1X_get_phase1_fast_provisioning, NM_SETTING_802_1X_PHASE1_FAST_PROVISIONING)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_auth, NM_SETTING_802_1X_PHASE2_AUTH)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_autheap, NM_SETTING_802_1X_PHASE2_AUTHEAP)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_ca_path, NM_SETTING_802_1X_PHASE2_CA_PATH)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_subject_match, NM_SETTING_802_1X_PHASE2_SUBJECT_MATCH)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_altsubject_matches, NM_SETTING_802_1X_PHASE2_ALTSUBJECT_MATCHES)
DEFINE_GETTER (nmc_property_802_1X_get_password, NM_SETTING_802_1X_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_802_1X_get_password_flags, NM_SETTING_802_1X_PASSWORD_FLAGS)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_802_1X_get_password_raw_flags, NM_SETTING_802_1X_PASSWORD_RAW_FLAGS)
DEFINE_GETTER (nmc_property_802_1X_get_private_key_password, NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_802_1X_get_private_key_password_flags, NM_SETTING_802_1X_PRIVATE_KEY_PASSWORD_FLAGS)
DEFINE_GETTER (nmc_property_802_1X_get_phase2_private_key_password, NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_802_1X_get_phase2_private_key_password_flags, NM_SETTING_802_1X_PHASE2_PRIVATE_KEY_PASSWORD_FLAGS)
DEFINE_GETTER (nmc_property_802_1X_get_pin, NM_SETTING_802_1X_PIN)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_802_1X_get_pin_flags, NM_SETTING_802_1X_PIN_FLAGS)
DEFINE_GETTER (nmc_property_802_1X_get_system_ca_certs, NM_SETTING_802_1X_SYSTEM_CA_CERTS)

static char *
nmc_property_802_1X_get_ca_cert (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *ca_cert_str = NULL;

	scheme = nm_setting_802_1x_get_ca_cert_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		ca_cert_str = byte_array_to_string (nm_setting_802_1x_get_ca_cert_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		ca_cert_str = g_strdup (nm_setting_802_1x_get_ca_cert_path (s_8021X));

	return ca_cert_str;
}

static char *
nmc_property_802_1X_get_client_cert (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *client_cert_str = NULL;

	scheme = nm_setting_802_1x_get_client_cert_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		client_cert_str = byte_array_to_string (nm_setting_802_1x_get_client_cert_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		client_cert_str = g_strdup (nm_setting_802_1x_get_client_cert_path (s_8021X));

	return client_cert_str;
}

static char *
nmc_property_802_1X_get_phase2_ca_cert (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *phase2_ca_cert_str = NULL;

	scheme = nm_setting_802_1x_get_phase2_ca_cert_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		phase2_ca_cert_str = byte_array_to_string (nm_setting_802_1x_get_phase2_ca_cert_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		phase2_ca_cert_str = g_strdup (nm_setting_802_1x_get_phase2_ca_cert_path (s_8021X));

	return phase2_ca_cert_str;
}

static char *
nmc_property_802_1X_get_phase2_client_cert (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *phase2_client_cert_str = NULL;

	scheme = nm_setting_802_1x_get_phase2_client_cert_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		phase2_client_cert_str = byte_array_to_string (nm_setting_802_1x_get_phase2_client_cert_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		phase2_client_cert_str = g_strdup (nm_setting_802_1x_get_phase2_client_cert_path (s_8021X));

	return phase2_client_cert_str;
}

static char *
nmc_property_802_1X_get_password_raw (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	return byte_array_to_string (nm_setting_802_1x_get_password_raw (s_8021X));
}

static char *
nmc_property_802_1X_get_private_key (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *private_key_str = NULL;

	scheme = nm_setting_802_1x_get_private_key_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		private_key_str = byte_array_to_string (nm_setting_802_1x_get_private_key_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		private_key_str = g_strdup (nm_setting_802_1x_get_private_key_path (s_8021X));

	return private_key_str;
}

static char *
nmc_property_802_1X_get_phase2_private_key (NMSetting *setting)
{
	NMSetting8021x *s_8021X = NM_SETTING_802_1X (setting);
	NMSetting8021xCKScheme scheme;
	char *phase2_private_key_str = NULL;

	scheme = nm_setting_802_1x_get_phase2_private_key_scheme (s_8021X);
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_BLOB)
		phase2_private_key_str = byte_array_to_string (nm_setting_802_1x_get_phase2_private_key_blob (s_8021X));
	if (scheme == NM_SETTING_802_1X_CK_SCHEME_PATH)
		phase2_private_key_str = g_strdup (nm_setting_802_1x_get_phase2_private_key_path (s_8021X));

	return phase2_private_key_str;
}

/* --- NM_SETTING_ADSL_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_adsl_get_username, NM_SETTING_ADSL_USERNAME)
DEFINE_GETTER (nmc_property_adsl_get_password, NM_SETTING_ADSL_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_adsl_get_password_flags, NM_SETTING_ADSL_PASSWORD_FLAGS)
DEFINE_GETTER (nmc_property_adsl_get_protocol, NM_SETTING_ADSL_PROTOCOL)
DEFINE_GETTER (nmc_property_adsl_get_encapsulation, NM_SETTING_ADSL_ENCAPSULATION)
DEFINE_GETTER (nmc_property_adsl_get_vpi, NM_SETTING_ADSL_VPI)
DEFINE_GETTER (nmc_property_adsl_get_vci, NM_SETTING_ADSL_VCI)

/* --- NM_SETTING_BLUETOOTH_SETTING_NAME property get functions --- */
DEFINE_HWADDR_GETTER (nmc_property_bluetooth_get_bdaddr, NM_SETTING_BLUETOOTH_BDADDR)
DEFINE_GETTER (nmc_property_bluetooth_get_type, NM_SETTING_BLUETOOTH_TYPE)

/* --- NM_SETTING_BOND_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_bond_get_interface_name, NM_SETTING_BOND_INTERFACE_NAME)

static char *
nmc_property_bond_get_options (NMSetting *setting)
{
	NMSettingBond *s_bond = NM_SETTING_BOND (setting);
	GString *bond_options_s;
	int i;

	bond_options_s = g_string_new (NULL);
	for (i = 0; i < nm_setting_bond_get_num_options (s_bond); i++) {
		const char *key, *value;

		nm_setting_bond_get_option (s_bond, i, &key, &value);
		g_string_append_printf (bond_options_s, "%s=%s,", key, value);
	}
	g_string_truncate (bond_options_s, bond_options_s->len-1);  /* chop off trailing ',' */

	return g_string_free (bond_options_s, FALSE);
}

/* --- NM_SETTING_BRIDGE_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_bridge_get_interface_name, NM_SETTING_BRIDGE_INTERFACE_NAME)
DEFINE_GETTER (nmc_property_bridge_get_stp, NM_SETTING_BRIDGE_STP)
DEFINE_GETTER (nmc_property_bridge_get_priority, NM_SETTING_BRIDGE_PRIORITY)
DEFINE_GETTER (nmc_property_bridge_get_forward_delay, NM_SETTING_BRIDGE_FORWARD_DELAY)
DEFINE_GETTER (nmc_property_bridge_get_hello_time, NM_SETTING_BRIDGE_HELLO_TIME)
DEFINE_GETTER (nmc_property_bridge_get_max_age, NM_SETTING_BRIDGE_MAX_AGE)
DEFINE_GETTER (nmc_property_bridge_get_ageing_time, NM_SETTING_BRIDGE_AGEING_TIME)

/* --- NM_SETTING_BRIDGE_PORT_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_bridge_port_get_priority, NM_SETTING_BRIDGE_PORT_PRIORITY)
DEFINE_GETTER (nmc_property_bridge_port_get_path_cost, NM_SETTING_BRIDGE_PORT_PATH_COST)
DEFINE_GETTER (nmc_property_bridge_port_get_hairpin_mode, NM_SETTING_BRIDGE_PORT_HAIRPIN_MODE)

/* --- NM_SETTING_CDMA_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_cdma_get_number, NM_SETTING_CDMA_NUMBER)
DEFINE_GETTER (nmc_property_cdma_get_username, NM_SETTING_CDMA_USERNAME)
DEFINE_GETTER (nmc_property_cdma_get_password, NM_SETTING_CDMA_PASSWORD)

DEFINE_SECRET_FLAGS_GETTER (nmc_property_cdma_get_password_flags, NM_SETTING_CDMA_PASSWORD_FLAGS)

/* --- NM_SETTING_CONNECTION_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_connection_get_id, NM_SETTING_CONNECTION_ID)
DEFINE_GETTER (nmc_property_connection_get_uuid, NM_SETTING_CONNECTION_UUID)
DEFINE_GETTER (nmc_property_connection_get_interface_name, NM_SETTING_CONNECTION_INTERFACE_NAME)
DEFINE_GETTER (nmc_property_connection_get_type, NM_SETTING_CONNECTION_TYPE)
DEFINE_GETTER (nmc_property_connection_get_autoconnect, NM_SETTING_CONNECTION_AUTOCONNECT)
DEFINE_GETTER (nmc_property_connection_get_timestamp, NM_SETTING_CONNECTION_TIMESTAMP)
DEFINE_GETTER (nmc_property_connection_get_read_only, NM_SETTING_CONNECTION_READ_ONLY)

static char *
nmc_property_connection_get_permissions (NMSetting *setting)
{
	NMSettingConnection *s_con = NM_SETTING_CONNECTION (setting);
	GString *perm = NULL;
	const char *perm_item;
	const char *perm_type;
	int i;

	perm = g_string_new (NULL);
	for (i = 0; i < nm_setting_connection_get_num_permissions (s_con); i++) {
		nm_setting_connection_get_permission (s_con, i, &perm_type, &perm_item, NULL);
		g_string_append_printf (perm, "%s:%s,", perm_type, perm_item);
	}
	if (perm->len > 0)
		g_string_truncate (perm, perm->len-1); /* remove trailing , */

	return g_string_free (perm, FALSE);
}

DEFINE_GETTER (nmc_property_connection_get_zone, NM_SETTING_CONNECTION_ZONE)
DEFINE_GETTER (nmc_property_connection_get_master, NM_SETTING_CONNECTION_MASTER)
DEFINE_GETTER (nmc_property_connection_get_slave_type, NM_SETTING_CONNECTION_SLAVE_TYPE)
DEFINE_GETTER (nmc_property_connection_get_secondaries, NM_SETTING_CONNECTION_SECONDARIES)

/* --- NM_SETTING_GSM_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_gsm_get_number, NM_SETTING_GSM_NUMBER)
DEFINE_GETTER (nmc_property_gsm_get_username, NM_SETTING_GSM_USERNAME)
DEFINE_GETTER (nmc_property_gsm_get_password, NM_SETTING_GSM_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_gsm_get_password_flags, NM_SETTING_GSM_PASSWORD_FLAGS)
DEFINE_GETTER (nmc_property_gsm_get_apn, NM_SETTING_GSM_APN)
DEFINE_GETTER (nmc_property_gsm_get_network_id, NM_SETTING_GSM_NETWORK_ID)
DEFINE_GETTER (nmc_property_gsm_get_network_type, NM_SETTING_GSM_NETWORK_TYPE)

static char *
nmc_property_gsm_get_allowed_bands (NMSetting *setting)
{
	NMSettingGsm *s_gsm = NM_SETTING_GSM (setting);
	return allowed_bands_to_string (nm_setting_gsm_get_allowed_bands (s_gsm));
}

DEFINE_GETTER (nmc_property_gsm_get_pin, NM_SETTING_GSM_PIN)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_gsm_get_pin_flags, NM_SETTING_GSM_PIN_FLAGS)
DEFINE_GETTER (nmc_property_gsm_get_home_only, NM_SETTING_GSM_HOME_ONLY)

/* --- NM_SETTING_INFINIBAND_SETTING_NAME property get functions --- */
DEFINE_HWADDR_GETTER (nmc_property_ib_get_mac_address, NM_SETTING_INFINIBAND_MAC_ADDRESS)
DEFINE_GETTER (nmc_property_ib_get_transport_mode, NM_SETTING_INFINIBAND_TRANSPORT_MODE)

static char *
nmc_property_ib_get_mtu (NMSetting *setting)
{
	NMSettingInfiniband *s_infiniband = NM_SETTING_INFINIBAND (setting);
	int mtu;

	mtu = nm_setting_infiniband_get_mtu (s_infiniband);
	if (mtu == 0)
		return g_strdup (_("auto"));
	else
		return g_strdup_printf ("%d", nm_setting_infiniband_get_mtu (s_infiniband));
}

/* --- NM_SETTING_IP4_CONFIG_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_ipv4_get_method, NM_SETTING_IP4_CONFIG_METHOD)
DEFINE_GETTER (nmc_property_ipv4_get_dns, NM_SETTING_IP4_CONFIG_DNS)
DEFINE_GETTER (nmc_property_ipv4_get_dns_search, NM_SETTING_IP4_CONFIG_DNS_SEARCH)
DEFINE_GETTER (nmc_property_ipv4_get_addresses, NM_SETTING_IP4_CONFIG_ADDRESSES)
DEFINE_GETTER (nmc_property_ipv4_get_routes, NM_SETTING_IP4_CONFIG_ROUTES)
DEFINE_GETTER (nmc_property_ipv4_get_ignore_auto_routes, NM_SETTING_IP4_CONFIG_IGNORE_AUTO_ROUTES)
DEFINE_GETTER (nmc_property_ipv4_get_ignore_auto_dns, NM_SETTING_IP4_CONFIG_IGNORE_AUTO_DNS)
DEFINE_GETTER (nmc_property_ipv4_get_dhcp_client_id, NM_SETTING_IP4_CONFIG_DHCP_CLIENT_ID)
DEFINE_GETTER (nmc_property_ipv4_get_dhcp_send_hostname, NM_SETTING_IP4_CONFIG_DHCP_SEND_HOSTNAME)
DEFINE_GETTER (nmc_property_ipv4_get_dhcp_hostname, NM_SETTING_IP4_CONFIG_DHCP_HOSTNAME)
DEFINE_GETTER (nmc_property_ipv4_get_never_default, NM_SETTING_IP4_CONFIG_NEVER_DEFAULT)
DEFINE_GETTER (nmc_property_ipv4_get_may_fail, NM_SETTING_IP4_CONFIG_MAY_FAIL)

/* --- NM_SETTING_IP6_CONFIG_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_ipv6_get_method, NM_SETTING_IP6_CONFIG_METHOD)
DEFINE_GETTER (nmc_property_ipv6_get_dns, NM_SETTING_IP6_CONFIG_DNS)
DEFINE_GETTER (nmc_property_ipv6_get_dns_search, NM_SETTING_IP6_CONFIG_DNS_SEARCH)
DEFINE_GETTER (nmc_property_ipv6_get_addresses, NM_SETTING_IP6_CONFIG_ADDRESSES)
DEFINE_GETTER (nmc_property_ipv6_get_routes, NM_SETTING_IP6_CONFIG_ROUTES)
DEFINE_GETTER (nmc_property_ipv6_get_ignore_auto_routes, NM_SETTING_IP6_CONFIG_IGNORE_AUTO_ROUTES)
DEFINE_GETTER (nmc_property_ipv6_get_ignore_auto_dns, NM_SETTING_IP6_CONFIG_IGNORE_AUTO_DNS)
DEFINE_GETTER (nmc_property_ipv6_get_never_default, NM_SETTING_IP6_CONFIG_NEVER_DEFAULT)
DEFINE_GETTER (nmc_property_ipv6_get_may_fail, NM_SETTING_IP6_CONFIG_MAY_FAIL)
DEFINE_GETTER (nmc_property_ipv6_get_dhcp_hostname, NM_SETTING_IP6_CONFIG_DHCP_HOSTNAME)

static char *
nmc_property_ipv6_get_ip6_privacy (NMSetting *setting)
{
	NMSettingIP6Config *s_ip6 = NM_SETTING_IP6_CONFIG (setting);
	return ip6_privacy_to_string (nm_setting_ip6_config_get_ip6_privacy (s_ip6));
}

/* --- NM_SETTING_OLPC_MESH_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_olpc_get_channel, NM_SETTING_OLPC_MESH_CHANNEL)
DEFINE_HWADDR_GETTER (nmc_property_olpc_get_anycast_address, NM_SETTING_OLPC_MESH_DHCP_ANYCAST_ADDRESS)

static char *
nmc_property_olpc_get_ssid (NMSetting *setting)
{
	NMSettingOlpcMesh *s_olpc_mesh = NM_SETTING_OLPC_MESH (setting);
	const GByteArray *ssid;
	char *ssid_str = NULL;

	ssid = nm_setting_olpc_mesh_get_ssid (s_olpc_mesh);
	if (ssid)
		ssid_str = ssid_to_printable ((const char *) ssid->data, ssid->len);

	return ssid_str;
}

/* --- NM_SETTING_PPP_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_ppp_get_noauth, NM_SETTING_PPP_NOAUTH)
DEFINE_GETTER (nmc_property_ppp_get_refuse_eap, NM_SETTING_PPP_REFUSE_EAP)
DEFINE_GETTER (nmc_property_ppp_get_refuse_pap, NM_SETTING_PPP_REFUSE_PAP)
DEFINE_GETTER (nmc_property_ppp_get_refuse_chap, NM_SETTING_PPP_REFUSE_CHAP)
DEFINE_GETTER (nmc_property_ppp_get_refuse_mschap, NM_SETTING_PPP_REFUSE_MSCHAP)
DEFINE_GETTER (nmc_property_ppp_get_refuse_mschapv2, NM_SETTING_PPP_REFUSE_MSCHAPV2)
DEFINE_GETTER (nmc_property_ppp_get_nobsdcomp, NM_SETTING_PPP_NOBSDCOMP)
DEFINE_GETTER (nmc_property_ppp_get_nodeflate, NM_SETTING_PPP_NODEFLATE)
DEFINE_GETTER (nmc_property_ppp_get_no_vj_comp, NM_SETTING_PPP_NO_VJ_COMP)
DEFINE_GETTER (nmc_property_ppp_get_require_mppe, NM_SETTING_PPP_REQUIRE_MPPE)
DEFINE_GETTER (nmc_property_ppp_get_require_mppe_128, NM_SETTING_PPP_REQUIRE_MPPE_128)
DEFINE_GETTER (nmc_property_ppp_get_mppe_stateful, NM_SETTING_PPP_MPPE_STATEFUL)
DEFINE_GETTER (nmc_property_ppp_get_crtscts, NM_SETTING_PPP_CRTSCTS)
DEFINE_GETTER (nmc_property_ppp_get_baud, NM_SETTING_PPP_BAUD)
DEFINE_GETTER (nmc_property_ppp_get_mru, NM_SETTING_PPP_MRU)
DEFINE_GETTER (nmc_property_ppp_get_mtu, NM_SETTING_PPP_MTU)
DEFINE_GETTER (nmc_property_ppp_get_lcp_echo_failure, NM_SETTING_PPP_LCP_ECHO_FAILURE)
DEFINE_GETTER (nmc_property_ppp_get_lcp_echo_interval, NM_SETTING_PPP_LCP_ECHO_INTERVAL)

/* --- NM_SETTING_PPPOE_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_pppoe_get_service, NM_SETTING_PPPOE_SERVICE)
DEFINE_GETTER (nmc_property_pppoe_get_username, NM_SETTING_PPPOE_USERNAME)
DEFINE_GETTER (nmc_property_pppoe_get_password, NM_SETTING_PPPOE_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_pppoe_get_password_flags, NM_SETTING_PPPOE_PASSWORD_FLAGS)

/* --- NM_SETTING_SERIAL_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_serial_get_baud, NM_SETTING_SERIAL_BAUD)
DEFINE_GETTER (nmc_property_serial_get_bits, NM_SETTING_SERIAL_BITS)
DEFINE_GETTER (nmc_property_serial_get_parity, NM_SETTING_SERIAL_PARITY)
DEFINE_GETTER (nmc_property_serial_get_stopbits, NM_SETTING_SERIAL_STOPBITS)
DEFINE_GETTER (nmc_property_serial_get_send_delay, NM_SETTING_SERIAL_SEND_DELAY)

/* --- NM_SETTING_VLAN_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_vlan_get_interface_name, NM_SETTING_VLAN_INTERFACE_NAME)
DEFINE_GETTER (nmc_property_vlan_get_parent, NM_SETTING_VLAN_PARENT)
DEFINE_GETTER (nmc_property_vlan_get_id, NM_SETTING_VLAN_ID)


static char *
nmc_property_vlan_get_flags (NMSetting *setting)
{
	NMSettingVlan *s_vlan = NM_SETTING_VLAN (setting);
	return vlan_flags_to_string (nm_setting_vlan_get_flags (s_vlan));
}

static char *
nmc_property_vlan_get_ingress_priority_map (NMSetting *setting)
{
	NMSettingVlan *s_vlan = NM_SETTING_VLAN (setting);
	return vlan_priorities_to_string (s_vlan, NM_VLAN_INGRESS_MAP);
}

static char *
nmc_property_vlan_get_egress_priority_map (NMSetting *setting)
{
	NMSettingVlan *s_vlan = NM_SETTING_VLAN (setting);
	return vlan_priorities_to_string (s_vlan, NM_VLAN_EGRESS_MAP);
}

/* --- NM_SETTING_VPN_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_vpn_get_service_type, NM_SETTING_VPN_SERVICE_TYPE)
DEFINE_GETTER (nmc_property_vpn_get_user_name, NM_SETTING_VPN_USER_NAME)

static char *
nmc_property_vpn_get_data (NMSetting *setting)
{
	NMSettingVPN *s_vpn = NM_SETTING_VPN (setting);
	GString *data_item_str;

	data_item_str = g_string_new (NULL);
	nm_setting_vpn_foreach_data_item (s_vpn, &vpn_data_item, data_item_str);

	return g_string_free (data_item_str, FALSE);
}

static char *
nmc_property_vpn_get_secrets (NMSetting *setting)
{
	NMSettingVPN *s_vpn = NM_SETTING_VPN (setting);
	GString *secret_str;

	secret_str = g_string_new (NULL);
	nm_setting_vpn_foreach_secret (s_vpn, &vpn_data_item, secret_str);

	return g_string_free (secret_str, FALSE);
}

/* --- NM_SETTING_WIMAX_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_wimax_get_network_name, NM_SETTING_WIMAX_NETWORK_NAME)
DEFINE_HWADDR_GETTER (nmc_property_wimax_get_mac_address, NM_SETTING_WIMAX_MAC_ADDRESS)

/* --- NM_SETTING_WIRED_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_wired_get_port, NM_SETTING_WIRED_PORT)
DEFINE_GETTER (nmc_property_wired_get_speed, NM_SETTING_WIRED_SPEED)
DEFINE_GETTER (nmc_property_wired_get_duplex, NM_SETTING_WIRED_DUPLEX)
DEFINE_GETTER (nmc_property_wired_get_auto_negotiate, NM_SETTING_WIRED_AUTO_NEGOTIATE)
DEFINE_HWADDR_GETTER (nmc_property_wired_get_mac_address, NM_SETTING_WIRED_MAC_ADDRESS)
DEFINE_HWADDR_GETTER (nmc_property_wired_get_cloned_mac_address, NM_SETTING_WIRED_CLONED_MAC_ADDRESS)
DEFINE_GETTER (nmc_property_wired_get_mac_address_blacklist, NM_SETTING_WIRED_MAC_ADDRESS_BLACKLIST)
DEFINE_GETTER (nmc_property_wired_get_s390_subchannels, NM_SETTING_WIRED_S390_SUBCHANNELS)
DEFINE_GETTER (nmc_property_wired_get_s390_nettype, NM_SETTING_WIRED_S390_NETTYPE)
DEFINE_GETTER (nmc_property_wired_get_s390_options, NM_SETTING_WIRED_S390_OPTIONS)

static char *
nmc_property_wired_get_mtu (NMSetting *setting)
{
	NMSettingWired *s_wired = NM_SETTING_WIRED (setting);
	int mtu;

	mtu = nm_setting_wired_get_mtu (s_wired);
	if (mtu == 0)
		return g_strdup (_("auto"));
	else
		return g_strdup_printf ("%d", nm_setting_wired_get_mtu (s_wired));
}

/* --- NM_SETTING_WIRELESS_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_wireless_get_mode, NM_SETTING_WIRELESS_MODE)
DEFINE_GETTER (nmc_property_wireless_get_band, NM_SETTING_WIRELESS_BAND)
DEFINE_GETTER (nmc_property_wireless_get_channel, NM_SETTING_WIRELESS_CHANNEL)
DEFINE_HWADDR_GETTER (nmc_property_wireless_get_bssid, NM_SETTING_WIRELESS_BSSID)
DEFINE_GETTER (nmc_property_wireless_get_rate, NM_SETTING_WIRELESS_RATE)
DEFINE_GETTER (nmc_property_wireless_get_tx_power, NM_SETTING_WIRELESS_TX_POWER)
DEFINE_HWADDR_GETTER (nmc_property_wireless_get_mac_address, NM_SETTING_WIRELESS_MAC_ADDRESS)
DEFINE_HWADDR_GETTER (nmc_property_wireless_get_cloned_mac_address, NM_SETTING_WIRELESS_CLONED_MAC_ADDRESS)
DEFINE_GETTER (nmc_property_wireless_get_mac_address_blacklist, NM_SETTING_WIRELESS_MAC_ADDRESS_BLACKLIST)
DEFINE_GETTER (nmc_property_wireless_get_seen_bssids, NM_SETTING_WIRELESS_SEEN_BSSIDS)
DEFINE_GETTER (nmc_property_wireless_get_sec, NM_SETTING_WIRELESS_SEC)
DEFINE_GETTER (nmc_property_wireless_get_hidden, NM_SETTING_WIRELESS_HIDDEN)

static char *
nmc_property_wireless_get_ssid (NMSetting *setting)
{
	NMSettingWireless *s_wireless = NM_SETTING_WIRELESS (setting);
	const GByteArray *ssid;
	char *ssid_str = NULL;

	ssid = nm_setting_wireless_get_ssid (s_wireless);
	if (ssid)
		ssid_str = ssid_to_printable ((const char *) ssid->data, ssid->len);

	return ssid_str;
}

static char *
nmc_property_wireless_get_mtu (NMSetting *setting)
{
	NMSettingWireless *s_wireless = NM_SETTING_WIRELESS (setting);
	int mtu;

	mtu = nm_setting_wireless_get_mtu (s_wireless);
	if (mtu == 0)
		return g_strdup (_("auto"));
	else
		return g_strdup_printf ("%d", nm_setting_wireless_get_mtu (s_wireless));
}

/* --- NM_SETTING_WIRELESS_SECURITY_SETTING_NAME property get functions --- */
DEFINE_GETTER (nmc_property_wifi_sec_get_key_mgmt, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT)
DEFINE_GETTER (nmc_property_wifi_sec_get_wep_tx_keyidx, NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX)
DEFINE_GETTER (nmc_property_wifi_sec_get_auth_alg, NM_SETTING_WIRELESS_SECURITY_AUTH_ALG)
DEFINE_GETTER (nmc_property_wifi_sec_get_proto, NM_SETTING_WIRELESS_SECURITY_PROTO)
DEFINE_GETTER (nmc_property_wifi_sec_get_pairwise, NM_SETTING_WIRELESS_SECURITY_PAIRWISE)
DEFINE_GETTER (nmc_property_wifi_sec_get_group, NM_SETTING_WIRELESS_SECURITY_GROUP)
DEFINE_GETTER (nmc_property_wifi_sec_get_leap_username, NM_SETTING_WIRELESS_SECURITY_LEAP_USERNAME)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_wifi_sec_get_wep_key_flags, NM_SETTING_WIRELESS_SECURITY_WEP_KEY_FLAGS)
DEFINE_GETTER (nmc_property_wifi_sec_get_psk, NM_SETTING_WIRELESS_SECURITY_PSK)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_wifi_sec_get_psk_flags, NM_SETTING_WIRELESS_SECURITY_PSK_FLAGS)
DEFINE_GETTER (nmc_property_wifi_sec_get_leap_password, NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD)
DEFINE_SECRET_FLAGS_GETTER (nmc_property_wifi_sec_get_leap_password_flags, NM_SETTING_WIRELESS_SECURITY_LEAP_PASSWORD_FLAGS)

static char *
nmc_property_wifi_sec_get_wep_key0 (NMSetting *setting)
{
	NMSettingWirelessSecurity *s_wireless_sec = NM_SETTING_WIRELESS_SECURITY (setting);
	return g_strdup (nm_setting_wireless_security_get_wep_key (s_wireless_sec, 0));
}

static char *
nmc_property_wifi_sec_get_wep_key1 (NMSetting *setting)
{
	NMSettingWirelessSecurity *s_wireless_sec = NM_SETTING_WIRELESS_SECURITY (setting);
	return g_strdup (nm_setting_wireless_security_get_wep_key (s_wireless_sec, 1));
}

static char *
nmc_property_wifi_sec_get_wep_key2 (NMSetting *setting)
{
	NMSettingWirelessSecurity *s_wireless_sec = NM_SETTING_WIRELESS_SECURITY (setting);
	return g_strdup (nm_setting_wireless_security_get_wep_key (s_wireless_sec, 2));
}

static char *
nmc_property_wifi_sec_get_wep_key3 (NMSetting *setting)
{
	NMSettingWirelessSecurity *s_wireless_sec = NM_SETTING_WIRELESS_SECURITY (setting);
	return g_strdup (nm_setting_wireless_security_get_wep_key (s_wireless_sec, 3));
}

static char *
nmc_property_wifi_sec_get_wep_key_type (NMSetting *setting)
{
	NMSettingWirelessSecurity *s_wireless_sec = NM_SETTING_WIRELESS_SECURITY (setting);
	return wep_key_type_to_string (nm_setting_wireless_security_get_wep_key_type (s_wireless_sec));
}

/*----------------------------------------------------------------------------*/

static void
nmc_value_transform_bool_string (const GValue *src_value,
                                 GValue       *dest_value)
{
	dest_value->data[0].v_pointer = g_strdup (src_value->data[0].v_int ? "yes" : "no");
}

static void
nmc_value_transform_char_string (const GValue *src_value,
                                 GValue       *dest_value)
{
	dest_value->data[0].v_pointer = g_strdup_printf ("%c", src_value->data[0].v_uint);
}

static void __attribute__((constructor))
register_nmcli_value_transforms (void)
{
	g_value_register_transform_func (G_TYPE_BOOLEAN, G_TYPE_STRING, nmc_value_transform_bool_string);
	g_value_register_transform_func (G_TYPE_CHAR, G_TYPE_STRING, nmc_value_transform_char_string);
}

/*----------------------------------------------------------------------------*/

gboolean
setting_details (NMSetting *setting, NmCli *nmc)
{
	GType setting_type;

	g_return_val_if_fail (NM_IS_SETTING (setting), FALSE);

	setting_type = G_OBJECT_TYPE (setting);

	if (setting_type == NM_TYPE_SETTING_CONNECTION)
		return setting_connection_details (NM_SETTING_CONNECTION (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_WIRED)
		return setting_wired_details (NM_SETTING_WIRED (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_802_1X)
		return setting_802_1X_details (NM_SETTING_802_1X (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_WIRELESS)
		return setting_wireless_details (NM_SETTING_WIRELESS (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_WIRELESS_SECURITY)
		return setting_wireless_security_details (NM_SETTING_WIRELESS_SECURITY (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_IP4_CONFIG)
		return setting_ip4_config_details (NM_SETTING_IP4_CONFIG (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_IP6_CONFIG)
		return setting_ip6_config_details (NM_SETTING_IP6_CONFIG (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_SERIAL)
		return setting_serial_details (NM_SETTING_SERIAL (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_PPP)
		return setting_ppp_details (NM_SETTING_PPP (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_PPPOE)
		return setting_pppoe_details (NM_SETTING_PPPOE (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_GSM)
		return setting_gsm_details (NM_SETTING_GSM (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_CDMA)
		return setting_cdma_details (NM_SETTING_CDMA (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_BLUETOOTH)
		return setting_bluetooth_details (NM_SETTING_BLUETOOTH (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_OLPC_MESH)
		return setting_olpc_mesh_details (NM_SETTING_OLPC_MESH (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_VPN)
		return setting_vpn_details (NM_SETTING_VPN (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_WIMAX)
		return setting_wimax_details (NM_SETTING_WIMAX (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_INFINIBAND)
		return setting_infiniband_details (NM_SETTING_INFINIBAND (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_BOND)
		return setting_bond_details (NM_SETTING_BOND (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_VLAN)
		return setting_vlan_details (NM_SETTING_VLAN (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_ADSL)
		return setting_adsl_details (NM_SETTING_ADSL (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_BRIDGE)
		return setting_bridge_details (NM_SETTING_BRIDGE (setting), nmc);
	else if (setting_type == NM_TYPE_SETTING_BRIDGE_PORT)
		return setting_bridge_port_details (NM_SETTING_BRIDGE_PORT (setting), nmc);
	else
		/* should not be reached */
		return FALSE;
}

gboolean
setting_connection_details (NMSettingConnection *s_con, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_con);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_CONNECTION (s_con), FALSE);

	nmc->allowed_fields = nmc_fields_setting_connection;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_CONNECTION_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_connection_get_id (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_connection_get_uuid (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_connection_get_interface_name (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_connection_get_type (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_connection_get_autoconnect (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_connection_get_timestamp (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_connection_get_read_only (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_connection_get_permissions (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_connection_get_zone (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_connection_get_master (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_connection_get_slave_type (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_connection_get_secondaries (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_wired_details (NMSettingWired *s_wired, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_wired);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_WIRED (s_wired), FALSE);

	nmc->allowed_fields = nmc_fields_setting_wired;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_WIRED_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_wired_get_port (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_wired_get_speed (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_wired_get_duplex (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_wired_get_auto_negotiate (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_wired_get_mac_address (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_wired_get_cloned_mac_address (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_wired_get_mac_address_blacklist (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_wired_get_mtu (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_wired_get_s390_subchannels (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_wired_get_s390_nettype (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_wired_get_s390_options (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_802_1X_details (NMSetting8021x *s_8021X, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_8021X);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_802_1X (s_8021X), FALSE);

	nmc->allowed_fields = nmc_fields_setting_8021X;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_802_1X_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_802_1X_get_eap (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_802_1X_get_identity (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_802_1X_get_anonymous_identity (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_802_1X_get_pac_file (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_802_1X_get_ca_cert (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_802_1X_get_ca_path (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_802_1X_get_subject_match (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_802_1X_get_altsubject_matches (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_802_1X_get_client_cert (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_802_1X_get_phase1_peapver (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_802_1X_get_phase1_peaplabel (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_802_1X_get_phase1_fast_provisioning (setting));
	set_val_str (nmc->allowed_fields, 13, nmc_property_802_1X_get_phase2_auth (setting));
	set_val_str (nmc->allowed_fields, 14, nmc_property_802_1X_get_phase2_autheap (setting));
	set_val_str (nmc->allowed_fields, 15, nmc_property_802_1X_get_phase2_ca_cert (setting));
	set_val_str (nmc->allowed_fields, 16, nmc_property_802_1X_get_phase2_ca_path (setting));
	set_val_str (nmc->allowed_fields, 17, nmc_property_802_1X_get_phase2_subject_match (setting));
	set_val_str (nmc->allowed_fields, 18, nmc_property_802_1X_get_phase2_altsubject_matches (setting));
	set_val_str (nmc->allowed_fields, 19, nmc_property_802_1X_get_phase2_client_cert (setting));
	set_val_str (nmc->allowed_fields, 20, nmc_property_802_1X_get_password (setting));
	set_val_str (nmc->allowed_fields, 21, nmc_property_802_1X_get_password_flags (setting));
	set_val_str (nmc->allowed_fields, 22, nmc_property_802_1X_get_password_raw (setting));
	set_val_str (nmc->allowed_fields, 23, nmc_property_802_1X_get_password_raw_flags (setting));
	set_val_str (nmc->allowed_fields, 24, nmc_property_802_1X_get_private_key (setting));
	set_val_str (nmc->allowed_fields, 25, nmc_property_802_1X_get_private_key_password (setting));
	set_val_str (nmc->allowed_fields, 26, nmc_property_802_1X_get_private_key_password_flags (setting));
	set_val_str (nmc->allowed_fields, 27, nmc_property_802_1X_get_phase2_private_key (setting));
	set_val_str (nmc->allowed_fields, 28, nmc_property_802_1X_get_phase2_private_key_password (setting));
	set_val_str (nmc->allowed_fields, 29, nmc_property_802_1X_get_phase2_private_key_password_flags (setting));
	set_val_str (nmc->allowed_fields, 30, nmc_property_802_1X_get_pin (setting));
	set_val_str (nmc->allowed_fields, 31, nmc_property_802_1X_get_pin_flags (setting));
	set_val_str (nmc->allowed_fields, 32, nmc_property_802_1X_get_system_ca_certs (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_wireless_details (NMSettingWireless *s_wireless, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_wireless);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_WIRELESS (s_wireless), FALSE);

	nmc->allowed_fields = nmc_fields_setting_wireless;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_WIRELESS_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_wireless_get_ssid (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_wireless_get_mode (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_wireless_get_band (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_wireless_get_channel (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_wireless_get_bssid (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_wireless_get_rate (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_wireless_get_tx_power (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_wireless_get_mac_address (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_wireless_get_cloned_mac_address (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_wireless_get_mac_address_blacklist (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_wireless_get_mtu (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_wireless_get_seen_bssids (setting));
	set_val_str (nmc->allowed_fields, 13, nmc_property_wireless_get_sec (setting));
	set_val_str (nmc->allowed_fields, 14, nmc_property_wireless_get_hidden (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_wireless_security_details (NMSettingWirelessSecurity *s_wireless_sec, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_wireless_sec);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_WIRELESS_SECURITY (s_wireless_sec), FALSE);

	nmc->allowed_fields = nmc_fields_setting_wireless_security;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_WIRELESS_SECURITY_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_wifi_sec_get_key_mgmt (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_wifi_sec_get_wep_tx_keyidx (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_wifi_sec_get_auth_alg (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_wifi_sec_get_proto (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_wifi_sec_get_pairwise (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_wifi_sec_get_group (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_wifi_sec_get_leap_username (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_wifi_sec_get_wep_key0 (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_wifi_sec_get_wep_key1 (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_wifi_sec_get_wep_key2 (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_wifi_sec_get_wep_key3 (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_wifi_sec_get_wep_key_flags (setting));
	set_val_str (nmc->allowed_fields, 13, nmc_property_wifi_sec_get_wep_key_type (setting));
	set_val_str (nmc->allowed_fields, 14, nmc_property_wifi_sec_get_psk (setting));
	set_val_str (nmc->allowed_fields, 15, nmc_property_wifi_sec_get_psk_flags (setting));
	set_val_str (nmc->allowed_fields, 16, nmc_property_wifi_sec_get_leap_password (setting));
	set_val_str (nmc->allowed_fields, 17, nmc_property_wifi_sec_get_leap_password_flags (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_ip4_config_details (NMSettingIP4Config *s_ip4, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_ip4);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_IP4_CONFIG (s_ip4), FALSE);

	nmc->allowed_fields = nmc_fields_setting_ip4_config;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_IP4_CONFIG_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_ipv4_get_method (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_ipv4_get_dns (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_ipv4_get_dns_search (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_ipv4_get_addresses (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_ipv4_get_routes (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_ipv4_get_ignore_auto_routes (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_ipv4_get_ignore_auto_dns (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_ipv4_get_dhcp_client_id (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_ipv4_get_dhcp_send_hostname (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_ipv4_get_dhcp_hostname (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_ipv4_get_never_default (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_ipv4_get_may_fail (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_ip6_config_details (NMSettingIP6Config *s_ip6, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_ip6);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_IP6_CONFIG (s_ip6), FALSE);

	nmc->allowed_fields = nmc_fields_setting_ip6_config;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_IP6_CONFIG_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_ipv6_get_method (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_ipv6_get_dns (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_ipv6_get_dns_search (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_ipv6_get_addresses (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_ipv6_get_routes (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_ipv6_get_ignore_auto_routes (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_ipv6_get_ignore_auto_dns (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_ipv6_get_never_default (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_ipv6_get_may_fail (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_ipv6_get_ip6_privacy (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_ipv6_get_dhcp_hostname (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_serial_details (NMSettingSerial *s_serial, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_serial);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_SERIAL (s_serial), FALSE);

	nmc->allowed_fields = nmc_fields_setting_serial;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_SERIAL_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_serial_get_baud (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_serial_get_bits (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_serial_get_parity (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_serial_get_stopbits (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_serial_get_send_delay (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_ppp_details (NMSettingPPP *s_ppp, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_ppp);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_PPP (s_ppp), FALSE);

	nmc->allowed_fields = nmc_fields_setting_ppp;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_PPP_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_ppp_get_noauth (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_ppp_get_refuse_eap (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_ppp_get_refuse_pap (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_ppp_get_refuse_chap (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_ppp_get_refuse_mschap (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_ppp_get_refuse_mschapv2 (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_ppp_get_nobsdcomp (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_ppp_get_nodeflate (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_ppp_get_no_vj_comp (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_ppp_get_require_mppe (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_ppp_get_require_mppe_128 (setting));
	set_val_str (nmc->allowed_fields, 12, nmc_property_ppp_get_mppe_stateful (setting));
	set_val_str (nmc->allowed_fields, 13, nmc_property_ppp_get_crtscts (setting));
	set_val_str (nmc->allowed_fields, 14, nmc_property_ppp_get_baud (setting));
	set_val_str (nmc->allowed_fields, 15, nmc_property_ppp_get_mru (setting));
	set_val_str (nmc->allowed_fields, 16, nmc_property_ppp_get_mtu (setting));
	set_val_str (nmc->allowed_fields, 17, nmc_property_ppp_get_lcp_echo_failure (setting));
	set_val_str (nmc->allowed_fields, 18, nmc_property_ppp_get_lcp_echo_interval (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_pppoe_details (NMSettingPPPOE *s_pppoe, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_pppoe);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_PPPOE (s_pppoe), FALSE);

	nmc->allowed_fields = nmc_fields_setting_pppoe;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_PPPOE_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_pppoe_get_service (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_pppoe_get_username (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_pppoe_get_password (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_pppoe_get_password_flags (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_gsm_details (NMSettingGsm *s_gsm, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_gsm);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_GSM (s_gsm), FALSE);

	nmc->allowed_fields = nmc_fields_setting_gsm;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_GSM_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_gsm_get_number (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_gsm_get_username (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_gsm_get_password (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_gsm_get_password_flags (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_gsm_get_apn (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_gsm_get_network_id (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_gsm_get_network_type (setting));
	set_val_str (nmc->allowed_fields, 8, nmc_property_gsm_get_allowed_bands (setting));
	set_val_str (nmc->allowed_fields, 9, nmc_property_gsm_get_pin (setting));
	set_val_str (nmc->allowed_fields, 10, nmc_property_gsm_get_pin_flags (setting));
	set_val_str (nmc->allowed_fields, 11, nmc_property_gsm_get_home_only (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_cdma_details (NMSettingCdma *s_cdma, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_cdma);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_CDMA (s_cdma), FALSE);

	nmc->allowed_fields = nmc_fields_setting_cdma;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_CDMA_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_cdma_get_number (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_cdma_get_username (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_cdma_get_password (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_cdma_get_password_flags (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_bluetooth_details (NMSettingBluetooth *s_bluetooth, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_bluetooth);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_BLUETOOTH (s_bluetooth), FALSE);

	nmc->allowed_fields = nmc_fields_setting_bluetooth;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_BLUETOOTH_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_bluetooth_get_bdaddr (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_bluetooth_get_type (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_olpc_mesh_details (NMSettingOlpcMesh *s_olpc_mesh, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_olpc_mesh);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_OLPC_MESH (s_olpc_mesh), FALSE);

	nmc->allowed_fields = nmc_fields_setting_olpc_mesh;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_OLPC_MESH_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_olpc_get_ssid (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_olpc_get_channel (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_olpc_get_anycast_address (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_vpn_details (NMSettingVPN *s_vpn, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_vpn);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_VPN (s_vpn), FALSE);

	nmc->allowed_fields = nmc_fields_setting_vpn;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_VPN_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_vpn_get_service_type (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_vpn_get_user_name (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_vpn_get_data (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_vpn_get_secrets (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_wimax_details (NMSettingWimax *s_wimax, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_wimax);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_WIMAX (s_wimax), FALSE);

	nmc->allowed_fields = nmc_fields_setting_wimax;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_WIMAX_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_wimax_get_mac_address (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_wimax_get_network_name (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_infiniband_details (NMSettingInfiniband *s_infiniband, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_infiniband);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_INFINIBAND (s_infiniband), FALSE);

	nmc->allowed_fields = nmc_fields_setting_infiniband;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_INFINIBAND_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_ib_get_mac_address (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_ib_get_mtu (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_ib_get_transport_mode (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_bond_details (NMSettingBond *s_bond, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_bond);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_BOND (s_bond), FALSE);

	nmc->allowed_fields = nmc_fields_setting_bond;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_BOND_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_bond_get_interface_name (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_bond_get_options (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_vlan_details (NMSettingVlan *s_vlan, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_vlan);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_VLAN (s_vlan), FALSE);

	nmc->allowed_fields = nmc_fields_setting_vlan;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_VLAN_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_vlan_get_interface_name (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_vlan_get_parent (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_vlan_get_id (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_vlan_get_flags (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_vlan_get_ingress_priority_map (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_vlan_get_egress_priority_map (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_adsl_details (NMSettingAdsl *s_adsl, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_adsl);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_ADSL (s_adsl), FALSE);

	nmc->allowed_fields = nmc_fields_setting_adsl;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_ADSL_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_adsl_get_username (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_adsl_get_password (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_adsl_get_password_flags (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_adsl_get_protocol (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_adsl_get_encapsulation (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_adsl_get_vpi (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_adsl_get_vci (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_bridge_details (NMSettingBridge *s_bridge, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_bridge);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_BRIDGE (s_bridge), FALSE);

	nmc->allowed_fields = nmc_fields_setting_bridge;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_BRIDGE_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_bridge_get_interface_name (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_bridge_get_stp (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_bridge_get_priority (setting));
	set_val_str (nmc->allowed_fields, 4, nmc_property_bridge_get_forward_delay (setting));
	set_val_str (nmc->allowed_fields, 5, nmc_property_bridge_get_hello_time (setting));
	set_val_str (nmc->allowed_fields, 6, nmc_property_bridge_get_max_age (setting));
	set_val_str (nmc->allowed_fields, 7, nmc_property_bridge_get_ageing_time (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

gboolean
setting_bridge_port_details (NMSettingBridgePort *s_bridge_port, NmCli *nmc)
{
	NMSetting *setting = NM_SETTING (s_bridge_port);
	guint32 mode_flag = (nmc->print_output == NMC_PRINT_PRETTY) ? NMC_PF_FLAG_PRETTY : (nmc->print_output == NMC_PRINT_TERSE) ? NMC_PF_FLAG_TERSE : 0;
	guint32 multiline_flag = nmc->multiline_output ? NMC_PF_FLAG_MULTILINE : 0;
	guint32 escape_flag = nmc->escape_values ? NMC_PF_FLAG_ESCAPE : 0;

	g_return_val_if_fail (NM_IS_SETTING_BRIDGE_PORT (s_bridge_port), FALSE);

	nmc->allowed_fields = nmc_fields_setting_bridge_port;
	nmc->print_fields.indices = parse_output_fields (NMC_FIELDS_SETTING_BRIDGE_PORT_ALL, nmc->allowed_fields, NULL);
	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_FIELD_NAMES;
	print_fields (nmc->print_fields, nmc->allowed_fields);  /* Print field names */

	set_val_str (nmc->allowed_fields, 0, g_strdup (nm_setting_get_name (setting)));
	set_val_str (nmc->allowed_fields, 1, nmc_property_bridge_port_get_priority (setting));
	set_val_str (nmc->allowed_fields, 2, nmc_property_bridge_port_get_path_cost (setting));
	set_val_str (nmc->allowed_fields, 3, nmc_property_bridge_port_get_hairpin_mode (setting));

	nmc->print_fields.flags = multiline_flag | mode_flag | escape_flag | NMC_PF_FLAG_SECTION_PREFIX;
	print_fields (nmc->print_fields, nmc->allowed_fields); /* Print values */

	nmc_free_output_field_values (nmc->allowed_fields);
	return TRUE;
}

