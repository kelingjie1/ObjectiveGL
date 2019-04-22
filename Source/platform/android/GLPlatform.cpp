//
// Created by willisdai on 2019/3/21.
//

#include "../../GLPlatform.h"

std::shared_ptr<ObjectiveGL::GLShareGroup> ObjectiveGL::GLPlatform::createShareGroup() {
    return std::shared_ptr<ObjectiveGL::GLShareGroup>();
}

void *ObjectiveGL::GLPlatform::createContext(ObjectiveGL::GLShareGroup *shareGroup) {
    return nullptr;
}

void ObjectiveGL::GLPlatform::setContext(void *context) {

}

void ObjectiveGL::GLPlatform::releaseContext(void *context) {

}
