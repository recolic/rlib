#ifndef RLIB_CWITHCLASS_H_
#define RLIB_CWITHCLASS_H_

#error c_with_class not completed yet

#define RCPP_NEW(type,name,constructor_arg) struct type name __attribute__((cleanup(type##_destructor)));type##_constructor(&name,constructor_arg)
#define RCPP_CALL(i_objectname,i_funcname, ...) i_objectname.i_funcname(&i_objectname, ##__VA_ARGS__) //ONLY static public function can be called directly!!!
#define RCPP_PCALL(p_objectname,i_funcname, ...) p_objectname->i_funcname(p_objectname, ##__VA_ARGS__)

#define RCPP_CLASS_DECL(class_name) struct class_name;
#define RCPP_CLASS_METHOD_DECL_1(class_name, method_name, return_type, ...) typedef return_type (* method_name##_rcpp_t)(struct class_name *this, ##__VA_ARGS__); //VAARGS is `int arg1, float arg2, ...`
#define RCPP_CLASS_BEGIN(class_name) struct class_name {
#define RCPP_CLASS_MATHOD_DECL_2(method_name) RCPP_MEMBER_DECL(method_name##_rcpp_t, method_name)
#define RCPP_CLASS_MEMBER_DECL(type, name) type name;
#define RCPP_CLASS_END() };
#define RCPP_CLASS_METHOD_IMPL(class_name, method_name, return_type, ...) return_type method_name(struct class_name *this, ##__VA_ARGS__) //VAARGS is `int arg1, float arg2, ...`
#define RCPP_CLASS_CONSTRUCTOR_IMPL(class_name) void class_name##_constructor(struct class_name *this, void *arg) //TODO: Register all methods.
#define RCPP_CLASS_DESTRUCTOR_IMPL(class_name) void class_name##_destructor(struct class_name *this)

#endif
