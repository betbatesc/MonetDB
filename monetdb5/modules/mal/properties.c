/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2021 MonetDB B.V.
 */

/*
 *  Martin Kersten
 * The support routines for opt_properties
 */

#include "monetdb_config.h"
#include "properties.h"
#include "gdk_calc.h"
#include "mal_exception.h"


// dummy procedures to monitor the generation of the properties code.
str
PROPget(Client cntxt, MalBlkPtr mb, MalStkPtr stk, InstrPtr pci)
{
	(void) cntxt;
	(void) mb;
	(void) stk;
	(void) pci;

	return MAL_SUCCEED;
}

str
PROPbind(Client cntxt, MalBlkPtr mb, MalStkPtr stk, InstrPtr pci)
{
	// beware the bind() returns a view on the original BAT
	//bat orig = *getArgReference_bat(stk, pci, 1);
	//bat bid = (bat) *getArgReference_int(stk, pci, 2);
	(void) cntxt;
	(void) mb;
	(void) stk;
	(void) pci;

	//if( orig != bid)
		//throw(MAL,"properties.bind"," Incompatible BAT references ");
	return MAL_SUCCEED;
}

#include "mel.h"
mel_func properties_init_funcs[] = {
 pattern("properties", "bind", PROPbind, false, "Retrieve actual properties from a BAT", args(1,3,  arg("",void),batargany("",1),arg("min",int))),
 pattern("properties", "get", PROPget, false, "Retrieve actual properties from a BAT", args(1,5,  arg("",void),batargany("",1),arg("min",bte),arg("max",bte),arg("nils",lng) )),
 pattern("properties", "get", PROPget, false, "Retrieve actual properties from a BAT", args(1,5,  arg("",void),batargany("",1),arg("min",sht),arg("max",sht),arg("nils",lng) )),
 pattern("properties", "get", PROPget, false, "Retrieve actual properties from a BAT", args(1,5,  arg("",void),batargany("",1),arg("min",int),arg("max",int),arg("nils",lng) )),
 pattern("properties", "get", PROPget, false, "Retrieve actual properties from a BAT", args(1,5,  arg("",void),batargany("",1),arg("min",lng),arg("max",lng),arg("nils",lng) )),
 { .imp=NULL }
};
#include "mal_import.h"
#ifdef _MSC_VER
#undef read
#pragma section(".CRT$XCU",read)
#endif
LIB_STARTUP_FUNC(init_querylog_mal)
{ mal_module("properties", NULL, properties_init_funcs); }
