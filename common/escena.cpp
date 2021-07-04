#include "muro.hpp"
#include "escena.hpp"
MuroNode::MuroNode(Muro *newMuro, MuroNode *prevNode){
    muro = newMuro;
    sig = nullptr;
    if(prevNode!=nullptr){
        prevNode->sig = this;
    }
}

void MuroList::InitList(Muro *muro){
    if(head== nullptr){
        head = new MuroNode(muro, nullptr);
        muro;
        last = head;
    }
}
void MuroList::AddMuro(Muro *muro){
    if(last == nullptr){
        InitList(muro);
        return;
    }
    last->sig = new MuroNode(muro, last);
    last = last->sig;
}
void Escena::AddMuro(Muro *muro){
    if(muroList.head == nullptr){
        muroList.InitList(muro);
    }
    else{
        muroList.AddMuro(muro);
    }
}
void Escena::Draw(GLuint programID){
    MuroNode *currentNode = muroList.head;
    while(currentNode!=nullptr){
        currentNode->muro->Draw(programID);
        currentNode = currentNode->sig;
    }
}
void Escena::UnLoad(){
    MuroNode *currentNode = muroList.head;
    MuroNode *aux;
    while(currentNode != nullptr){
        aux = currentNode->sig;
        currentNode->muro->Destroy();
        free(currentNode->muro);
        free(currentNode);
        currentNode = aux;
    }

}