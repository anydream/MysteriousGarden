/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123


#ifndef _WWave_h_
#define _WWave_h_

#include "../AnyPlay.h"

namespace AnyPlay
{

#define SAMPLES 0
#define MILLISECONDS 1
#define SECONDS 2
#define BYTES 3


#define MAX_ERROR_STR MAX_PATH * 2

	class WWaveOut
	{
	public:

		void SetBufferData(char* data,DWORD index) ;
		WWaveOut();
		~WWaveOut();	 
		BOOL Initialize(WORD FormatTag, WORD Channels, DWORD SamplesPerSec,
			WORD BitsPerSample, UINT NumOfBuffers, UINT BufferSize);
		BOOL Uninitialize();
		BOOL OpenAudioOutput(UINT uDeviceID, DWORD dwCallback , DWORD dwCallbackInstance,
			DWORD fdwOpen);
		BOOL Close();
		BOOL GetInfo();
		BOOL PlayBuffer(int buffer);
		BOOL UnprepareHeader(int buffer);
		BOOL Stop();
		BOOL Pause();
		BOOL Resume();
		DWORD GetPosition(UINT wType = MILLISECONDS);

		DWORD SetVolume(WORD lvolume, WORD rvolume);
		DWORD GetVolume(WORD* lvolume, WORD* rvolume);


		LPSTR GetBufferData(DWORD index);
		void SetBufferSize(DWORD index, DWORD size);
		BOOL IsBufferDone(DWORD index);
		DWORD GetNumOfBuffers();
		void SetBufferDone(DWORD index);

		DWORD GetBufferSize(DWORD index);
		DWORD GetBufferAllocSize() { return _alloc_buffer_size;}


		BOOL PlayStream(char *stream, int size, int buffer);


		static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
			DWORD dwParam1, DWORD dwParam2);

	private:
		WAVEHDR* _waveHdr;
		WAVEFORMATEX _wfx;
		DWORD _number_of_buffers;
		DWORD _buffer_size;
		DWORD _samples_per_sec;
		DWORD channel;
		HWAVEOUT _whandle;
		WAVEOUTCAPS* _woc;
		UINT _num_devs;
		DWORD _alloc_buffer_size;


		void Error(char* errormessage);

	};

}

#endif
