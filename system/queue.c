/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */

//struct qentry* getNewNode(pid32 pid,qid q)


pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	
//	qid16	tail, prev;		/* Tail & previous node indexes	*/

	struct qentry *tail;
	struct qentry *prev;

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}
	tail =&queuetab[queuetail(q)];
//	tail = queuetail(q);
//	prev = queuetab[tail].qprev;
//	prev = (queuetab[tail].qprev)->pid;
	prev = queuetab[tail->pid].qprev;
	

//	queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
//	(queuetab[pid].qnext)->pid  = tail;
	queuetab[tail->pid].qprev=&queuetab[pid];
	
//	queuetab[pid].qprev  = prev;
//	(queuetab[pid].qprev)->pid=prev;
	queuetab[pid].qnext=&queuetab[tail->pid];

//	queuetab[prev].qnext = pid;
//	(queuetab[prev].qnext)->pid =pid;

//	queuetab[tail].qprev = pid;
//	(queuetab[tail].qprev)->pid =pid;
	queuetab[pid].qprev=prev;
	prev->qnext=&queuetab[pid];
		
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirst(q);
	queuetab[pid].qprev = NULL;
//	(queuetab[pid].qprev)->pid = EMPTY;

	queuetab[pid].qnext = NULL;
//	(queuetab[pid].qnext)->pid = EMPTY;
	
	return pid;
}
