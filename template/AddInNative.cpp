#include "AddInNative.h"
#include "StringUtils.h"
#include "Debug.h"

#include "ReceiverComponent.h"

enum Classes {
	COMPONENT_RECEIVER = 0,
	CLASS_LAST
};

static const std::wstring CLASS_NAMES[CLASS_LAST] = { L"RecevierComponent" };

static AppCapabilities capabilities = eAppCapabilitiesInvalid;

const WCHAR_T* GetClassNames() {
	static WCHAR_T* classNames = NULL;

	if (CLASS_LAST) {
		std::wstring classes = L"";
		for (long i = 0; i < CLASS_LAST; i++) {
			classes = classes + CLASS_NAMES[i];
			if (i < CLASS_LAST - 1) {
				classes = classes + L"|";
			}
		}

		convToWCHAR(&classNames, classes);
		Debug::v(L"AddInNative", L"GetClassNames: " + classes);
	}

	return classNames;
}

long GetClassObject(const WCHAR_T* className, IComponentBase** pIntf) {
	std::wstring name = convFromWCHAR(className);
	
	Debug::v(L"AddInNative", L"GetClassObject: " + name);

	long classNum = -1;
	for (long i = 0; i < CLASS_LAST; i++) {
		if (name.compare(CLASS_NAMES[i]) == 0) {
			classNum = i;
			break;
		}
	};

	switch (classNum) {
	case COMPONENT_RECEIVER:
		*pIntf = new ReceiverComponent(capabilities);
		if (*pIntf) { return 1; }
		break;
	}

	return 0;
}

long DestroyObject(IComponentBase** pIntf) {
	if (!*pIntf) { return -1; }

	Debug::v(L"AddInNative", L"DestroyObject");

	delete* pIntf;
	*pIntf = NULL;
	return 0;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities) {
	Debug::v(L"AddInNative", L"SetPlatformCapabilities");
	
	::capabilities = capabilities;
	return eAppCapabilitiesLast;
}
