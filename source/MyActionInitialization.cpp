#include "MyActionInitialization.h"
#include "MyPrimaryGenerator.h"

void MyActionInitialization::Build() const {
    SetUserAction( new MyPrimaryGenerator );


    MyRunAction *runAction = new MyRunAction();
    SetUserAction(runAction);

};
