#include "muro.hpp"
#ifndef ESCENA_H
#define ESCENA_H
class MuroNode{
    public:
        Muro *muro;
        MuroNode *sig = nullptr;
        MuroNode(Muro *newMuro, MuroNode *prevNode);
};

class MuroList{
    public:
        MuroNode *head = nullptr;
        MuroNode *last = nullptr;
        void InitList(Muro *muro);
        void AddMuro(Muro *muro);
};

class Escena{
    public:
        Escena(){

        }
        MuroList muroList;
        void AddMuro(Muro *muro);
        void Draw(GLuint programID);
        void UnLoad();
};
#endif