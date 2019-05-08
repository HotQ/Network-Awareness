#include <iostream>
#include <atlbase.h>
#include <windows.h>

#include "BVNetworkListManager.h"

#define show(str) printf(#str##"\n");
#define _CRTDBG_MAP_ALLOC

VOID BV_NetWorkEvent::LostWait(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	BV_NLM *sink = static_cast<BV_NLM *>(lpParameter);
	sink->m_onConLost(sink->lpParamLost);
}

BV_NetWorkEvent::BV_NetWorkEvent(bool isConAlive)
{
	m_ref = 1;
	m_sink = nullptr;
	m_hTimerQueue = CreateTimerQueue();
	m_hTimer = NULL;
	m_inLoop = false;
	m_conAlive = isConAlive;
}

void BV_NetWorkEvent::setSink(BV_NLM *sink)
{
	m_sink = sink;
}

HRESULT __stdcall BV_NetWorkEvent::ConnectivityChanged(NLM_CONNECTIVITY netStat)
{
	bool conAlive;

	if (netStat == NLM_CONNECTIVITY_DISCONNECTED)
	{// 物理断网
		show(NLM_CONNECTIVITY_DISCONNECTED);
		conAlive = false;
	}
	else
	{
		if ((netStat & NLM_CONNECTIVITY_IPV4_NOTRAFFIC) && (netStat & NLM_CONNECTIVITY_IPV6_NOTRAFFIC))
		{// 无连接
			conAlive = false;
			show(NLM_CONNECTIVITY_IPV46_NOTRAFFIC)
		}
		else
		{
			conAlive = true;
		}
	}

	if (conAlive == true && m_conAlive == false)
	{
		m_conAlive = conAlive;
		show(断->活);
		if (m_inLoop)
		{
			DeleteTimerQueueTimer(m_hTimerQueue, m_hTimer, NULL);
			m_inLoop = false;
		}
	}
	if (conAlive == false && m_conAlive == true)
	{
		show(活->断)
			m_conAlive = conAlive;
		if (m_sink != nullptr && m_sink->m_onConInterrupted != nullptr)
		{
			m_sink->m_onConInterrupted(m_sink->lpParamInterrupted);
		}

		if (m_sink != nullptr && m_sink->m_onConLost != nullptr)
		{
		m_inLoop = true;
			CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue, WAITORTIMERCALLBACK(LostWait), m_sink, NLM_FRST_WAIT_PERIOD, NLM_LOOP_WAIT_PERIOD, NULL);
		}
	}
	m_conAlive = conAlive;
	printf("ConnectivityChanged: %04X\n", netStat);
	return S_OK;
}

STDMETHODIMP BV_NetWorkEvent::QueryInterface(REFIID refIID, void **pIFace)
{
	HRESULT hr = S_OK;
	*pIFace = NULL;
	if (IsEqualIID(refIID, IID_IUnknown))
	{
		*pIFace = (IUnknown *)this;
		((IUnknown *)*pIFace)->AddRef();
	}
	else if (IsEqualIID(refIID, IID_INetworkListManagerEvents))
	{
		*pIFace = (INetworkListManagerEvents *)this;
		((IUnknown *)*pIFace)->AddRef();
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}

ULONG __stdcall BV_NetWorkEvent::AddRef(void)
{
	return (ULONG)InterlockedIncrement(&m_ref);
}

ULONG __stdcall BV_NetWorkEvent::Release(void)
{
	LONG Result = InterlockedDecrement(&m_ref);
	if (Result == 0)
		delete this;
	return (ULONG)Result;
}

/* TODO: 错误处理 */
DWORD WINAPI BV_NLM::AdviseDispatch(LPVOID lpParam)
{
	/* CComPtr是atl实现的智能指针，自动执行Realse防leak */
	CComPtr<INetworkListManager>		pNLM;
	CComPtr<IConnectionPointContainer>	pCpc;
	CComPtr<IConnectionPoint>           pConnectionPoint;
	DWORD m_cookie;
	MSG msg;

	HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (LPVOID *)&pNLM);
	printf("CLSID_NetworkListManager: %d\n", FAILED(hr));

	VARIANT_BOOL bConnected = VARIANT_FALSE;
	hr = pNLM->get_IsConnected(&bConnected);

	hr = pNLM->QueryInterface(IID_IConnectionPointContainer, (void **)&pCpc);
	hr = pCpc->FindConnectionPoint(IID_INetworkListManagerEvents, &pConnectionPoint);

	std::shared_ptr<BV_NetWorkEvent> pEvent = std::make_shared<BV_NetWorkEvent>(bConnected == VARIANT_TRUE);
	pEvent->setSink(static_cast<BV_NLM *>(lpParam));
	hr = pConnectionPoint->Advise((IUnknown *)pEvent.get(), &m_cookie);

	/* GetMessage阻塞 */
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT || msg.message == WM_USER_NLM)
		{
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (pConnectionPoint != NULL)
	{
		pConnectionPoint->Unadvise(m_cookie);
	}
	return 0;
}

BV_NLM::BV_NLM()
{
	/*
	 * CoInitialize() 和CoUninitialize()需配对使用
	 * 且两个函数内部维护了引用计数，so keep going
	 */
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_onConInterrupted = nullptr;
	m_onConLost = nullptr;
}

BV_NLM::~BV_NLM()
{
	CoUninitialize();
}

std::shared_ptr<BV_NLM> BV_NLM::m_instance(new BV_NLM());

std::shared_ptr<BV_NLM> BV_NLM::GetInstance()
{
	return m_instance;
}

void BV_NLM::onInterrupted(NLM_CALLBACK callback, PVOID lpParameter)
{
	m_onConInterrupted = callback;
	lpParamInterrupted = lpParameter;
}

void BV_NLM::onLost(NLM_CALLBACK callback, PVOID lpParameter)
{
	m_onConLost = callback;
	lpParamLost = lpParameter;
}

void BV_NLM::Destroy()
{
	m_instance.reset();
}

void BV_NLM::Begin()
{
	hThread = CreateThread(NULL, 0, AdviseDispatch, (LPVOID)this, 0, &ThreadID);
	show(======================);
	printf("THREAD ID == %d\n", ThreadID);
}

void BV_NLM::Stop()
{
	PostThreadMessage(ThreadID, WM_USER_NLM, 0, 0);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
}
/*
 * https://docs.microsoft.com/zh-cn/previous-versions/windows/desktop/mpc/network-awareness-on-windows-vista-and-windows-7
 */