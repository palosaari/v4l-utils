/*
 * Copyright (c) 2013 - Mauro Carvalho Chehab <m.chehab@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * Described on ARIB STD-B10 as TS information descriptor
 */

#include "descriptors.h"
#include "descriptors/desc_ts_info.h"
#include "dvb-fe.h"
#include "parse_string.h"

void dvb_desc_ts_info_init(struct dvb_v5_fe_parms *parms,
			      const uint8_t *buf, struct dvb_desc *desc)
{
	struct dvb_desc_ts_info *d = (void *)desc;
	unsigned char *p = (unsigned char *)buf;
	struct dvb_desc_ts_info_transmission_type *t;
	size_t len;
	int i;

	len = sizeof(*d) - offsetof(struct dvb_desc_ts_info, remote_control_key_id);
	memcpy(&d->remote_control_key_id, p, len);
	p += len;

	len = d->length_of_ts_name;
	d->ts_name = NULL;
	d->ts_name_emph = NULL;
	parse_string(parms, &d->ts_name, &d->ts_name_emph, buf, len,
		     default_charset, output_charset);
	p += len;

	memcpy(&d->transmission_type, p, sizeof(d->transmission_type));
	p += sizeof(d->transmission_type);

	t = &d->transmission_type;

	d->service_id = malloc(sizeof(*d->service_id) * t->num_of_service);
	if (!d->service_id)
		dvb_perror("Out of memory!");

	memcpy(d->service_id, p, sizeof(*d->service_id) * t->num_of_service);

	for (i = 0; i < t->num_of_service; i++)
		bswap16(d->service_id[i]);

	p += sizeof(*d->service_id) * t->num_of_service;
}

void dvb_desc_ts_info_print(struct dvb_v5_fe_parms *parms, const struct dvb_desc *desc)
{
	const struct dvb_desc_ts_info *d = (const void *) desc;
	const struct dvb_desc_ts_info_transmission_type *t;
	int i;

	t = &d->transmission_type;

	dvb_log("|        TS Information");
	dvb_log("|           remote key ID     %d", d->remote_control_key_id);
	dvb_log("|           name              %s", d->ts_name);
	dvb_log("|           emphasis name     %s", d->ts_name_emph);
	dvb_log("|           transmission type %s", d->ts_name_emph);

	for (i = 0; i < t->num_of_service; i++)
		dvb_log("|           service ID[%d]     %d", i, d->service_id[i]);
}

void dvb_desc_ts_info_free(struct dvb_desc *desc)
{
	const struct dvb_desc_ts_info *d = (const void *) desc;

	if (d->ts_name)
	      free(d->ts_name);
	if (d->ts_name_emph)
	      free(d->ts_name_emph);

	free(d->service_id);
}
