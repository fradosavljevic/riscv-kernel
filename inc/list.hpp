#ifndef LIST_H
#define LIST_H
#include "MemoryAllocator.hpp"

template<typename T>
class List
{
private:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}

        void* operator new(size_t size) {
            return MemoryAllocator::mallocWrapper(size);
        }
        void* operator new[](size_t size) {
            return MemoryAllocator::mallocWrapper(size);
        }

        void operator delete(void *ptr) {
            MemoryAllocator::freeWrapper(ptr);
        }
        void operator delete[](void *ptr) {
            MemoryAllocator::freeWrapper(ptr);
        }
    };

    Elem *head, *tail;

public:
    List() : head(0), tail(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    class Iterator
    {
    private:
        Elem *curr;

    public:
        Iterator(Elem *curr) : curr(curr) {}

        bool operator!=(const Iterator &other) const {
            return curr != other.curr;
        }

        Iterator &operator++() {
            curr = curr->next;
            return *this;
        }

        T *operator*() const {
            return curr->data;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    void addFirst(T *data)
    {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        Elem *elem = new Elem(data, 0);
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }

    void addSorted(T* data) {
        time_t remaining = data->getSleepTime();

        Elem* prev = nullptr;
        Elem* curr = head;

        while (curr && remaining > curr->data->getRelativeSleepTime()) {
            remaining -= curr->data->getRelativeSleepTime();
            prev = curr;
            curr = curr->next;
        }

        data->setRelativeSleepTime(remaining);

        if (curr) curr->data->setRelativeSleepTime(curr->data->getRelativeSleepTime() - remaining);

        Elem* elem = new Elem(data, curr);

        if (prev) prev->next = elem;
        else head = elem;

        if (!curr) tail = elem;
        else if (!elem->next) tail = elem;
    }
};

#endif
