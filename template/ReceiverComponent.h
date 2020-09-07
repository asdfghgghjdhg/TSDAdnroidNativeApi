#ifndef __TEMPLATECOMPONENT_H__
#define __TEMPLATECOMPONENT_H__

#include <jni.h>
#include <string>
#ifdef _WINDOWS
#include <wtypes.h>
#endif

#include "../include/ComponentBase.h"
#include "../include/AddInDefBase.h"
#include "../include/IMemoryManager.h"
#include "../include/mobile.h"
#include "../include/IAndroidComponentHelper.h"
#include "../jni/jnienv.h"

class ReceiverComponent : public IComponentBase {
public:
    static const std::wstring EXTENSION_NAME;
    static const long VERSION;
    static const std::wstring JAVA_APP_CLASSNAME;

    enum Properties {
        PROPERTY_ENABLED,
        PROPERTY_DEVICEID,
        PROPERTY_LAST
    };

    enum Methods {
        METHOD_ENABLE,
        METHOD_DISABLE,
        METHOD_SEND_EXTERNAL_EVENT,
        METHOD_LAST
    };

    static const std::wstring PROPERTY_NAMES[PROPERTY_LAST];
    static const std::wstring PROPERTY_NAMES_RU[PROPERTY_LAST];
    static const std::wstring METHOD_NAMES[METHOD_LAST];
    static const std::wstring METHOD_NAMES_RU[METHOD_LAST];

    ReceiverComponent();
    ReceiverComponent(AppCapabilities capabilities);
    virtual ~ReceiverComponent();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void* connection);
    virtual bool ADDIN_API setMemManager(void* manager);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** extensionName);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* propertyName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long property, long alias);
    virtual bool ADDIN_API GetPropVal(const long property, tVariant* value);
    virtual bool ADDIN_API SetPropVal(const long property, tVariant* value);
    virtual bool ADDIN_API IsPropReadable(const long property);
    virtual bool ADDIN_API IsPropWritable(const long property);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* methodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long method, const long alias);
    virtual long ADDIN_API GetNParams(const long method);
    virtual bool ADDIN_API GetParamDefValue(const long method, const long paramNum, tVariant* value);
    virtual bool ADDIN_API HasRetVal(const long method);
    virtual bool ADDIN_API CallAsProc(const long method, tVariant* params, const long paramsCount);
    virtual bool ADDIN_API CallAsFunc(const long method, tVariant* value, tVariant* params, const long paramsCount);
    operator IComponentBase* () { return (IComponentBase*)this; }
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* locale);
private:
    IAddInDefBaseEx* connection;
    IMemoryManager* memoryManager;
    AppCapabilities capabilities;
    bool receiverEnabled;

    jclass appClass;
    jobject app;
};

#endif