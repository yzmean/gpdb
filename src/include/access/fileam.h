/*-------------------------------------------------------------------------
*
* fileam.h
*	  external file access method definitions.
*
* Portions Copyright (c) 2007-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present Pivotal Software, Inc.
 *
 *
 * IDENTIFICATION
 *	    src/include/access/fileam.h
*
*-------------------------------------------------------------------------
*/
#ifndef FILEAM_H
#define FILEAM_H

#include "access/formatter.h"
#include "access/relscan.h"
#include "access/sdir.h"
#include "access/url.h"
#include "utils/rel.h"

/*
 * ExternalInsertDescData is used for storing state related
 * to inserting data into a writable external table.
 */
typedef struct ExternalInsertDescData
{
	Relation	ext_rel;
	URL_FILE   *ext_file;
	char	   *ext_uri;		/* "command:<cmd>" or "tablespace:<path>" */
	bool		ext_noop;		/* no op. this segdb needs to do nothing (e.g.
								 * mirror seg) */

	TupleDesc	ext_tupDesc;
	Datum	   *ext_values;
	bool	   *ext_nulls;

	FmgrInfo   *ext_custom_formatter_func; /* function to convert to custom format */
	List	   *ext_custom_formatter_params; /* list of defelems that hold user's format parameters */

	FormatterData *ext_formatter_data;

	struct CopyStateData *ext_pstate;	/* data parser control chars and state */

} ExternalInsertDescData;

typedef ExternalInsertDescData *ExternalInsertDesc;

typedef enum DataLineStatus
{
	LINE_OK,
	LINE_ERROR,
	NEED_MORE_DATA,
	END_MARKER
} DataLineStatus;

extern FileScanDesc external_beginscan(Relation relation,
				   uint32 scancounter, List *uriList,
				   char *fmtOptString, char fmtType, bool isMasterOnly,
				   int rejLimit, bool rejLimitInRows,
				   bool logErrors, int encoding);
extern void external_rescan(FileScanDesc scan);
extern void external_endscan(FileScanDesc scan);
extern void external_stopscan(FileScanDesc scan);
extern HeapTuple external_getnext(FileScanDesc scan, ScanDirection direction, List* filter_quals);
extern ExternalInsertDesc external_insert_init(Relation rel);
extern Oid	external_insert(ExternalInsertDesc extInsertDesc, HeapTuple instup);
extern void external_insert_finish(ExternalInsertDesc extInsertDesc);
extern void external_set_env_vars(extvar_t *extvar, char *uri, bool csv, char *escape, char *quote, bool header, uint32 scancounter);
extern char *linenumber_atoi(char buffer[20], int64 linenumber);

/* prototypes for functions in url_execute.c */
extern int popen_with_stderr(int *rwepipe, const char *exe, bool forwrite);
extern int pclose_with_stderr(int pid, int *rwepipe, StringInfo sinfo);
extern char *make_command(const char *cmd, extvar_t *ev);

#endif   /* FILEAM_H */
