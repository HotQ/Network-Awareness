#ifndef BVNETWORKLISTMANAGER_H
#define BVNETWORKLISTMANAGER_H
#include <memory>
#include <netlistmgr.h>
#pragma comment(lib, "ole32.lib")

typedef void (*NLM_CALLBACK)(PVOID);

#define NLM_FRST_WAIT_PERIOD 10000
#define NLM_LOOP_WAIT_PERIOD 10000

#define WM_USER_NLM (WM_USER + 100)

class BV_NLM;
class BV_NetWorkEvent : public INetworkListManagerEvents
{
private:
	LONG m_ref;
	bool m_conAlive;
	bool m_inLoop;

	HANDLE m_hTimerQueue;
	HANDLE m_hTimer;

	BV_NLM *sink;

	static VOID CALLBACK LostWait(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

public:
	BV_NetWorkEvent(bool isConAlive);

	void setSink(BV_NLM *sink);

	virtual HRESULT __stdcall ConnectivityChanged(NLM_CONNECTIVITY netStat);

	STDMETHODIMP QueryInterface(REFIID refIID, void **pIFace);
	virtual ULONG __stdcall AddRef(void);
	virtual ULONG __stdcall Release(void);
};

class BV_NLM
{
private:
	static std::shared_ptr<BV_NLM> m_instance;
	static DWORD WINAPI AdviseDispatch(LPVOID lpParam);

	DWORD ThreadID;
	HANDLE hThread;

	BV_NLM();

public:
	BV_NLM(const BV_NLM &) = delete;			//禁止拷贝
	BV_NLM &operator=(const BV_NLM &) = delete; //禁止赋值
	static std::shared_ptr<BV_NLM> GetInstance();

	NLM_CALLBACK m_onConInterrupted;
	NLM_CALLBACK m_onConLost;
	PVOID lpParamInterrupted;
	PVOID lpParamLost;

	void onInterrupted(NLM_CALLBACK callback, PVOID lpParameter);
	void onLost(NLM_CALLBACK callback, PVOID lpParameter);
	static void Destroy();
	void Begin();
	void Stop();

	~BV_NLM();
};

#endif // !BVNETWORKLISTMANAGER_H