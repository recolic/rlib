#include "../c-with-class.h"
#include <stdio.h>

RCPP_CLASS_DECL(vector)
RCPP_CLASS_METHOD_DECL_1(vector, push_back, void, int)
RCPP_CLASS_METHOD_DECL_1(vector, at, int, int)

RCPP_CLASS_BEGIN(vector)
RCPP_CLASS_METHOD_DECL_2(vector, push_back)
RCPP_CLASS_METHOD_DECL_2(vector, at)
RCPP_CLASS_END()

RCPP_CLASS_METHOD_IMPL(vector, push_back, void, int data) {
    printf("pushing back %d\n", data);
}
RCPP_CLASS_METHOD_IMPL(vector, at, int, int index) {
    int element = index * index;
    return element;
}

RCPP_CLASS_CONSTRUCTOR_IMPL(vector) {
    RCPP_CLASS_METHOD_REGISTER(vector, push_back)
    RCPP_CLASS_METHOD_REGISTER(vector, at)
    printf("constructor called\n");
}
RCPP_CLASS_DESTRUCTOR_IMPL(vector) {
    printf("destructor called\n");
}
int main(){
    RCPP_NEW(vector, vct, NULL);
    RCPP_CALL(vct, push_back, 333);
    vct.push_back(&vct, 666);

    printf("Element at index %d is %d.\n", 5, vct.at(&vct, 5));
    return 123;
}