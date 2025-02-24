#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "defs.h"
#include "mutex.h"

// глобальная таблица мьютексов
struct mutex mutex[NMUTEX];

// инициализация таблицы мьютексов при старте системы
void
mutexinit(void)
{
  for (int i = 0; i < NMUTEX; i++) {
    initlock(&mutex[i].splock, "mutex");
    initsleeplock(&mutex[i].slock, "mutex");
    mutex[i].nlink = 0;
    mutex[i].locked = 0;
  }
}

// внутренняя функция ядра для освобождения мьютекса
int
rmutex(int fd)
{
  struct proc *p = myproc();
  struct mutex *m;

  if(fd < 0 || fd >= NMUTEXPROC || (m = p->mutex[fd]) == 0)
    return -1;

  acquire(&m->splock);
  p->mutex[fd] = 0;
  m->nlink--;
  if(m->nlink == 0)
    m->locked = 0;
  release(&m->splock);

  return 0;
}

//вызов создания мьютекса
uint64
sys_cmutex(void)
{
  struct proc *p = myproc();
  int i;

  acquire(&p->lock);
  // поиск свободного дескриптора мьютекса в процессе
  for (i = 0; i < NMUTEXPROC; i++) {
    if (p->mutex[i] == 0)
      break;
  }
  if (i == NMUTEXPROC) {
    release(&p->lock);
    return -1; // нет свободных дескрипторов
  }

  int j;
  // поиск свободного мьютекса в глобальной таблице
  for (j = 0; j < NMUTEX; j++) {
    acquire(&mutex[j].splock);
    if (mutex[j].nlink == 0)
      break;
    release(&mutex[j].splock);
  }
  if (j == NMUTEX) {
    release(&p->lock);
    return -1; // нет свободных мьютексов в глобальной таблице
  }
  
  p->mutex[i] = &mutex[j];
  p->mutex[i]->nlink = 1;
  release(&mutex[j].splock);
  release(&p->lock);

  return i; // возвращаем индекс в таблице дескрипторов процесса
}

//вызов освобождения мьютекса
uint64
sys_rmutex(void)
{
  int fd;
  argint(0, &fd);
  
  struct proc *p = myproc();
  struct mutex *m;

  acquire(&p->lock);
  if (fd < 0 || fd >= NMUTEXPROC || (m = p->mutex[fd]) == 0) {
    release(&p->lock);
    return -1;
  }

  acquire(&m->splock);
  p->mutex[fd] = 0;
  m->nlink--;
  if (m->nlink == 0) {
    m->locked = 0;
  }
  release(&m->splock);
  release(&p->lock);
  return 0;
}

//вызов блокировки мьютекса
uint64
sys_lock(void)
{
  int fd;
  argint(0, &fd); 

  struct proc *p = myproc();
  struct mutex *m;

  acquire(&p->lock);
  if (fd < 0 || fd >= NMUTEXPROC || (m = p->mutex[fd]) == 0) {
    release(&p->lock);
    return -1;
  }
  release(&p->lock);

  acquiresleep(&m->slock);
  return 0;
}

//вызов разблокировки мьютекса
uint64
sys_unlock(void)
{
  int fd;
  argint(0, &fd);

  struct proc *p = myproc();
  struct mutex *m;

  acquire(&p->lock);
  if (fd < 0 || fd >= NMUTEXPROC || (m = p->mutex[fd]) == 0) {
    release(&p->lock);
    return -1;
  }
  releasesleep(&m->slock);
  release(&p->lock);
  return 0;
}
