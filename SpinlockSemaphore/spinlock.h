#ifndef _SPINLOCK_H

void spin_lock(volatile char *lock);
void spin_unlock(volatile char *lock);

#define __SPIN_H
#endif //_SPINLOCK_H
