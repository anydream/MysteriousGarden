/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#include "../AnyPlay.h"

#ifdef _MSC_VER
#pragma comment( lib, "winmm")
#endif

#include "wwave.h"


namespace AnyPlay
{

	WWaveOut::WWaveOut()
	{
		_whandle = 0;
		_woc = 0;
		_alloc_buffer_size = 0;


		_waveHdr = 0;
	}

	WWaveOut::~WWaveOut()
	{
		if( _woc) delete [] _woc;

	}


	BOOL WWaveOut::GetInfo()
	{
		_num_devs = waveOutGetNumDevs();
		if(_num_devs == 0)
		{
			Error("WWaveOut::GetInfo->No wave out devices are presen");
			return FALSE;
		}

		_woc = new WAVEOUTCAPS[_num_devs];

		for (UINT i = 0; i < _num_devs; i++ )
		{
			waveOutGetDevCaps(i, &_woc[i], sizeof(WAVEOUTCAPS));

		}

		return TRUE;
	}


	BOOL WWaveOut::OpenAudioOutput(UINT uDeviceID, DWORD dwCallback, DWORD dwCallbackInstance,
		DWORD fdwOpen)
	{



		DWORD OutProc;
		if(dwCallback == 0)
			OutProc = (DWORD) waveOutProc;
		else
			OutProc = dwCallback;

		MMRESULT result;
		result = waveOutOpen(&_whandle,uDeviceID,&_wfx, OutProc, dwCallbackInstance, fdwOpen);

		switch (result)
		{
		case MMSYSERR_NOERROR:
			return TRUE;

		case MMSYSERR_ALLOCATED:
			Error("WWaveOut::OpenAudioOutput->Specified resource is already allocated");

		case MMSYSERR_BADDEVICEID:
			Error("WWaveOut::OpenAudioOutput->Specified device identifier is out of range");

		case MMSYSERR_NODRIVER:
			Error("WWaveOut::OpenAudioOutput->No device driver is present");

		case MMSYSERR_NOMEM:
			Error("WWaveOut::OpenAudioOutput->Unable to allocate or lock memory");

		case WAVERR_BADFORMAT:
			Error("WWaveOut::OpenAudioOutput->Attempted to open with an unsupported waveform-audio format");
		case WAVERR_SYNC:
			Error("WWaveOut::OpenAudioOutput->The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag");

		default:
			Error("WWaveOut::OpenAudioOutput->Unknown error");
		}

		return FALSE;
	}




	BOOL WWaveOut::PlayStream(char *stream, int size, int buffer)
	{

		_waveHdr[buffer].lpData = stream;
		_waveHdr[buffer].dwBufferLength = size;
		_waveHdr[buffer].dwFlags = 0L;
		_waveHdr[buffer].dwLoops = 0L;

		if(waveOutPrepareHeader(_whandle, &_waveHdr[buffer], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			Error("WWaveOut::PlayBuffer->Can't prepare data");
			return FALSE;
		}

		if(waveOutWrite(_whandle, &_waveHdr[buffer], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			Error("WWaveOut::PlayBuffer->Can't write data");
			return FALSE;

		}

		return TRUE;

	}


	BOOL WWaveOut::PlayBuffer(int buffer)
	{







		_waveHdr[buffer].dwFlags = 0L;
		_waveHdr[buffer].dwLoops = 0L;


		if(waveOutPrepareHeader(_whandle, &_waveHdr[buffer], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			Error("WWaveOut::PlayBuffer->Can't prepare data");
			return FALSE;
		}

		if(waveOutWrite(_whandle, &_waveHdr[buffer], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
		{
			Error("WWaveOut::PlayBuffer->Can't write data");
			return FALSE;

		}

		return TRUE;

	}




	void CALLBACK WWaveOut::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
		DWORD dwParam1, DWORD dwParam2)
	{
		switch(uMsg)
		{
		case WOM_DONE:
			{




			}
			break;
		}
	}




	BOOL WWaveOut::UnprepareHeader(int buffer)
	{
		if( waveOutUnprepareHeader(_whandle, &_waveHdr[buffer],sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
			return TRUE;

		return FALSE;
	}

	BOOL WWaveOut::Stop()
	{
		waveOutReset(_whandle);
		return TRUE;
	}

	BOOL WWaveOut::Resume()
	{
		waveOutRestart(_whandle);
		return TRUE;
	}

	DWORD WWaveOut::GetPosition(UINT wType)
	{
		if(!_whandle)
			return 0;
		MMTIME mmt;
		switch (wType)
		{
		case BYTES:
			mmt.wType = TIME_BYTES;
			waveOutGetPosition(_whandle, &mmt, sizeof(MMTIME));
			return mmt.u.cb;

		case SAMPLES:
			mmt.wType = TIME_SAMPLES;
			waveOutGetPosition(_whandle, &mmt, sizeof(MMTIME));
			return mmt.u.sample;

		case MILLISECONDS:
			mmt.wType = TIME_SAMPLES;
			waveOutGetPosition(_whandle, &mmt, sizeof(MMTIME));
			return ( unsigned int) ((double) mmt.u.sample * 1000.0 / (double) _samples_per_sec );

		case SECONDS:
			mmt.wType = TIME_SAMPLES;
			waveOutGetPosition(_whandle, &mmt, sizeof(MMTIME));
			return ( mmt.u.sample / _samples_per_sec);


		}
		return 0;
	}

	BOOL WWaveOut::Pause()
	{
		::waveOutPause(_whandle);
		return TRUE;
	}

	BOOL WWaveOut::Close()
	{
		MMRESULT result;

		result = ::waveOutClose(_whandle);


		switch(result)
		{
		case MMSYSERR_NOERROR:
			_whandle = 0;
			return TRUE;

		case MMSYSERR_NODRIVER:
			Error("WWaveOut::Close->No device driver is present");
		case MMSYSERR_INVALHANDLE:
			Error("WWaveOut::Close->Specified device handle is invalid");
		case MMSYSERR_NOMEM:
			Error("WWaveOut::Close->Unable to allocate or lock memory");
		case WAVERR_STILLPLAYING:
			Error("WWaveOut::Close->There are still buffers in the queue");
		default:
			Error("WWaveOut::Close->Unknown error");

		}

		return FALSE;
	}

	BOOL WWaveOut::Initialize(WORD FormatTag, WORD Channels, DWORD SamplesPerSec,
		WORD BitsPerSample, UINT NumOfBuffers, UINT BufferSize)
	{

		if(_waveHdr)
		{
			Error("WWaveOut::Initialize->Already initializet. Uninitialize first.");
			return FALSE;
		}

		_wfx.wFormatTag = FormatTag;
		_wfx.nChannels = Channels;
		_wfx.nSamplesPerSec = SamplesPerSec;
		_wfx.nAvgBytesPerSec = ( Channels * SamplesPerSec * BitsPerSample ) / 8 ;
		_wfx.nBlockAlign = (unsigned short)( ( Channels * BitsPerSample ) / 8);
		_wfx.wBitsPerSample = (unsigned short) BitsPerSample;
		_wfx.cbSize = 0;


		_waveHdr = ( WAVEHDR * ) malloc(sizeof(WAVEHDR) * NumOfBuffers);
		if(_waveHdr == NULL )
		{
			Error ( "WWave::Initialize->Can't allocate memory for WAVEHDR structure");
			return FALSE;
		}

		ZeroMemory(_waveHdr, sizeof(WAVEHDR) * NumOfBuffers);


		_alloc_buffer_size = (unsigned int) ( (double) _wfx.nAvgBytesPerSec * (double) BufferSize / 1000.0 );


		for (UINT i = 0; i < NumOfBuffers; i++ )
		{

			_waveHdr[i].lpData = (char* ) malloc(_alloc_buffer_size);
			_waveHdr[i].dwUser = i;
			if(_waveHdr[i].lpData == NULL )
			{
				for(UINT j = 0; j < i; j++)
					free(_waveHdr[j].lpData);
				free(_waveHdr);
				_waveHdr = 0;
				Error ( "WWave::Initialize->Can't allocate memory for output data buffer");
				return FALSE;

			}
		}

		_number_of_buffers = NumOfBuffers;
		_buffer_size = BufferSize;
		_samples_per_sec = SamplesPerSec;

		return TRUE;

	}

	BOOL WWaveOut::Uninitialize()
	{
		if(_waveHdr == 0)
			return TRUE;
		for (UINT i = 0; i < _number_of_buffers; i++ )
		{
			free(_waveHdr[i].lpData);

		}
		free(_waveHdr);
		_waveHdr = 0;



		return TRUE;
	}

	LPSTR WWaveOut::GetBufferData(DWORD index) { return _waveHdr[index].lpData; };
	void WWaveOut::SetBufferSize(DWORD index, DWORD size) { _waveHdr[index].dwBufferLength = size;};
	BOOL WWaveOut::IsBufferDone(DWORD index) {  return ((_waveHdr[index].dwFlags & WHDR_DONE) ==WHDR_DONE )  ? 1 : 0;};
	DWORD WWaveOut::GetNumOfBuffers() { return _number_of_buffers; };
	void WWaveOut::SetBufferDone(DWORD index) { _waveHdr[index].dwFlags |= WHDR_DONE; };
	void WWaveOut::SetBufferData(char* data,DWORD index) { _waveHdr[index].lpData = data; };



	void WWaveOut::Error(char* errormessage)
	{
	}

	DWORD WWaveOut::SetVolume(WORD lvolume, WORD rvolume)
	{
		return ::waveOutSetVolume(_whandle, MAKELONG(lvolume, rvolume));
	}


	DWORD WWaveOut::GetVolume(WORD* lvolume, WORD* rvolume)
	{
		MMRESULT ret;
		DWORD vol;
		ret =  ::waveOutGetVolume(_whandle, &vol);
		*lvolume = LOWORD(vol);
		*rvolume = HIWORD(vol);
		return ret;
	}



	DWORD WWaveOut::GetBufferSize(DWORD index)
	{ 
		return _waveHdr[index].dwBufferLength;
	}


}