#include <inc/types.h>
#include <kern/alloc.h>
#include <inc/assert.h>
#include <kern/spinlock.h>

#define SPACE_SIZE 5 * 0x1000

//Кусок пустого места под динамическую память и в конце заголовок пустого блока.
static uint8_t space[SPACE_SIZE];
static Header base = {.s = {.next = (Header *)space, .prev = (Header *)space, .size = 0}}; /* empty list to get started */

static Header *freep = NULL; /* start of free list */


//проверяет правильность построения списка. Каждый следующий блок существует и указывает на предыдущий.
static void
check_list(void) {
  Header *p, *prevp;
  __asm __volatile("cli;");
  prevp = freep;
  for (p = prevp->s.next; p != freep; p = p->s.next) {
    if (prevp != p->s.prev) {
      panic("Corrupted list.\n");
      for (;;) {
      }
    }
    prevp = p;
  }
  __asm __volatile("sti;");
}

/* malloc: general-purpose storage allocator */
void *
test_alloc(uint8_t nbytes) {
  Header *p;
  unsigned nunits;

  // Make allocator thread-safe with the help of spin_lock/spin_unlock.
  // LAB 5: Your code here.
  // ............................................................freep\/
  // [hdr.]-------------------------------------------------[base]||
  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1; //подгоняем под целое число headerов, чтобы все заголовки были выровнены;
  
  spin_lock(&kernel_lock);//Блокируем 
  //если еще нет списка свободных блоков, создаем свободный блок размером во всю память(его заголовок вначале)
  //Его указатели указывают на последний служебный блок. Список закольцевался. Получился двусвязный. Кольцевой. Список.
  if (freep == NULL) { /* no free list yet */
    ((Header *)&space)->s.next = (Header *)&base;
    ((Header *)&space)->s.prev = (Header *)&base;
    ((Header *)&space)->s.size = (SPACE_SIZE - sizeof(Header)) / sizeof(Header);
    freep                      = &base;
  }

  check_list();

  for (p = freep->s.next;; p = p->s.next) {
    if (p->s.size >= nunits) { /* big enough */
      freep = p->s.prev;
      if (p->s.size == nunits) { /* exactly */
        (p->s.prev)->s.next = p->s.next;//выключаем блок из списка.
        (p->s.next)->s.prev = p->s.prev;
      } else { /* allocate tail end. */
        p->s.size -= nunits;//откусываем память от блока сзади
        p += p->s.size;// переходим на новый блок
        p->s.size = nunits;//не ставим ему указатели, потому он не должен быть в списке
      }
      spin_unlock(&kernel_lock);
      return (void *)(p + 1);
    }
    if (p == freep) { /* wrapped around free list */
      spin_unlock(&kernel_lock);
      return NULL;
    }
  }
}

/* free: put block ap in free list */
void
test_free(void *ap) {
  Header *bp, *p;
  bp = (Header *)ap - 1; /* point to block header который хотим удалить */

  // Make allocator thread-safe with the help of spin_lock/spin_unlock.
  // LAB 5: Your code here.
  spin_lock(&kernel_lock);
  // Хотим чтобы блоки в списке были в порядке расположения в памяти. Ищем два свободных блока слева и справа
  for (p = freep; !(bp > p && bp < p->s.next); p = p->s.next)
    if (p >= p->s.next && (bp > p || bp < p->s.next))
      break;                                                 /* freed block at start or end of arena */
  if (bp + bp->s.size == p->s.next && p + p->s.size == bp) { /* join to both */
    p->s.size += bp->s.size + p->s.next->s.size;
    p->s.next->s.next->s.prev = p;
    p->s.next                 = p->s.next->s.next;
  } else if (bp + bp->s.size == p->s.next) { /* join to upper nbr */
    bp->s.size += p->s.next->s.size;
    bp->s.next                = p->s.next->s.next;
    bp->s.prev                = p->s.next->s.prev;
    p->s.next->s.next->s.prev = bp;
    p->s.next                 = bp;
  } else if (p + p->s.size == bp) { /* join to lower nbr */
    p->s.size += bp->s.size;
  } else {
    bp->s.next        = p->s.next;
    bp->s.prev        = p;
    p->s.next->s.prev = bp;
    p->s.next         = bp;
  }
  freep = p;

  check_list();
  spin_unlock(&kernel_lock);
}
