/*
 * opp_type.h
 *
 *  Created on: Dec 13, 2011
 *      Author: ayaskanti
 */

#ifndef OPP_TYPE_H_
#define OPP_TYPE_H_

struct opp_type {
	int (*callback)(void*data, int callback, void*cb_data, va_list ap);
};

#define opp_convert(x,ytype,zvalue) ({opp_callback2(x, OPPN_ACTION_CONVERT, ytype, &zvalue);})
#define opp_typeof(x,ytype) ({opp_callback(x, OPPN_ACTION_TYPEOF, ytype);})

#define opp_type_new(name) extern const struct opp_type opp_type_name


#endif /* OPP_TYPE_H_ */
