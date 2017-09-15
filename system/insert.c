/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{

	struct qentry* curr;
	struct qentry* prev;

	/* Runs through items in a queue*/
	/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = &queuetab[firstid(q)];

	while (curr!=NULL&& curr->qkey>=key) {
		curr = queuetab[curr->pid].qnext;
	}



	/* Insert process between curr node and previous node */

	prev = queuetab[curr->pid].qprev;



	(queuetab[pid].qnext)=curr;
	(queuetab[pid].qprev)=prev;
	(queuetab[pid].qkey)=key;
	(queuetab[pid].pid)=pid;


	queuetab[prev->pid].qnext =&queuetab[pid];
	queuetab[curr->pid].qprev =&queuetab[pid];
	return OK;
}
