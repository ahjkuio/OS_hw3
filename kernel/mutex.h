#ifndef _MUTEX_H_
#define _MUTEX_H_

struct mutex {
  struct spinlock splock; // Защита структуры мьютекса
  struct sleeplock slock; // Для блокировки/разблокировки
  int nlink;              // Счетчик ссылок (сколько процессов используют мьютекс)
  int locked;             // Флаг, показывающий, заблокирован ли мьютекс
};

extern struct mutex mutex[NMUTEX];

void mutexinit(void);
// Системные вызовы
uint64 sys_cmutex(void);
uint64 sys_rmutex(void);
uint64 sys_lock(void);
uint64 sys_unlock(void);
int rmutex(int);


#endif