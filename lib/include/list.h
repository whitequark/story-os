#ifndef _LIST_H_
#define _LIST_H_

#include <story.h>

template<class T> class List
{
public:
List<T> *next;
List<T> *prev;
T item;

List(T _item) : item(_item), next(NULL), prev(NULL)
 {
 }
~List()
 {
 remove();
 }
void remove()
 {
 if(this->next)
  this->next->prev = this->prev;
 if(this->prev)
  this->prev->next = this->next;
 this->next = NULL;
 this->prev = NULL;
 }
void add_after(List<T>* elem)
 {
 elem->next = this->next;
 elem->prev = this;
 if(this->next)
  this->next->prev = elem;
 this->next = elem;
 }
void add_before(List<T>* elem)
 {
 elem->prev = this->prev;
 elem->next = this;
 if(this->prev)
  this->prev->next = elem;
 this->prev = elem;
 }
void add_tail(List<T>* elem)
 {
 tail()->add_after(elem);
 }
void add_head(List<T>* elem)
 {
 head()->add_before(elem);
 }
List<T>* head()
 {
 List<T> *elem, *next;
 for(elem = this; elem; next = elem, elem = elem->prev);
 return next;
 }
List<T>* tail()
 {
 List<T>* elem;
 for(elem = this; elem->next; elem = elem->next);
 return elem;
 }
unsigned int length()
 {
 unsigned int i = 0;
 List<T>* elem;
 for(elem = head(); elem; elem = elem->next, i++);
 return i;
 }
#define iterate_list(i, list) \
   for(i = list; i; i = i->next)
#define iterate_list_backward(i, list) \
   for(i = list; i; i = i->prev)
#define iterate_list_safe(i, list, n) \
   for(i = list->head(); i; i = n, n = i->next)
};

#endif
