/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2021 MonetDB B.V.
 */

/*
 * opt_prelude
 * M. Kersten
 * These definitions are handy to have around in the optimizer
 */
#include "monetdb_config.h"
#include "opt_prelude.h"
#include "optimizer_private.h"

/* ! please keep this list sorted for easier maintenance ! */
const char *abortRef;
const char *actionRef;
const char *affectedRowsRef;
const char *aggrRef;
const char *alarmRef;
const char *algebraRef;
const char *alter_add_range_partitionRef;
const char *alter_add_tableRef;
const char *alter_add_value_partitionRef;
const char *alter_constraintRef;
const char *alter_del_tableRef;
const char *alter_functionRef;
const char *alter_indexRef;
const char *alter_roleRef;
const char *alter_schemaRef;
const char *alter_seqRef;
const char *alter_set_tableRef;
const char *alter_tableRef;
const char *alter_triggerRef;
const char *alter_typeRef;
const char *alter_userRef;
const char *alter_viewRef;
const char *andRef;
const char *appendidxRef;
const char *appendBulkRef;
const char *appendRef;
const char *arrayRef;
const char *assertRef;
const char *attachRef;
const char *avgRef;
const char *bandjoinRef;
const char *basketRef;
const char *batalgebraRef;
const char *batcalcRef;
const char *batcapiRef;
const char *batmalRef;
const char *batmmathRef;
const char *batmtimeRef;
const char *batpyapi3Ref;
const char *batrapiRef;
const char *batRef;
const char *batsqlRef;
const char *batstrRef;
const char *batxmlRef;
const char *bbpRef;
const char *betweenRef;
const char *binddbatRef;
const char *bindidxRef;
const char *bindRef;
const char *blockRef;
const char *bpmRef;
const char *bstreamRef;
const char *bulk_rotate_xor_hashRef;
const char *calcRef;
const char *capiRef;
const char *catalogRef;
const char *claimRef;
const char *clear_tableRef;
const char *closeRef;
const char *columnBindRef;
const char *columnRef;
const char *comment_onRef;
const char *commitRef;
const char *connectRef;
const char *contextRef;
const char *copy_fromRef;
const char *copyRef;
const char *count_no_nilRef;
const char *countRef;
const char *create_constraintRef;
const char *create_functionRef;
const char *create_indexRef;
const char *createRef;
const char *create_roleRef;
const char *create_schemaRef;
const char *create_seqRef;
const char *create_tableRef;
const char *create_triggerRef;
const char *create_typeRef;
const char *create_userRef;
const char *create_viewRef;
const char *crossRef;
const char *cume_distRef;
const char *dataflowRef;
const char *dateRef;
const char *dblRef;
const char *defineRef;
const char *deleteRef;
const char *deltaRef;
const char *dense_rankRef;
const char *diffcandRef;
const char *differenceRef;
const char *disconnectRef;
const char *divRef;
const char *drop_constraintRef;
const char *drop_functionRef;
const char *drop_indexRef;
const char *drop_roleRef;
const char *drop_schemaRef;
const char *drop_seqRef;
const char *drop_tableRef;
const char *drop_triggerRef;
const char *drop_typeRef;
const char *drop_userRef;
const char *drop_viewRef;
const char *emptybindidxRef;
const char *emptybindRef;
const char *eqRef;
const char *evalRef;
const char *execRef;
const char *expandRef;
const char *exportOperationRef;
const char *export_tableRef;
const char *fetchRef;
const char *findRef;
const char *finishRef;
const char *firstnRef;
const char *first_valueRef;
const char *generatorRef;
const char *getRef;
const char *getTraceRef;
const char *getVariableRef;
const char *grant_functionRef;
const char *grantRef;
const char *grant_rolesRef;
const char *groupbyRef;
const char *group_concatRef;
const char *groupdoneRef;
const char *groupRef;
const char *growRef;
const char *hashRef;
const char *hgeRef;
const char *identityRef;
const char *ifthenelseRef;
const char *importColumnRef;
const char *intersectcandRef;
const char *intersectRef;
const char *intRef;
const char *ioRef;
const char *iteratorRef;
const char *jitRef;
const char *joinRef;
const char *jsonRef;
const char *lagRef;
const char *languageRef;
const char *last_valueRef;
const char *leadRef;
const char *leftjoinRef;
const char *likejoinRef;
const char *likeRef;
const char *likeselectRef;
const char *listRef;
const char *lockRef;
const char *lookupRef;
const char *malRef;
const char *manifoldRef;
const char *mapiRef;
const char *markRef;
const char *maskRef;
const char *matRef;
const char *max_no_nilRef;
const char *maxRef;
const char *mdbRef;
const char *mergecandRef;
const char *mergepackRef;
const char *min_no_nilRef;
const char *minRef;
const char *minusRef;
const char *mirrorRef;
const char *mitosisRef;
const char *mkeyRef;
const char *mmathRef;
const char *modRef;
const char *mtimeRef;
const char *mulRef;
const char *multicolumnRef;
const char *multiplexRef;
const char *mvcRef;
const char *newRef;
const char *nextRef;
const char *not_likeRef;
const char *notRef;
const char *not_uniqueRef;
const char *nth_valueRef;
const char *ntileRef;
const char *oidRef;
const char *oltpRef;
const char *openRef;
const char *optimizerRef;
const char *orRef;
const char *outerjoinRef;
const char *pack2Ref;
const char *packIncrementRef;
const char *packRef;
const char *parametersRef;
const char *partitionRef;
const char *passRef;
const char *pcreRef;
const char *percent_rankRef;
const char *pinRef;
const char *plusRef;
const char *postludeRef;
const char *preludeRef;
const char *printRef;
const char *prodRef;
const char *propertiesRef;
const char *profilerRef;
const char *projectdeltaRef;
const char *projectionpathRef;
const char *projectionRef;
const char *projectRef;
const char *putRef;
const char *pyapi3mapRef;
const char *pyapi3Ref;
const char *querylogRef;
const char *queryRef;
const char *raiseRef;
const char *rangejoinRef;
const char *rankRef;
const char *rapiRef;
const char *reconnectRef;
const char *refineRef;
const char *registerRef;
const char *register_supervisorRef;
const char *releaseRef;
const char *remapRef;
const char *remoteRef;
const char *rename_columnRef;
const char *rename_schemaRef;
const char *rename_tableRef;
const char *rename_userRef;
const char *replaceRef;
const char *replicatorRef;
const char *resultSetRef;
const char *reuseRef;
const char *revoke_functionRef;
const char *revokeRef;
const char *revoke_rolesRef;
const char *rollbackRef;
const char *row_numberRef;
const char *rpcRef;
const char *rsColumnRef;
const char *sampleRef;
const char *schedulerRef;
const char *selectNotNilRef;
const char *selectRef;
const char *semaRef;
const char *semijoinRef;
const char *seriesRef;
const char *setAccessRef;
const char *setVariableRef;
const char *setWriteModeRef;
const char *set_protocolRef;
const char *singleRef;
const char *sinkRef;
const char *sliceRef;
const char *sortRef;
const char *sqlcatalogRef;
const char *sqlRef;
const char *startRef;
const char *starttraceRef;
const char *stoptraceRef;
const char *streamsRef;
const char *strRef;
const char *subavgRef;
const char *subcountRef;
const char *subdeltaRef;
const char *subeval_aggrRef;
const char *subgroupdoneRef;
const char *subgroupRef;
const char *subinterRef;
const char *submaxRef;
const char *submedianRef;
const char *subminRef;
const char *subprodRef;
const char *subsliceRef;
const char *subsumRef;
const char *subuniformRef;
const char *sumRef;
const char *takeRef;
const char *thetajoinRef;
const char *thetaselectRef;
const char *tidRef;
const char *timestampRef;
const char *transaction_abortRef;
const char *transaction_beginRef;
const char *transaction_commitRef;
const char *transactionRef;
const char *transaction_releaseRef;
const char *transaction_rollbackRef;
const char *umaskRef;
const char *uniqueRef;
const char *unlockRef;
const char *unpackRef;
const char *unpinRef;
const char *updateRef;
const char *userRef;
const char *vectorRef;
const char *window_boundRef;
const char *wlcRef;
const char *wlrRef;
const char *zero_or_oneRef;
/* ! please keep this list sorted for easier maintenance ! */

void optimizerInit(void)
{
/* ! please keep this list sorted for easier maintenance ! */
	abortRef = putName("abort");
	actionRef = putName("action");
	affectedRowsRef = putName("affectedRows");
	aggrRef = putName("aggr");
	alarmRef = putName("alarm");
	algebraRef = putName("algebra");
	alter_add_range_partitionRef = putName("alter_add_range_partition");
	alter_add_tableRef = putName("alter_add_table");
	alter_add_value_partitionRef = putName("alter_add_value_partition");
	alter_constraintRef = putName("alter_constraint");
	alter_del_tableRef = putName("alter_del_table");
	alter_functionRef = putName("alter_function");
	alter_indexRef = putName("alter_index");
	alter_roleRef = putName("alter_role");
	alter_schemaRef = putName("alter_schema");
	alter_seqRef = putName("alter_seq");
	alter_set_tableRef = putName("alter_set_table");
	alter_tableRef = putName("alter_table");
	alter_triggerRef = putName("alter_trigger");
	alter_typeRef = putName("alter_type");
	alter_userRef = putName("alter_user");
	alter_viewRef = putName("alter_view");
	andRef = putName("and");
	appendidxRef = putName("append_idxbat");
	appendBulkRef = putName("appendBulk");
	appendRef = putName("append");
	arrayRef = putName("array");
	assertRef = putName("assert");
	attachRef = putName("attach");
	avgRef = putName("avg");
	bandjoinRef = putName("bandjoin");
	basketRef = putName("basket");
	batalgebraRef = putName("batalgebra");
	batcalcRef = putName("batcalc");
	batcapiRef = putName("batcapi");
	batmalRef = putName("batmal");
	batmmathRef = putName("batmmath");
	batmtimeRef = putName("batmtime");
	batpyapi3Ref = putName("batpyapi3");
	batrapiRef = putName("batrapi");
	batRef = putName("bat");
	batsqlRef = putName("batsql");
	batstrRef = putName("batstr");
	batxmlRef = putName("batxml");
	bbpRef = putName("bbp");
	betweenRef = putName("between");
	binddbatRef = putName("bind_dbat");
	bindidxRef = putName("bind_idxbat");
	bindRef = putName("bind");
	blockRef = putName("block");
	bpmRef = putName("bpm");
	bstreamRef = putName("bstream");
	bulk_rotate_xor_hashRef = putName("bulk_rotate_xor_hash");
	calcRef = putName("calc");
	capiRef = putName("capi");
	catalogRef = putName("catalog");
	claimRef = putName("claim");
	clear_tableRef = putName("clear_table");
	closeRef = putName("close");
	columnBindRef = putName("columnBind");
	columnRef = putName("column");
	comment_onRef = putName("comment_on");
	commitRef = putName("commit");
	connectRef = putName("connect");
	contextRef = putName("context");
	copy_fromRef = putName("copy_from");
	copyRef = putName("copy");
	count_no_nilRef = putName("count_no_nil");
	countRef = putName("count");
	create_constraintRef = putName("create_constraint");
	create_functionRef = putName("create_function");
	create_indexRef = putName("create_index");
	createRef = putName("create");
	create_roleRef = putName("create_role");
	create_schemaRef = putName("create_schema");
	create_seqRef = putName("create_seq");
	create_tableRef = putName("create_table");
	create_triggerRef = putName("create_trigger");
	create_typeRef = putName("create_type");
	create_userRef = putName("create_user");
	create_viewRef = putName("create_view");
	crossRef = putName("crossproduct");
	cume_distRef = putName("cume_dist");
	dataflowRef = putName("dataflow");
	dateRef = putName("date");
	dblRef = putName("dbl");
	defineRef = putName("define");
	deleteRef = putName("delete");
	deltaRef = putName("delta");
	dense_rankRef = putName("dense_rank");
	diffcandRef= putName("diffcand");
	differenceRef = putName("difference");
	disconnectRef= putName("disconnect");
	divRef = putName("/");
	drop_constraintRef = putName("drop_constraint");
	drop_functionRef = putName("drop_function");
	drop_indexRef = putName("drop_index");
	drop_roleRef = putName("drop_role");
	drop_schemaRef = putName("drop_schema");
	drop_seqRef = putName("drop_seq");
	drop_tableRef = putName("drop_table");
	drop_triggerRef = putName("drop_trigger");
	drop_typeRef = putName("drop_type");
	drop_userRef = putName("drop_user");
	drop_viewRef = putName("drop_view");
	emptybindidxRef = putName("emptybindidx");
	emptybindRef = putName("emptybind");
	eqRef = putName("==");
	evalRef = putName("eval");
	execRef = putName("exec");
	expandRef = putName("expand");
	exportOperationRef = putName("exportOperation");
	export_tableRef = putName("export_table");
	fetchRef = putName("fetch");
	findRef = putName("find");
	finishRef = putName("finish");
	firstnRef = putName("firstn");
	first_valueRef = putName("first_value");
	generatorRef = putName("generator");
	getRef = putName("get");
	getTraceRef = putName("getTrace");
	getVariableRef = putName("getVariable");
	grant_functionRef = putName("grant_function");
	grantRef = putName("grant");
	grant_rolesRef = putName("grant_roles");
	groupbyRef = putName("groupby");
	group_concatRef = putName("group_concat");
	groupdoneRef = putName("groupdone");
	groupRef = putName("group");
	growRef = putName("grow");
	hashRef = putName("hash");
	hgeRef = putName("hge");
	identityRef = putName("identity");
	ifthenelseRef = putName("ifthenelse");
	importColumnRef = putName("importColumn");
	intersectcandRef= putName("intersectcand");
	intersectRef = putName("intersect");
	intRef = putName("int");
	ioRef = putName("io");
	iteratorRef = putName("iterator");
	jitRef = putName("jit");
	joinRef = putName("join");
	jsonRef = putName("json");
	lagRef = putName("lag");
	languageRef= putName("language");
	last_valueRef = putName("last_value");
	leadRef = putName("lead");
	leftjoinRef = putName("leftjoin");
	likejoinRef = putName("likejoin");
	likeRef = putName("like");
	likeselectRef = putName("likeselect");
	listRef = putName("list");
	lockRef = putName("lock");
	lookupRef = putName("lookup");
	malRef = putName("mal");
	manifoldRef = putName("manifold");
	mapiRef = putName("mapi");
	markRef = putName("mark");
	maskRef = putName("mask");
	matRef = putName("mat");
	max_no_nilRef = putName("max_no_nil");
	maxRef = putName("max");
	mdbRef = putName("mdb");
	mergecandRef= putName("mergecand");
	mergepackRef= putName("mergepack");
	min_no_nilRef = putName("min_no_nil");
	minRef = putName("min");
	minusRef = putName("-");
	mirrorRef = putName("mirror");
	mitosisRef = putName("mitosis");
	mkeyRef = putName("mkey");
	mmathRef = putName("mmath");
	modRef = putName("%");
	mtimeRef = putName("mtime");
	mulRef = putName("*");
	multicolumnRef = putName("multicolumn");
	multiplexRef = putName("multiplex");
	mvcRef = putName("mvc");
	newRef = putName("new");
	nextRef = putName("next");
	not_likeRef = putName("not_like");
	notRef = putName("not");
	not_uniqueRef= putName("not_unique");
	nth_valueRef = putName("nth_value");
	ntileRef = putName("ntile");
	oidRef = putName("oid");
	oltpRef = putName("oltp");
	openRef = putName("open");
	optimizerRef = putName("optimizer");
	orRef = putName("or");
	outerjoinRef = putName("outerjoin");
	pack2Ref = putName("pack2");
	packIncrementRef = putName("packIncrement");
	packRef = putName("pack");
	parametersRef = putName("parameters");
	partitionRef = putName("partition");
	passRef = putName("pass");
	pcreRef = putName("pcre");
	percent_rankRef = putName("percent_rank");
	pinRef = putName("pin");
	plusRef = putName("+");
	postludeRef = putName("postlude");
	preludeRef = putName("prelude");
	printRef = putName("print");
	prodRef = putName("prod");
	propertiesRef = putName("properties");
	profilerRef = putName("profiler");
	projectdeltaRef = putName("projectdelta");
	projectionpathRef = putName("projectionpath");
	projectionRef = putName("projection");
	projectRef = putName("project");
	putRef = putName("put");
	pyapi3mapRef = putName("batpyapi3map");
	pyapi3Ref = putName("pyapi3");
	querylogRef = putName("querylog");
	queryRef = putName("query");
	raiseRef = putName("raise");
	rangejoinRef = putName("rangejoin");
	rankRef = putName("rank");
	rapiRef = putName("rapi");
	reconnectRef = putName("reconnect");
	refineRef = putName("refine");
	registerRef = putName("register");
	register_supervisorRef = putName("register_supervisor");
	releaseRef = putName("release");
	remapRef = putName("remap");
	remoteRef = putName("remote");
	rename_columnRef = putName("rename_column");
	rename_schemaRef = putName("rename_schema");
	rename_tableRef = putName("rename_table");
	rename_userRef = putName("rename_user");
	replaceRef = putName("replace");
	replicatorRef = putName("replicator");
	resultSetRef = putName("resultSet");
	reuseRef = putName("reuse");
	revoke_functionRef = putName("revoke_function");
	revokeRef = putName("revoke");
	revoke_rolesRef = putName("revoke_roles");
	rollbackRef = putName("rollback");
	row_numberRef = putName("row_number");
	rpcRef = putName("rpc");
	rsColumnRef = putName("rsColumn");
	sampleRef= putName("sample");
	schedulerRef = putName("scheduler");
	selectNotNilRef = putName("selectNotNil");
	selectRef = putName("select");
	semaRef = putName("sema");
	semijoinRef = putName("semijoin");
	seriesRef = putName("series");
	setAccessRef = putName("setAccess");
	setVariableRef = putName("setVariable");
	setWriteModeRef= putName("setWriteMode");
	set_protocolRef= putName("set_protocol");
	singleRef = putName("single");
	sinkRef = putName("sink");
	sliceRef = putName("slice");
	sortRef = putName("sort");
	sqlcatalogRef = putName("sqlcatalog");
	sqlRef = putName("sql");
	startRef = putName("start");
	starttraceRef = putName("starttrace");
	stoptraceRef = putName("stoptrace");
	streamsRef = putName("streams");
	strRef = putName("str");
	subavgRef = putName("subavg");
	subcountRef = putName("subcount");
	subdeltaRef = putName("subdelta");
	subeval_aggrRef = putName("subeval_aggr");
	subgroupdoneRef= putName("subgroupdone");
	subgroupRef = putName("subgroup");
	subinterRef = putName("subinterRef");
	submaxRef = putName("submax");
	submedianRef = putName("submedian");
	subminRef = putName("submin");
	subprodRef = putName("subprod");
	subsliceRef = putName("subslice");
	subsumRef = putName("subsum");
	subuniformRef= putName("subuniform");
	sumRef = putName("sum");
	takeRef= putName("take");
	thetajoinRef = putName("thetajoin");
	thetaselectRef = putName("thetaselect");
	tidRef = putName("tid");
	timestampRef = putName("timestamp");
	transaction_abortRef= putName("transaction_abort");
	transaction_beginRef= putName("transaction_begin");
	transaction_commitRef= putName("transaction_commit");
	transactionRef= putName("transaction");
	transaction_releaseRef= putName("transaction_release");
	transaction_rollbackRef= putName("transaction_rollback");
	umaskRef = putName("umask");
	uniqueRef= putName("unique");
	unlockRef= putName("unlock");
	unpackRef = putName("unpack");
	unpinRef = putName("unpin");
	updateRef = putName("update");
	userRef = putName("user");
	vectorRef = putName("vector");
	window_boundRef = putName("window_bound");
	wlcRef = putName("wlc");
	wlrRef = putName("wlr");
	zero_or_oneRef = putName("zero_or_one");
/* ! please keep this list sorted for easier maintenance ! */
}
