/* getitem.c - getfirst, getlast, getitem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getfirst  -  Remove a process from the front of a queue
 *------------------------------------------------------------------------
 */
pid32	getfirst(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32	head;
//	struct qentry *head;
	
	
	if (isempty(q)) {
		return EMPTY;
	}

//	head = queuehead(q);
	
	head= queuehead(q);

	return getitem((queuetab[head].qnext)->pid);
//	return getitem((head->qnext)->pid);

}

/*------------------------------------------------------------------------
 *  getlast  -  Remove a process from end of queue
 *------------------------------------------------------------------------
 */
pid32	getlast(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove a process (assumed	*/
					/*   valid with no check)	*/
{
	pid32 tail;

	if (isempty(q)) {
		return EMPTY;
	}

	tail = queuetail(q);
	return getitem((queuetab[tail].qprev)->pid);
}

/*------------------------------------------------------------------------
 *  getitem  -  Remove a process from an arbitrary point in a queue
 *------------------------------------------------------------------------
 */
pid32	getitem(
	  pid32		pid		/* ID of process to remove	*/
//struct qentry *nextpid

	)
{
//	pid32	prev, next;

	 struct qentry *next;
	 struct qentry *prev;


//	next = nextpid.qnext;	/* Following node in list	*/
//	prev = nextpid.qprev;	/* Previous node in list	*/

//	next =queuetab[pid].qnext;
//	prev=queuetab[pid].qprev;

	next = &(queuetab[pid].qnext);
	prev = &(queuetab[pid].qprev);

//	prev.qnext = next;
//	next.qprev = prev;

	prev->qnext=next;
	next->qprev=prev;

	return pid;
}
