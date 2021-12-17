#include <jni.h>
#include "inc/Log.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>

#define HELLOLIBS_JNI_CLASS_NAME "com/hyh/jnifunctions/jni/JNIManager"

typedef struct tick_context{
    JavaVM *javaVM;
    jclass jniManagerClz;
    jobject jniManagerObj;
    pthread_mutex_t lock;
    int done;
}TickContext;
TickContext g_tickctx;

typedef struct monitor_context{
    JavaVM *javaVM;
    jclass jniManagerClz;
    jobject jniManagerObj;
}MonitorContext;
MonitorContext g_monitorctx;

void *UpdateTicks(void *context) {
    FUNCTION_ENTER;

    TickContext *pctx = (TickContext *) context;
    JavaVM *javaVM = pctx->javaVM;
    JNIEnv *env;
    //(1)因为UpdateTicks属于子线程，子线程并没有绑定到javaVM，所以res返回一定是error
    jint ret = javaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    LOGD_HYH;
    if (JNI_OK != ret) {
        LOGD_HYH;
        //(2)绑定子线程到虚拟机实例，即初始化env
        ret = javaVM->AttachCurrentThread(&env, NULL);
        if (JNI_OK != ret) {
            LOGE("AttachCurrentThread error,ret = %d", ret);
            return NULL;
        }
    }
    struct timeval beginTime, curTime, usedTime, leftTime;
    const struct timeval kOneSecond = {
            (__kernel_time_t) 1,
            (__kernel_suseconds_t) 0
    };
    jmethodID timerId = env->GetMethodID(pctx->jniManagerClz, "updateTimer", "()V");
    while (1) {
        gettimeofday(&beginTime, NULL);

        pthread_mutex_lock(&pctx->lock);
        int done = pctx->done;
        if (pctx->done) {
            pctx->done = 0;
        }
        pthread_mutex_unlock(&pctx->lock);
        if (done) {
            break;
        }

        env->CallVoidMethod(pctx->jniManagerObj, timerId);

        gettimeofday(&curTime, NULL);
        timersub(&curTime, &beginTime, &usedTime);
        timersub(&kOneSecond, &usedTime, &leftTime);
        struct timespec sleepTime;
        sleepTime.tv_sec = leftTime.tv_sec;
        sleepTime.tv_nsec = leftTime.tv_usec * 1000;

        if (sleepTime.tv_sec <= 1) {
            nanosleep(&sleepTime, NULL);
        } else {
            LOGE("Process too long!");
            break;
        }
    }
    //(3)使得子线程脱离虚拟机实例
    javaVM->DetachCurrentThread();

    FUNCTION_QUIT;
    return context;
}


JNIEXPORT void JNICALL Native_StartTicks(JNIEnv *env, jobject obj) {
    FUNCTION_ENTER;

    pthread_t threadInfo_;
    pthread_attr_t threadAttr_;
    pthread_attr_init(&threadAttr_);
    pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&g_tickctx.lock, NULL);

    jclass clz = env->GetObjectClass(obj);
    g_tickctx.jniManagerClz = static_cast<jclass>(env->NewGlobalRef(clz));          //static_cast<jclass>:jobject-->jclass
    g_tickctx.jniManagerObj = env->NewGlobalRef(obj);

    int res = pthread_create(&threadInfo_, &threadAttr_, UpdateTicks, &g_tickctx);
    assert(res == 0);
    pthread_attr_destroy(&threadAttr_);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_StopTicks(JNIEnv *env, jobject obj) {
    FUNCTION_ENTER;

    pthread_mutex_lock(&g_tickctx.lock);
    g_tickctx.done = 1;
    pthread_mutex_unlock(&g_tickctx.lock);

    env->DeleteGlobalRef(g_tickctx.jniManagerClz);
    env->DeleteGlobalRef(g_tickctx.jniManagerObj);
    g_tickctx.jniManagerClz=NULL;
    g_tickctx.jniManagerObj=NULL;
    pthread_mutex_destroy(&g_tickctx.lock);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_StringOperations(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)Unicode String Operations
    jstring str = env->NewStringUTF("testhellolibs1");
    jint len = env->GetStringUTFLength(str);
    LOGD("len:%d",len);
    const jchar *strchar = env->GetStringChars(str, nullptr);
    for (jint i = 0; i < len; i++) {
        LOGD("strchar:%c", strchar[i]);
    }
    //GetStringChars/ReleaseStringChars
    env->ReleaseStringChars(str,strchar);
    //虽然函数结束后，LocalRef str会自动释放，还是要养成手动释放的习惯
    env->DeleteLocalRef(str);


    //(2)UTF String Operations
    jstring strutf = env->NewStringUTF("testhellolibs2");
    jint lenutf = env->GetStringUTFLength(strutf);
    LOGD("lenutf:%d",lenutf);
    const char *strcharutf = env->GetStringUTFChars(strutf, nullptr);
    for (int i = 0; i < lenutf; i++) {
        LOGD("strcharutf:%c", strcharutf[i]);
    }
    //GetStringUTFChars/ReleaseStringUTFChars
    env->ReleaseStringUTFChars(strutf,strcharutf);
    env->DeleteLocalRef(strutf);


    //(3)GetStringCritical
    jstring strcritical = env->NewStringUTF("testhellolibs3");
    jint lencritical = env->GetStringUTFLength(strcritical);
    LOGD("lencritical:%d",lencritical);
    const jchar* jcharscritical = env->GetStringCritical(strcritical,NULL);
    for(int i = 0;i < lencritical;i++){
        LOGD("jcharscritical:%c", jcharscritical[i]);
    }
    //GetStringCritical/ReleaseStringCritical
    env->ReleaseStringCritical(strcritical,jcharscritical);
    env->DeleteLocalRef(strcritical);

    //(4)GetStringUTFRegion
    jstring strregion = env->NewStringUTF("testhellolibs4");
    jint lenregion = env->GetStringUTFLength(strregion);
    LOGD("lenregion:%d",lenregion);
    char buff1[128];
    char* pBuff1 = buff1;
    env->GetStringUTFRegion(strregion,0,lenregion,pBuff1);
    for(int i=0;i < lenregion;i++){
        LOGD("buff1:%c",buff1[i]);
    }
    env->DeleteLocalRef(strregion);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_ArrayOperations(JNIEnv *env, jobject obj) {
    FUNCTION_ENTER;

    //(1)基本类型
    //(i)NewIntArray
    int len = 3;
    jintArray intArray = env->NewIntArray(len);
    if (NULL == intArray) {
        LOGD("NULL == ret");
        return ;
    }
    //(ii)GetArrayLength
    jint arrayLen = env->GetArrayLength(intArray);
    LOGD("arrayLen:%d", arrayLen);
    //(iii)GetIntArrayRegion/SetIntArrayRegion
    int tmp[3]={7,8,9};
    env->SetIntArrayRegion(intArray,0,arrayLen,tmp);
    int array[3];
    env->GetIntArrayRegion(intArray,0,arrayLen,array);
    for(int i=0;i<arrayLen;i++){
        LOGD("tmp1:%d ",array[i]);
    }
    //(iv)GetIntArrayElements/ReleaseIntArrayElements
    jboolean isCopy;
    jint *c_array = env->GetIntArrayElements(intArray,&isCopy);
    LOGD("isCopy:%d ",isCopy);
    if(c_array == nullptr){
        LOGE("GetIntArrayElements error!!!");
        return ;
    }
    for(int i = 0;i<arrayLen;i++){
        LOGD("get:%d ",c_array[i]);
    }
    env->ReleaseIntArrayElements(intArray,c_array,0);
    env->DeleteLocalRef(intArray);


    //(2)Object类型
    int lenObject = 5;
    jclass stringArrCls = env->FindClass("java/lang/String");
    if (NULL == stringArrCls) {
        LOGD("NULL == stringArrCls");
        return ;
    }
    //(i)NewObjectArray
    jobjectArray objectArray = env->NewObjectArray(lenObject, stringArrCls, 0);      //0:jobject initialElement
    if (NULL == objectArray) {
        LOGD("NULL == objectArray");
        return ;
    }
    //(ii)GetArrayLength
    jint arrayLenObject = env->GetArrayLength(objectArray);
    LOGD("arrayLenObject:%d", arrayLenObject);
    //(iii)GetObjectArrayElement/SetObjectArrayElement
    char *sa[]={"Hello","Han","yu","hang","!!!"};
    for(int i=0;i<arrayLenObject;i++){
        jstring str = env->NewStringUTF(sa[i]);
        env->SetObjectArrayElement(objectArray,i,str);
        env->DeleteLocalRef(str);
    }
    for(int i=0;i<arrayLenObject;i++){
        jstring str = (jstring)env->GetObjectArrayElement(objectArray,i);
        jboolean isCopy;
        const char *chars = env->GetStringUTFChars(str, &isCopy);
        LOGD("isCopy:%d,chars:%s",isCopy,chars);
        env->ReleaseStringUTFChars(str,chars);
    }
    env->DeleteLocalRef(objectArray);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_ObjectOperations(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)GetObjectClass
    jclass clz = env->GetObjectClass(obj);

    //(2)IsInstanceOf
    jboolean bool_x = env->IsInstanceOf(obj,clz);
    LOGD("bool_x:%d",bool_x);
    jclass stringCls = env->FindClass("java/lang/String");
    if(NULL == stringCls){
        LOGE("NULL == stringCls");
        return ;
    }
    jmethodID methodId = env->GetMethodID(stringCls,"<init>","([C)V");
    jstring strUnicode= env->NewStringUTF("testhellolibs!");            //(i)create jstring
    jboolean isCopy = false;
    const jchar *chars = env->GetStringChars(strUnicode,&isCopy);      //(ii)jstring-->jchar*
    int len = env->GetStringLength(strUnicode);
    jcharArray charArray = env->NewCharArray(len);                      //(iii)create jcharArray
    env->SetCharArrayRegion(charArray,0,len,chars);                     //(iv)jchar*-->jcharArray

    //(3)NewObject
    jstring strUnicode1 = (jstring)env->NewObject(stringCls,methodId,charArray);    //jcharArray-->jstring
    const char *charUTF = env->GetStringUTFChars(strUnicode1,&isCopy);             //jstring-->char*
    LOGD("charUTF:%s",charUTF);

    env->DeleteLocalRef(clz);
    env->DeleteLocalRef(stringCls);
    env->DeleteLocalRef(charArray);
    env->DeleteLocalRef(strUnicode);
    env->DeleteLocalRef(strUnicode1);

    FUNCTION_QUIT;
}


JNIEXPORT void JNICALL Native_ClassAndInterfaceOperations(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)DefineClass

    //(2)FindClass
    jclass clazz = env->FindClass(HELLOLIBS_JNI_CLASS_NAME);
    jboolean bool_x = env->IsInstanceOf(obj,clazz);
    LOGD("bool_x:%d",bool_x);
    //(3)GetSuperclass
    jclass clazz_super = env->GetSuperclass(clazz);
    jboolean bool_y = env->IsInstanceOf(obj,clazz_super);
    LOGD("bool_y:%d",bool_y);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(clazz_super);

    FUNCTION_QUIT;
}

void JNI_ThrowNew(JNIEnv *env,const char *name,const char *msg){
    FUNCTION_ENTER;

    jclass cls = env->FindClass(name);
    if(NULL != cls){
        env->ThrowNew(cls,msg);
    }
    env->DeleteLocalRef(cls);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_Exceptions(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)FatalError
//    env->FatalError("FatalError!!!");             //will crash


    jclass jCls = env->GetObjectClass(obj);
    jmethodID jMid = env->GetStaticMethodID(jCls,"ExcepitonCallback","()V");        //C层获取Java层静态方法ExcepitonCallback
    if(NULL != jMid){
        env->CallStaticVoidMethod(jCls,jMid);
    }
    LOGD("JNI:ExcepitonCallback");
    //(2)ExceptionCheck
    if(env->ExceptionCheck()){
        LOGE("print exception stack information!!!");
        //(3)ExceptionDescribe
        env->ExceptionDescribe();
        //(4)ExceptionClear
        env->ExceptionClear();
        //(5)ThrowNew
        JNI_ThrowNew(env,"java/lang/Exception","JNI throw exception1!!!");
        //注意:退出前释放相关资源!
        //return ;
    }

    jMid = env->GetStaticMethodID(jCls,"NormalCallback","()V");
    if(NULL != jMid){
        env->CallStaticVoidMethod(jCls,jMid);
    }
    LOGD("JNI:NormalCallback");
    //(6)ExceptionOccurred
    jthrowable jExc = env->ExceptionOccurred();
    if(jExc){
        env->ExceptionDescribe();
        env->ExceptionClear();
        JNI_ThrowNew(env,"java/lang/Exception","JNI throw exception2!!!");
        //注意:退出前释放相关资源!
        //return ;
    }
    env->DeleteLocalRef(jCls);
    env->DeleteLocalRef(jExc);

    FUNCTION_QUIT;
}


static jclass g_strCls1;
static jclass g_strCls2;
JNIEXPORT void JNICALL Native_GlobalAndLocalReferences(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)全局引用
    jclass strCls = env->FindClass("java/lang/String");
    g_strCls1 = static_cast<jclass >(env->NewGlobalRef(strCls));

    //(2)全局弱引用
    g_strCls2 = static_cast<jclass >(env->NewWeakGlobalRef(strCls));

    //(3)局部引用
    jmethodID mid = env->GetMethodID(strCls,"<init>","([C)V");
    jstring strUnicode = env->NewStringUTF("hanyuhang");    //(i)create jstring
    const jchar *charUnicode = env->GetStringChars(strUnicode, nullptr);    //(ii)jstring-->jchar*
    jint len = env->GetStringUTFLength(strUnicode);
    LOGD("GetStringUTFLength:%d",len);
    jint len1 = env->GetStringLength(strUnicode);
    LOGD("GetStringLength:%d",len1);

    jcharArray charArr = env->NewCharArray(len);
    env->SetCharArrayRegion(charArr,0,len,charUnicode);                     //(iii)jchar*-->jcharArray
    jobject strObject = env->NewObject(strCls,mid,charArr);                 //(iv)jcharArray-->jobject(通过String构造函数)
    jobject strLocalref = env->NewLocalRef(strObject);                      //(v)NewLocalRef
    const char *charUtf = env->GetStringUTFChars(static_cast<jstring >(strLocalref),nullptr);     //(vi)jobject-->char*
    LOGD("charUtf:%s",charUtf);

    
    //GetStringUTFChars/ReleaseStringUTFChars
    env->ReleaseStringUTFChars(static_cast<jstring>(strLocalref),charUtf);
    //(1)删除全局引用
    env->DeleteGlobalRef(g_strCls1);
    //(2)删除全局弱引用
    env->DeleteWeakGlobalRef(g_strCls2);
    //(3)删除局部引用
    env->DeleteLocalRef(strCls);
    env->DeleteLocalRef(strUnicode);
    env->DeleteLocalRef(charArr);
    env->DeleteLocalRef(strObject);
    env->DeleteLocalRef(strLocalref);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_InstanceField(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //Jni局部变量命名习惯:j开头!!!
    jclass jCls = env->GetObjectClass(obj);
    jfieldID jWidthFid = env->GetFieldID(jCls,"mWidth","I");
    int width = env->GetIntField(obj,jWidthFid);
    LOGD("get width:%d",width);
    env->SetIntField(obj,jWidthFid,8);
    width = env->GetIntField(obj,jWidthFid);
    LOGD("set width:%d",width);
    //todo:GetObjectFiled

    FUNCTION_QUIT;
}

void Native_StaticField(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    jclass jCls = env->FindClass(HELLOLIBS_JNI_CLASS_NAME);
    if(jCls == nullptr){
        return ;
    }
    jfieldID jWidthFid = env->GetStaticFieldID(jCls,"mStaicWidth","I");
    if(jWidthFid == nullptr){
        return ;
    }
    jint jWidth = env->GetStaticIntField(jCls,jWidthFid);
    LOGD("get jNum:%d",jWidth);
    env->SetStaticIntField(jCls,jWidthFid,88);
    jWidth = env->GetStaticIntField(jCls,jWidthFid);
    LOGD("set jNum:%d",jWidth);
    env->DeleteLocalRef(jCls);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL Native_InstanceMethod(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    jclass jCls = env->GetObjectClass(obj);
    jmethodID jmid = env->GetMethodID(jCls,"InstanceMethodCallback","()V");
    env->CallVoidMethod(obj,jmid);

    FUNCTION_QUIT;
}

//todo:apk有偶现性crash,2019.01.28
void Native_StaticMethod(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    jclass jCls = env->FindClass(HELLOLIBS_JNI_CLASS_NAME);
    if(jCls == nullptr){
        return ;
    }
    jmethodID static_mid = env->GetStaticMethodID(jCls,"StaticMethodCallback","()V");
    if(static_mid == nullptr){
        return ;
    }
    env->CallStaticVoidMethod(jCls,static_mid);
    env->DeleteLocalRef(jCls);           //删除jCls局部引用

    FUNCTION_QUIT;
}


void native_JavaVMInterface(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    static JavaVM *pJavaVM = nullptr;
    env->GetJavaVM(&pJavaVM);
    LOGD("pJavaVM:%p",pJavaVM);

    FUNCTION_QUIT;
}

static int globle_monitor = 0;
void* monitorThread2(void *context){
    FUNCTION_ENTER;

    MonitorContext *pctx = (MonitorContext *) context;
    JavaVM *pJavaVM = pctx->javaVM;
    JNIEnv *env;
    //(1)因为monitorThread2属于子线程，子线程并没有绑定到javaVM，所以ret返回一定是error
    jint ret = pJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    LOGD_HYH;
    if (JNI_OK != ret) {
        LOGD_HYH;
        //(2)绑定子线程到虚拟机实例，即初始化env
        ret = pJavaVM->AttachCurrentThread(&env, NULL);
        if (JNI_OK != ret) {
            LOGE("AttachCurrentThread error,ret = %d", ret);
            return NULL;
        }
    }

    //(3)MonitorEnter/MonitorExit
    if(env->MonitorEnter(pctx->jniManagerObj)!=JNI_OK){
        LOGE("MonitorEnter error!!!");
        return nullptr;
    }
    //进入临界区
    for(int i=0;i<10;i++){
        globle_monitor++;
        LOGD("globle_monitor:%d thread:%lu ==================",globle_monitor,pthread_self());
    }
    if(env->MonitorExit(pctx->jniManagerObj)!=JNI_OK){
        LOGE("MonitorExit error!!!");
        return nullptr;
    }

    //(4)使得子线程脱离虚拟机实例
    pJavaVM->DetachCurrentThread();

    FUNCTION_QUIT;
    return context;
}
void native_monitorOperations(JNIEnv *env,jobject obj){
    FUNCTION_ENTER;

    //(1)创建线程monitorThread2
    pthread_t threadInfo_;
    pthread_attr_t threadAttr_;
    pthread_attr_init(&threadAttr_);
    pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

    jclass clz = env->GetObjectClass(obj);
    g_monitorctx.jniManagerClz = static_cast<jclass>(env->NewGlobalRef(clz));            //static_cast<jclass>:jobject-->jclass
    g_monitorctx.jniManagerObj = env->NewGlobalRef(obj);

    int res = pthread_create(&threadInfo_, &threadAttr_,monitorThread2, &g_monitorctx);
    assert(res == 0);

    //(2)MonitorEnter/MonitorExit
    if(env->MonitorEnter(obj)!=JNI_OK){
        LOGE("MonitorEnter error!!!");
        return ;
    }
    //进入临界区
    for(int i=0;i<20;i++){
        globle_monitor++;
        LOGD("globle_monitor:%d thread:%lu",globle_monitor,pthread_self());
    }
    if(env->MonitorExit(obj)!=JNI_OK){
        LOGE("MonitorExit error!!!");
        return ;
    }
    //(3)pthread_attr_destroy
    pthread_attr_destroy(&threadAttr_);

    FUNCTION_QUIT;
}



/***************************************************************************************************************/
jint Register_Hellolibs_Functions(JNIEnv * env)
{
    FUNCTION_ENTER;

	jclass clazz = env->FindClass(HELLOLIBS_JNI_CLASS_NAME);
	if (clazz == 0)
	{
		return -1;
	}

    /**
     * Table of methods associated with a single class.
     */
    static JNINativeMethod g_NativeMethods[] = {
		/*      name,                	        signature,               			    funcPtr   */
    	//{ "native_test", 					    "()I", 									(void*) Native_Test },
		{ "native_startTicks", 				    "()V", 									(void*) Native_StartTicks },
        { "native_stopTicks", 				    "()V", 									(void*) Native_StopTicks },
        { "native_stringOperations", 		    "()V", 									(void*) Native_StringOperations },
        { "native_arrayOperations", 		    "()V", 									(void*) Native_ArrayOperations },
        { "native_objectOperations", 		    "()V", 									(void*) Native_ObjectOperations },
        { "native_classAndInterfaceOperations", "()V", 									(void*) Native_ClassAndInterfaceOperations },
        { "native_exceptions",                  "()V", 									(void*) Native_Exceptions },
        { "native_globalAndLocalReferences",    "()V", 									(void*) Native_GlobalAndLocalReferences },
        { "native_instanceField",               "()V", 									(void*) Native_InstanceField },
        { "native_staticField",                 "()V", 									(void*) Native_StaticField },
        { "native_instanceMethod",              "()V", 									(void*) Native_InstanceMethod },
        { "native_staticMethod",                "()V", 									(void*) Native_StaticMethod },
        { "native_javaVMInterface",             "()V", 									(void*) native_JavaVMInterface },
        { "native_monitorOperations",           "()V", 									(void*) native_monitorOperations },
    };

	jint nMethods = sizeof(g_NativeMethods) / sizeof(JNINativeMethod);
	jint res = env->RegisterNatives(clazz, g_NativeMethods, nMethods);
	if (res < 0)
	{
		return -1;
	}
	env->DeleteLocalRef(clazz);

    FUNCTION_QUIT;
	return 0;
}

int WillCrash(){
    FUNCTION_ENTER;

    int i = 10;
    int j = i/0;
    char *k = nullptr;
    LOGD("k:%c",*k);

    FUNCTION_QUIT;
    return j;
}

/*
 * Returns the JNI version on success, -1 on failure.
 */JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    FUNCTION_ENTER;

	JNIEnv* env = NULL;
	jint ret;

    g_tickctx.javaVM = vm;
    g_tickctx.done = 0;
    //todo:g_tickctx.javaVM和g_monitorctx.javaVM 同一个即可
    g_monitorctx.javaVM = vm;

	if (JNI_OK != vm->GetEnv((void **) &env, JNI_VERSION_1_6))
	{
        LOGE("GetEnv error!!!");
		return JNI_ERR;
	}
    ret = env->GetVersion();
    LOGD("GetVersion:%x",ret);
	if (Register_Hellolibs_Functions(env) != 0)
	{
		LOGE("Register_Hellolibs_Functions error!!!");
		return JNI_ERR;
	}
    //20190102:test crash debug methods
    //WillCrash();
	/* success -- return valid version number */
	ret = JNI_VERSION_1_6;

    FUNCTION_QUIT;
	return ret;
}

static void UnregisterNativeMethods(JNIEnv* env, const char* className)
{
    FUNCTION_ENTER;

	jclass clazz;
	clazz = env->FindClass(className);
	if (NULL == clazz)  return;

	if (NULL != env)
	{
		env->UnregisterNatives(clazz);
	}

    FUNCTION_QUIT;
}

static void Unregisternatives(JNIEnv* env)
{
    FUNCTION_ENTER;

	UnregisterNativeMethods(env, HELLOLIBS_JNI_CLASS_NAME);

    FUNCTION_QUIT;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    FUNCTION_ENTER;

    JNIEnv *env = NULL;
    if (JNI_OK != vm->GetEnv((void **) &env, JNI_VERSION_1_6)) {
        return;
    }

	if (NULL != env)
	{
		Unregisternatives(env);
	}

    FUNCTION_QUIT;
}

