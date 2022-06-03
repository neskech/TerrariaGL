#pragma once
#include "pch.h"

template <typename T>
struct Node{
    Node<T>* after = nullptr;
    Node<T>* before = nullptr;
    T* value = nullptr;

    Node(Node<T>* after_, Node<T>* before_, T* value_)
    : after(after_), before(before_), value(value_)
    {}

    Node(T* value_): value(value_) {}

    ~Node(){
        delete value;
    }
};

template <typename T>
class LinkedList{
    public:
        LinkedList(){
            Node<T>* node = new Node<T>();
            front = node;
            end = node;
            length = 1;
        }

        ~LinkedList(){
            Node<T>* curr = front;
            while (curr != nullptr){
                Node<T>* next = curr->after;
                delete curr;
                curr = next;
            }
        }

        template<typename... Args>
        T& emplaceBack(Args&&... args){
            if (end == nullptr && front == nullptr){
                end = new Node<T>();
                front = end;
            }

            else{
                Node<T>* newNode = new Node<T>();

                end->after = newNode;
                newNode->before = end;
                end = newNode;
            }

            new (end->value) T(std::forward<Args>(args)...);
            length++;
            return *(end->value)
        }

        template<typename... Args>
        T& emplaceFront(Args&&... args){
            if (front == nullptr && end == nullptr){
                front = new Node<T>();
                end = front;
            }

            else{
                Node<T>* newNode = new Node<T>();

                front->before = newNode;
                newNode->after = front;
                front = newNode;
            }

            new (front->value) T(std::forward<Args>(args)...);
            length++;
            return *(front->value);

        }

         T& addFront(T* data){
            if (front == nullptr && end == nullptr){
                front = new Node<T>(data);
                end = front;
            }

            else{
                Node<T>* newNode = new Node<T>();

                front->before = newNode;
                newNode->after = front;
                front = newNode;
            }
            length++;
            return *(front->value);

        }

        T& addBackk(T* data){
            if (end == nullptr && front == nullptr){
                end = new Node<T>(data);
                front = end;
            }

            else{
                Node<T>* newNode = new Node<T>();

                end->after = newNode;
                newNode->before = end;
                end = newNode;
            }

            length++;
            return *(end->value)
        }

        Scoped<Node<T>> popOffEnd(){
            Node<T>* before = end->before;
            before->after = nullptr;
            end->before = nullptr;

            Node<T>* toBeReturned = end;
            end = before;
            length--;
            return Scoped(toBeReturned);
            
        }

        Scoped<Node<T>> popOffFront(){
            Node<T>* after = front->after;
            after->before = nullptr;
            front->after = nullptr;

            Node<T>* toBeReturned = front;
            front = after;
            length--;
            return Scoped(toBeReturned);
        }

        inline T& getFront(){ return *front; }
        inline T& getEnd(){ return *end; }

        Node<T>* getNode(int index){
            Node<T>* curr = front;
            for (int i = 0; i < index; i++)
                curr = curr->after;
            return curr;
        }

        Node<T>* getNodeStartingFrom(Node<T>* node, int nodeIndex, int desiredIndex){
             for (int i = 0; i < desiredIndex - nodeIndex; i++)
                node = node->after;
             return node;
        }     
    private:
        Node<T>* front;
        Node<T>* end;
        int length;

};