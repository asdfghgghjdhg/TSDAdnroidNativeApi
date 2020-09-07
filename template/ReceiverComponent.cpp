#include "locale.h"

#include "StringUtils.h"
#include "Debug.h"
#include "ReceiverComponent.h"

enum Aliases {
    ALIAS_EN = 0,
    ALIAS_RU = 1
};

extern "C" JNIEXPORT void JNICALL Java_com_google_asdfghgghjdhg_tsdandroidnativeapi_ScannerReceiver_doExternalEvent(JNIEnv* env, jclass clazz, jlong v8Object, jstring source, jstring event, jstring data) {
    IAddInDefBaseEx* connection = (IAddInDefBaseEx*)v8Object;
    if (connection != nullptr) {
        WCHAR_T* s = NULL;
        WCHAR_T* e = NULL;
        WCHAR_T* d = NULL;
        convToWCHAR(&s, convFromJString(source));
        convToWCHAR(&e, convFromJString(event));
        convToWCHAR(&d, convFromJString(data));
        if (connection->ExternalEvent(s, e, d)) {
            //Debug::v(L"JNI", L"External event sent: " + convFromJString(event));
        }
    }
}

const std::wstring ReceiverComponent::EXTENSION_NAME = L"ReceiverComponentExtension";
const long ReceiverComponent::VERSION = 2100;
const std::wstring ReceiverComponent::JAVA_APP_CLASSNAME = L"com.google.asdfghgghjdhg.tsdandroidnativeapi.ScannerReceiver";
const std::wstring ReceiverComponent::PROPERTY_NAMES[] = { L"Enabled", L"DeviceId" };
const std::wstring ReceiverComponent::PROPERTY_NAMES_RU[] = { L"Включен", L"ИдентификаторУстройства" };
const std::wstring ReceiverComponent::METHOD_NAMES[] = { L"Enable", L"Disable", L"SendExternalEvent" };
const std::wstring ReceiverComponent::METHOD_NAMES_RU[] = { L"Включить", L"Выключить", L"ПослатьВнешнееСобытие" };

ReceiverComponent::ReceiverComponent() {
    this->connection = NULL;
    this->memoryManager = NULL;
    this->capabilities = eAppCapabilitiesInvalid;
    receiverEnabled = false;
    appClass = NULL;
    app = NULL;
}
ReceiverComponent::ReceiverComponent(AppCapabilities capabilities) : ReceiverComponent() {
    ReceiverComponent::capabilities = capabilities;
}
ReceiverComponent::~ReceiverComponent() {
    JNIEnv* env = getJniEnv();
    if (receiverEnabled) {
        jmethodID methID = env->GetMethodID(appClass, "disableReceiver", "()V");
        env->CallVoidMethod(app, methID);
    }

    if (app) { env->DeleteGlobalRef(app); }
    if (appClass) { env->DeleteGlobalRef(appClass); }
    
    this->connection = NULL;
    this->memoryManager = NULL;
}

// IInitDoneBase
bool ReceiverComponent::Init(void* connection) {
    Debug::v(EXTENSION_NAME, L"Init");

    ReceiverComponent::connection = (IAddInDefBaseEx*)connection;
    
    if (!app && ReceiverComponent::connection) {
        IAndroidComponentHelper* helper = (IAndroidComponentHelper*)ReceiverComponent::connection->GetInterface(eIAndroidComponentHelper);
        if (helper) {
            WCHAR_T* className = NULL;
            convToWCHAR(&className, JAVA_APP_CLASSNAME);
            jclass ccloc = helper->FindClass(className);
            delete[] className;
            if (ccloc) {
                JNIEnv* env = getJniEnv();
                appClass = static_cast<jclass>(env->NewGlobalRef(ccloc));
                env->DeleteLocalRef(ccloc);
                jobject activity = helper->GetActivity();
                jmethodID methID = env->GetMethodID(appClass, "<init>", "(Landroid/app/Activity;J)V");
                jobject objloc = env->NewObject(appClass, methID, activity, (jlong)ReceiverComponent::connection);
                app = static_cast<jobject>(env->NewGlobalRef(objloc));
                env->DeleteLocalRef(objloc);
                env->DeleteLocalRef(activity);
            }
        }
    }

    return ReceiverComponent::connection != 0;
}
bool ReceiverComponent::setMemManager(void* manager) {
    Debug::v(EXTENSION_NAME, L"setMemManager");

    ReceiverComponent::memoryManager = (IMemoryManager*)manager;
    return ReceiverComponent::memoryManager != 0;
}
long ReceiverComponent::GetInfo() {
    Debug::v(EXTENSION_NAME, L"GetInfo");

    return VERSION;
}
void ReceiverComponent::Done() {
    Debug::v(EXTENSION_NAME, L"Done");

    ReceiverComponent::connection = NULL;
}

// ILanguageExtenderBase
bool ReceiverComponent::RegisterExtensionAs(WCHAR_T** extensionName) {
    Debug::v(EXTENSION_NAME, L"RegisterExtensionAs: " + EXTENSION_NAME);

    size_t size = EXTENSION_NAME.length() + 1;
    if (memoryManager->AllocMemory((void**)extensionName, (unsigned long)size * sizeof(WCHAR_T))) {
        convToWCHAR(extensionName, EXTENSION_NAME);
        return true;
    }

    return false;
}
long ReceiverComponent::GetNProps() {
    Debug::v(EXTENSION_NAME, L"GetNProps");

    return PROPERTY_LAST;
}
long ReceiverComponent::FindProp(const WCHAR_T* propertyName) {
    long property = -1;
    std::wstring propName = convFromWCHAR(propertyName);
    Debug::v(EXTENSION_NAME, L"FindProp: " + propName);

    for (long i = 0; i < PROPERTY_LAST; i++) {
        if (propName.compare(PROPERTY_NAMES[i]) == 0 || propName.compare(PROPERTY_NAMES_RU[i]) == 0) {
            property = i;
            break;
        }
    }

    return property;
}
const WCHAR_T* ReceiverComponent::GetPropName(long property, long alias) {
    Debug::v(EXTENSION_NAME, format(L"GetPropName: %d", property));

    if (property < 0 || property >= PROPERTY_LAST) { return NULL; }

    std::wstring name = L"";

    switch (alias) {
    case ALIAS_EN:
        name = PROPERTY_NAMES[property];
        break;
    case ALIAS_RU:
        name = PROPERTY_NAMES_RU[property];
        break;
    }

    if (name.length()) {
        size_t size = name.length() + 1;
        WCHAR_T* propName = NULL;
        if (memoryManager->AllocMemory((void**)&propName, (unsigned long)size * sizeof(WCHAR_T))) {
            convToWCHAR(&propName, name);
            return propName;
        }
    }

    return NULL;
}
bool ReceiverComponent::GetPropVal(const long property, tVariant* value) {
    Debug::v(EXTENSION_NAME, format(L"GetPropVal: %d", property));

    switch (property) {
        case PROPERTY_ENABLED: {
            value->vt = VTYPE_BOOL;
            value->bVal = receiverEnabled;
            return true;
        }
        case PROPERTY_DEVICEID: {
            value->vt = VTYPE_EMPTY;
            if (app && memoryManager) {
                JNIEnv* env = getJniEnv();
                jmethodID methID = env->GetMethodID(appClass, "getDeviceId", "()Ljava/lang/String;");
                jstring id = (jstring)env->CallObjectMethod(app, methID);
                std::wstring deviceId = convFromJString(id);
                env->DeleteLocalRef(id);

                if (convToV8String(value, deviceId, memoryManager)) { return true; }
            }
            break;
        }
    }

    return false;
}
bool ReceiverComponent::SetPropVal(const long property, tVariant* value) {
    Debug::v(EXTENSION_NAME, format(L"SetPropVal: %d", property));

    return false;
}
bool ReceiverComponent::IsPropReadable(const long property) {
    Debug::v(EXTENSION_NAME, format(L"IsPropReadable: %d", property));

    switch (property) {
        case PROPERTY_ENABLED:
            return true;
        case PROPERTY_DEVICEID:
            return true;
    }

    return false;
}
bool ReceiverComponent::IsPropWritable(const long property) {
    Debug::v(EXTENSION_NAME, format(L"IsPropWritable: %d", property));

    return false;
}
long ReceiverComponent::GetNMethods() {
    Debug::v(EXTENSION_NAME, L"GetNMethods");

    return METHOD_LAST;
}
long ReceiverComponent::FindMethod(const WCHAR_T* methodName) {
    long method = -1;
    std::wstring methName = convFromWCHAR(methodName);
    Debug::v(EXTENSION_NAME, L"FindMethod: " + methName);

    for (long i = 0; i < METHOD_LAST; i++) {
        if (methName.compare(METHOD_NAMES[i]) == 0 || methName.compare(METHOD_NAMES_RU[i]) == 0) {
            method = i;
            break;
        }
    }

    return method;
}
const WCHAR_T* ReceiverComponent::GetMethodName(const long method, const long alias) {
    Debug::v(EXTENSION_NAME, format(L"GetMethodName: %d", method));

    if (method < 0 || method >= METHOD_LAST) { return NULL; }

    std::wstring name = L"";

    switch (alias) {
    case ALIAS_EN:
        name = METHOD_NAMES[method];
        break;
    case ALIAS_RU:
        name = METHOD_NAMES_RU[method];
        break;
    }

    if (name.length()) {
        size_t size = name.length() + 1;
        WCHAR_T* methName = NULL;
        if (memoryManager->AllocMemory((void**)&methName, (unsigned long)size * sizeof(WCHAR_T))) {
            convToWCHAR(&methName, name);
            return methName;
        }
    }

    return NULL;
}
long ReceiverComponent::GetNParams(const long method) {
    Debug::v(EXTENSION_NAME, format(L"GetNParams: %d", method));

    switch (method) {
        case METHOD_SEND_EXTERNAL_EVENT:
            return 2;
    }

    return 0;
}
bool ReceiverComponent::GetParamDefValue(const long method, const long paramNum, tVariant* value) {
    Debug::v(EXTENSION_NAME, format(L"GetParamDefValue: %d,%d", method, paramNum));

    return false;
}
bool ReceiverComponent::HasRetVal(const long method) {
    Debug::v(EXTENSION_NAME, format(L"HasRetVal: %d", method));

    return false;
}
bool ReceiverComponent::CallAsProc(const long method, tVariant* params, const long paramsCount) {
    Debug::v(EXTENSION_NAME, format(L"CallAsProc: %d", method));

    switch (method) {
        case METHOD_ENABLE: {
            if (app) {
                JNIEnv* env = getJniEnv();
                jmethodID methID = env->GetMethodID(appClass, "enableReceiver", "()V");
                env->CallVoidMethod(app, methID);

                receiverEnabled = true;
                return true;
            }
            break;
        }
        case METHOD_DISABLE: {
            if (app) {
                JNIEnv* env = getJniEnv();
                jmethodID methID = env->GetMethodID(appClass, "disableReceiver", "()V");
                env->CallVoidMethod(app, methID);

                receiverEnabled = false;
                return true;
            }
        }
        case METHOD_SEND_EXTERNAL_EVENT: {
            if (paramsCount == 2 && app) {
                std::wstring event = convFromV8String(&params[0]);
                std::wstring data = convFromV8String(&params[1]);
                Debug::v(EXTENSION_NAME, L"Event: " + event + L", Data: " + data);

                JNIEnv* env = getJniEnv();
                jmethodID methID = env->GetMethodID(appClass, "sendExternalEvent", "(Ljava/lang/String;Ljava/lang/String;)V");
                jstring jevent = env->NewStringUTF(convToString(event).c_str());
                jstring jdata = env->NewStringUTF(convToString(data).c_str());
                env->CallVoidMethod(app, methID, jevent, jdata);
                env->DeleteLocalRef(jevent);
                env->DeleteLocalRef(jdata);
                return true;
            }
        }
    }

    return false;
}
bool ReceiverComponent::CallAsFunc(const long method, tVariant* value, tVariant* params, const long paramsCount) {
    Debug::v(EXTENSION_NAME, format(L"CallAsFunc: %d", method));

    return false;
}

// LocaleBase
void ReceiverComponent::SetLocale(const WCHAR_T* locale) {
    std::wstring loc = convFromWCHAR(locale);
    Debug::v(EXTENSION_NAME, L"SetLocale: " + loc);

#ifdef _WINDOWS
    _wsetlocale(LC_ALL, locale);
#else
    setlocale(LC_ALL, convToString(loc).c_str());
#endif

}