/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager
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
 * Copyright (C) 2008–2013 Red Hat, Inc.
 */

#ifndef __NETWORKMANAGER_IP6_CONFIG_H__
#define __NETWORKMANAGER_IP6_CONFIG_H__

#include <netinet/in.h>

#include "nm-exported-object.h"
#include "nm-setting-ip6-config.h"

#include "nm-utils/nm-dedup-multi.h"
#include "platform/nmp-object.h"

/*****************************************************************************/

void nm_ip_config_iter_ip6_address_init (NMDedupMultiIter *iter, const NMIP6Config *self);
void nm_ip_config_iter_ip6_route_init (NMDedupMultiIter *iter, const NMIP6Config *self);

static inline gboolean
nm_ip_config_iter_ip6_address_next (NMDedupMultiIter *ipconf_iter, const NMPlatformIP6Address **out_address)
{
	gboolean has_next;

	g_return_val_if_fail (out_address, FALSE);

	has_next = nm_dedup_multi_iter_next (ipconf_iter);
	if (has_next)
		*out_address = NMP_OBJECT_CAST_IP6_ADDRESS (ipconf_iter->current->obj);
	return has_next;
}

static inline gboolean
nm_ip_config_iter_ip6_route_next (NMDedupMultiIter *ipconf_iter, const NMPlatformIP6Route **out_route)
{
	gboolean has_next;

	g_return_val_if_fail (out_route, FALSE);

	has_next = nm_dedup_multi_iter_next (ipconf_iter);
	if (has_next)
		*out_route = NMP_OBJECT_CAST_IP6_ROUTE (ipconf_iter->current->obj);
	return has_next;
}

#define nm_ip_config_iter_ip6_address_for_each(iter, self, address) \
    for (*(address) = NULL, nm_ip_config_iter_ip6_address_init ((iter), (self)); \
         nm_ip_config_iter_ip6_address_next ((iter), (address)); \
         )

#define nm_ip_config_iter_ip6_route_for_each(iter, self, route) \
    for (*(route) = NULL, nm_ip_config_iter_ip6_route_init ((iter), (self)); \
         nm_ip_config_iter_ip6_route_next ((iter), (route)); \
         )

/*****************************************************************************/

#define NM_TYPE_IP6_CONFIG (nm_ip6_config_get_type ())
#define NM_IP6_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_IP6_CONFIG, NMIP6Config))
#define NM_IP6_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_IP6_CONFIG, NMIP6ConfigClass))
#define NM_IS_IP6_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_IP6_CONFIG))
#define NM_IS_IP6_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NM_TYPE_IP6_CONFIG))
#define NM_IP6_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_IP6_CONFIG, NMIP6ConfigClass))

typedef struct _NMIP6ConfigClass NMIP6ConfigClass;

/* internal */
#define NM_IP6_CONFIG_MULTI_IDX "multi-idx"
#define NM_IP6_CONFIG_IFINDEX "ifindex"

/* public */
#define NM_IP6_CONFIG_ADDRESS_DATA "address-data"
#define NM_IP6_CONFIG_ROUTE_DATA "route-data"
#define NM_IP6_CONFIG_GATEWAY "gateway"
#define NM_IP6_CONFIG_NAMESERVERS "nameservers"
#define NM_IP6_CONFIG_DOMAINS "domains"
#define NM_IP6_CONFIG_SEARCHES "searches"
#define NM_IP6_CONFIG_DNS_OPTIONS "dns-options"
#define NM_IP6_CONFIG_DNS_PRIORITY "dns-priority"

/* deprecated */
#define NM_IP6_CONFIG_ADDRESSES "addresses"
#define NM_IP6_CONFIG_ROUTES "routes"

GType nm_ip6_config_get_type (void);


NMIP6Config * nm_ip6_config_new (struct _NMDedupMultiIndex *multi_idx, int ifindex);
NMIP6Config * nm_ip6_config_new_cloned (const NMIP6Config *src);

int nm_ip6_config_get_ifindex (const NMIP6Config *self);

struct _NMDedupMultiIndex *nm_ip6_config_get_multi_idx (const NMIP6Config *self);

NMIP6Config *nm_ip6_config_capture (struct _NMDedupMultiIndex *multi_idx, NMPlatform *platform, int ifindex,
                                    gboolean capture_resolv_conf, NMSettingIP6ConfigPrivacy use_temporary);
gboolean nm_ip6_config_commit (const NMIP6Config *self,
                               NMPlatform *platform,
                               NMRouteManager *route_manager,
                               int ifindex,
                               gboolean routes_full_sync);
void nm_ip6_config_merge_setting (NMIP6Config *self, NMSettingIPConfig *setting, guint32 default_route_metric);
NMSetting *nm_ip6_config_create_setting (const NMIP6Config *self);


void nm_ip6_config_merge (NMIP6Config *dst, const NMIP6Config *src, NMIPConfigMergeFlags merge_flags);
void nm_ip6_config_subtract (NMIP6Config *dst, const NMIP6Config *src);
void nm_ip6_config_intersect (NMIP6Config *dst, const NMIP6Config *src);
gboolean nm_ip6_config_replace (NMIP6Config *dst, const NMIP6Config *src, gboolean *relevant_changes);
int nm_ip6_config_destination_is_direct (const NMIP6Config *self, const struct in6_addr *dest, guint8 plen);
void nm_ip6_config_dump (const NMIP6Config *self, const char *detail);


void nm_ip6_config_set_never_default (NMIP6Config *self, gboolean never_default);
gboolean nm_ip6_config_get_never_default (const NMIP6Config *self);
void nm_ip6_config_set_gateway (NMIP6Config *self, const struct in6_addr *);
const struct in6_addr *nm_ip6_config_get_gateway (const NMIP6Config *self);
gint64 nm_ip6_config_get_route_metric (const NMIP6Config *self);

const NMDedupMultiHeadEntry *nm_ip6_config_lookup_addresses (const NMIP6Config *self);
void nm_ip6_config_reset_addresses (NMIP6Config *self);
void nm_ip6_config_add_address (NMIP6Config *self, const NMPlatformIP6Address *address);
void _nmtst_nm_ip6_config_del_address (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_addresses (const NMIP6Config *self);
const NMPlatformIP6Address *nm_ip6_config_get_first_address (const NMIP6Config *self);
const NMPlatformIP6Address *_nmtst_nm_ip6_config_get_address (const NMIP6Config *self, guint i);
const NMPlatformIP6Address *nm_ip6_config_get_address_first_nontentative (const NMIP6Config *self, gboolean linklocal);
gboolean nm_ip6_config_address_exists (const NMIP6Config *self, const NMPlatformIP6Address *address);
const NMPlatformIP6Address *nm_ip6_config_lookup_address (const NMIP6Config *self,
                                                          const struct in6_addr *addr);
gboolean _nmtst_nm_ip6_config_addresses_sort (NMIP6Config *self);
gboolean nm_ip6_config_has_any_dad_pending (const NMIP6Config *self,
                                            const NMIP6Config *candidates);

const NMDedupMultiHeadEntry *nm_ip6_config_lookup_routes (const NMIP6Config *self);
void nm_ip6_config_reset_routes (NMIP6Config *self);
void nm_ip6_config_add_route (NMIP6Config *self, const NMPlatformIP6Route *route);
void _nmtst_ip6_config_del_route (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_routes (const NMIP6Config *self);
const NMPlatformIP6Route *_nmtst_ip6_config_get_route (const NMIP6Config *self, guint i);

const NMPlatformIP6Route *nm_ip6_config_get_direct_route_for_host (const NMIP6Config *self, const struct in6_addr *host);
const NMPlatformIP6Address *nm_ip6_config_get_subnet_for_host (const NMIP6Config *self, const struct in6_addr *host);

void nm_ip6_config_reset_nameservers (NMIP6Config *self);
void nm_ip6_config_add_nameserver (NMIP6Config *self, const struct in6_addr *nameserver);
void nm_ip6_config_del_nameserver (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_nameservers (const NMIP6Config *self);
const struct in6_addr *nm_ip6_config_get_nameserver (const NMIP6Config *self, guint i);

void nm_ip6_config_reset_domains (NMIP6Config *self);
void nm_ip6_config_add_domain (NMIP6Config *self, const char *domain);
void nm_ip6_config_del_domain (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_domains (const NMIP6Config *self);
const char * nm_ip6_config_get_domain (const NMIP6Config *self, guint i);

void nm_ip6_config_reset_searches (NMIP6Config *self);
void nm_ip6_config_add_search (NMIP6Config *self, const char *search);
void nm_ip6_config_del_search (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_searches (const NMIP6Config *self);
const char * nm_ip6_config_get_search (const NMIP6Config *self, guint i);

void nm_ip6_config_reset_dns_options (NMIP6Config *self);
void nm_ip6_config_add_dns_option (NMIP6Config *self, const char *option);
void nm_ip6_config_del_dns_option (NMIP6Config *self, guint i);
guint nm_ip6_config_get_num_dns_options (const NMIP6Config *self);
const char * nm_ip6_config_get_dns_option (const NMIP6Config *self, guint i);

void nm_ip6_config_set_dns_priority (NMIP6Config *self, gint priority);
gint nm_ip6_config_get_dns_priority (const NMIP6Config *self);

void nm_ip6_config_set_mss (NMIP6Config *self, guint32 mss);
guint32 nm_ip6_config_get_mss (const NMIP6Config *self);

void nm_ip6_config_hash (const NMIP6Config *self, GChecksum *sum, gboolean dns_only);
gboolean nm_ip6_config_equal (const NMIP6Config *a, const NMIP6Config *b);

void nm_ip6_config_set_privacy (NMIP6Config *self, NMSettingIP6ConfigPrivacy privacy);

/*****************************************************************************/
/* Testing-only functions */

gboolean nm_ip6_config_capture_resolv_conf (GArray *nameservers,
                                            GPtrArray *dns_options,
                                            const char *rc_contents);

#endif /* __NETWORKMANAGER_IP6_CONFIG_H__ */
