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
//	int16	curr;			/* Runs through items in a queue*/
//	int16	prev;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = &queuetab[firstid(q)];
//	curr = firstid(q);
//	while (queuetab[curr]->qkey >=key){
//		curr=queuetab[curr]->qnext;
//	}
	while (curr!=NULL&& curr->qkey>=key) {
		curr = queuetab[curr->pid].qnext;
	}

//	queuetab[pid].pid=pid;

	/* Insert process between curr node and previous node */

	prev = queuetab[curr->pid].qprev;

//	prev = queuetab[curr].qprev;	/* Get index of previous node	*/

	(queuetab[pid].qnext)=curr;
	(queuetab[pid].qprev)=prev;
	(queuetab[pid].qkey)=key;
	(queuetab[pid].pid)=pid;

//	queuetab[pid]= key;
//	(queuetab[pid].qnext)->pid=pid;
//	(queuetab[pid].qprev)->pid=pid;
		

//	queuetab[pid].qnext = curr;
//	queuetab[pid].qprev = prev;
//	queuetab[pid].qkey = key;
	queuetab[prev->pid].qnext =&queuetab[pid];
	queuetab[curr->pid].qprev =&queuetab[pid];
	return OK;
}
