/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#ifndef _Wmp3_h_
#define _Wmp3_h_

#include "wwave.h"
#include "wmp3decoder.h"
#include "wequ.h"
#include "../FrameWork/Manager.h"

namespace AnyPlay
{
	typedef struct
	{
		int h;
		int m;
		int s;
		int ms;
	} MP3_TIME;

	struct tag_xing
	{
		long flags;		    
		unsigned long frames;	    
		unsigned long bytes;	    
		unsigned char toc[100];   
		long scale;		    
	};

	enum rgain_name
	{
		RGAIN_NAME_NOT_SET    = 0x0,
		RGAIN_NAME_RADIO      = 0x1,
		RGAIN_NAME_AUDIOPHILE = 0x2
	};

	enum rgain_originator
	{
		RGAIN_ORIGINATOR_UNSPECIFIED = 0x0,
		RGAIN_ORIGINATOR_PRESET      = 0x1,
		RGAIN_ORIGINATOR_USER        = 0x2,
		RGAIN_ORIGINATOR_AUTOMATIC   = 0x3
	};

	struct rgain
	{
		enum rgain_name name;			 
		enum rgain_originator originator;	 
		signed short adjustment;		 
	};

	enum tag_lame_vbr
	{
		TAG_LAME_VBR_CONSTANT      = 1,
		TAG_LAME_VBR_ABR           = 2,
		TAG_LAME_VBR_METHOD1       = 3,
		TAG_LAME_VBR_METHOD2       = 4,
		TAG_LAME_VBR_METHOD3       = 5,
		TAG_LAME_VBR_METHOD4       = 6,
		TAG_LAME_VBR_CONSTANT2PASS = 8,
		TAG_LAME_VBR_ABR2PASS      = 9
	};

	enum tag_lame_source
	{
		TAG_LAME_SOURCE_32LOWER  = 0x00,
		TAG_LAME_SOURCE_44_1     = 0x01,
		TAG_LAME_SOURCE_48       = 0x02,
		TAG_LAME_SOURCE_HIGHER48 = 0x03
	};

	enum tag_lame_mode
	{
		TAG_LAME_MODE_MONO      = 0x00,
		TAG_LAME_MODE_STEREO    = 0x01,
		TAG_LAME_MODE_DUAL      = 0x02,
		TAG_LAME_MODE_JOINT     = 0x03,
		TAG_LAME_MODE_FORCE     = 0x04,
		TAG_LAME_MODE_AUTO      = 0x05,
		TAG_LAME_MODE_INTENSITY = 0x06,
		TAG_LAME_MODE_UNDEFINED = 0x07
	};

	enum tag_lame_surround
	{
		TAG_LAME_SURROUND_NONE      = 0,
		TAG_LAME_SURROUND_DPL       = 1,
		TAG_LAME_SURROUND_DPL2      = 2,
		TAG_LAME_SURROUND_AMBISONIC = 3
	};

	enum tag_lame_preset
	{
		TAG_LAME_PRESET_NONE          =    0,
		TAG_LAME_PRESET_V9            =  410,
		TAG_LAME_PRESET_V8            =  420,
		TAG_LAME_PRESET_V7            =  430,
		TAG_LAME_PRESET_V6            =  440,
		TAG_LAME_PRESET_V5            =  450,
		TAG_LAME_PRESET_V4            =  460,
		TAG_LAME_PRESET_V3            =  470,
		TAG_LAME_PRESET_V2            =  480,
		TAG_LAME_PRESET_V1            =  490,
		TAG_LAME_PRESET_V0            =  500,
		TAG_LAME_PRESET_R3MIX         = 1000,
		TAG_LAME_PRESET_STANDARD      = 1001,
		TAG_LAME_PRESET_EXTREME       = 1002,
		TAG_LAME_PRESET_INSANE        = 1003,
		TAG_LAME_PRESET_STANDARD_FAST = 1004,
		TAG_LAME_PRESET_EXTREME_FAST  = 1005,
		TAG_LAME_PRESET_MEDIUM        = 1006,
		TAG_LAME_PRESET_MEDIUM_FAST   = 1007,
		TAG_LAME_PRESET_PORTABLE      = 1010,
		TAG_LAME_PRESET_RADIO         = 1015
	};

	struct tag_lame
	{
		unsigned char revision;
		unsigned char flags;

		enum tag_lame_vbr vbr_method;
		unsigned short lowpass_filter;

		mad_fixed_t peak;
		struct rgain replay_gain[2];

		unsigned char ath_type;
		unsigned char bitrate;

		int start_delay;
		int end_padding;

		enum tag_lame_source source_samplerate;
		enum tag_lame_mode stereo_mode;
		unsigned char noise_shaping;

		signed char gain;
		enum tag_lame_surround surround;
		enum tag_lame_preset preset;

		unsigned long music_length;
		unsigned short music_crc;
	};

	struct tag_xl
	{
		int flags;
		struct tag_xing xing;
		struct tag_lame lame;
		char encoder[21];
	};


#define ERRORMESSAGE_SIZE 1024  

#define ECHO_BUFFER_NUMBER 40
#define ECHO_MAX_DELAY_NUMBER 20
	typedef int weq_param[10];
	typedef weq_param EQ_PARAM;


#define MPEG1 0
#define MPEG2 1
#define MPEG25 2

#define ID3_VERSION2	2
#define ID3_VERSION1	1

	typedef struct
	{
		BOOL play;
		BOOL pause;
		BOOL stop;
		BOOL echo;
		unsigned int echo_mode;
		BOOL eq_external;
		BOOL eq_internal;
		BOOL vocal_cut;
		BOOL channel_mix;
		BOOL fade_in;
		BOOL fade_out;
		BOOL internal_volume;
		BOOL loop;
	} MP3_STATUS;

#define real double

	static unsigned long get32bits(unsigned char *buf);
	typedef real SFX_MODE[ECHO_MAX_DELAY_NUMBER];

#define mad_fixed_t signed int

	typedef struct audio_dither
	{
		mad_fixed_t error[3];
		mad_fixed_t random;
	} AUTODITHER_STRUCT;


	typedef struct
	{
		char Header[10];
		DWORD MajorVersion;
		DWORD RevisionNumber;
		DWORD _flags;
		struct
		{
			BOOL Unsync;
			BOOL Extended;
			BOOL Experimental;
			BOOL Footer;
		} Flags;
		DWORD Size;

	} ID3v2STRUCT;


	typedef struct
	{
		DWORD Size;
		DWORD _flags;
		struct
		{
			BOOL Tag_alter_preservation;
			BOOL File_alter_preservation;
			BOOL Read_only;
			BOOL Grouping_identity;
			BOOL Compression;
			BOOL Encryption;
			BOOL Unsynchronisation;
			BOOL Data_length_indicator;
		} Flags;
	} ID3v2FRAME;

	typedef struct
	{
		char *artist;
		char *album;
		char *title;
		char *comment;
		char *genre;
		char *year;
		char *tracknum;
		char *ID3TAG;
		char *NEWID3;
		char header[10];
		int currsize;
		int currpos;
		int size;
		int ready;
		int dynbuffer;
		int	footer, extended, experimental;
		DWORD flags;

		ID3v2STRUCT Id3v2;
	} ID3STRUCT;

	class AudioSystem;
	//////////////////////////////////////////////////////////////////////////
	// MP3播放
	class WMp3 : public Managed<WMp3>
	{
	public:
		friend class AudioSystem;

		WMp3();
		~WMp3();  

		char* GetError() { return _lpErrorMessage;};
		BOOL OpenMp3File(LPCTSTR filename ,int WaveBufferLengthMs);
		int OpenMp3File(LPCTSTR filename, int WaveBufferLengthMs, DWORD seek, DWORD size);
		BOOL OpenMp3Stream(char* mem_stream, DWORD size,  int WaveBufferLengthMs);
		BOOL OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType,int WaveBufferLengthMs);
		BOOL OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType,  int WaveBufferLengthMs, DWORD seekfromstart, DWORD dwSize);
		void Close();
		BOOL Play();
		BOOL PlayLoop(unsigned int start_s, unsigned int length_s, unsigned int n_times);
		BOOL PlayLoopM(unsigned int start_ms, unsigned int length_ms, unsigned int n_times);
		BOOL Seek(unsigned int seconds);
		BOOL SeekM(unsigned int milliseconds);
		BOOL Stop();
		BOOL Pause();
		BOOL Resume();

		MP3_TIME* GetPositionHMS();
		unsigned int GetPosition();
		unsigned int GetPositionM();

		void InternalVolume(BOOL fEnable);
		void SetVolume(unsigned int lvolume, unsigned int rvolume);
		void GetVolume(unsigned int* lvolume, unsigned int* rvolume);

		char* GetMpegVersion();
		char* GetMpegLayer();
		char* GetChannelMode();
		char* GetEmphasis();


		unsigned int GetSamplingRate();
		unsigned int GetHeaderStart();
		unsigned int  GetSongLength();
		unsigned int  GetSongLengthM();
		unsigned int GetFileBitrate();
		unsigned int GetCurrentBitrate();
		unsigned int GetAvgBitrate();
		MP3_STATUS* GetStatus();
		BOOL IsVBR() { return c_vbr;}
		void MixChannels(BOOL fEnable, unsigned int left_percent, unsigned int right_percent);
		void GetVUData(unsigned int* left, unsigned int* right);
		void VocalCut(BOOL fEnable);

		void FadeOutM(unsigned int length_ms);
		void FadeOut(unsigned int length_seconds);
		void FadeIn(unsigned int length_seconds);
		void FadeInM(unsigned int length_ms);

		void EnableEQ(BOOL fEnable, BOOL fExternal);
		void SetEQParam(BOOL fExternal, int iPreAmpValue, EQ_PARAM  values);
		BOOL LoadID3(int id3Version);

		char *GetArtist();
		char *GetTrackTitle();
		char *GetTrackNum();
		char *GetAlbum();
		char *GetYear();
		char *GetComment();
		char *GetGenre();

		BOOL SetSfxParam(BOOL fEnable, int iMode,  int uDelayMs, int InputGain, int EchoGain, int OutputGain);
		int GetSfxMode() { return c_echo_mode; };

		BOOL IsPlaying() const { return c_play; }

	private:

		struct mad_stream c_stream;
		struct mad_frame c_frame; 
		struct mad_synth c_synth; 

		unsigned char* c_stream_start;
		unsigned int c_stream_size;
		unsigned char* c_stream_end;   
		unsigned char* c_start;
		unsigned char* c_end;
		unsigned char* c_position;
		unsigned int c_size;     
		BOOL c_ID3v1;
		BOOL c_ID3v2;

		unsigned int c_old_sampling_rate;

		unsigned int c_sampling_rate;
		int c_layer;
		int c_mode;
		int c_channel;
		int c_emphasis;
		int c_mode_extension;
		unsigned int c_bitrate;
		float c_avg_bitrate;
		unsigned int c_current_bitrate;
		unsigned int c_frame_counter;
		unsigned int c_bitrate_sum;	
		unsigned int c_nPosition;		 		
		MP3_TIME c_hms_pos;

		float c_avg_frame_size;	 
		mad_timer_t c_duration;
		unsigned int c_sample_per_frame;
		int c_flags;
		unsigned int c_mpeg_version;
		char* c_mpeg_version_str;
		char* c_channel_str;
		char* c_emphasis_str;
		char* c_layer_str;

		unsigned int c_xing_frame_size;
		struct tag_xl xing;
		BOOL c_valid_xing;

		unsigned int c_song_length_samples;
		unsigned int c_song_length_ms;
		unsigned int c_song_length_bytes;
		unsigned int c_song_length_frames;	 		

		unsigned int c_input_buffer_size;	 		
		unsigned char* c_tmp;			



		unsigned int c_play;
		unsigned int c_vbr;
		BOOL c_pause;	 

		DWORD c_dwPos;
		DWORD c_dwIndex;
		BOOL c_vudata_ready;  		
		BOOL c_bLoop;
		unsigned int c_nLoopCount;
		unsigned int c_nLoop;
		unsigned int c_nLoopFrameNum;
		unsigned int c_nLoopFramePlayed;
		BOOL c_bLoopIgnoreFrame;
		unsigned int c_dLoopPositionCorrector;  		

		BOOL c_bFadeOut;
		BOOL c_bFadeIn;
		unsigned int c_nFadeOutFrameNum;
		unsigned int c_nFadeInFrameNum;
		double c_fadeOutStep;
		double c_fadeInStep;
		unsigned int c_fade_frame_count;
		mad_fixed_t fadeout_gain[2];

		BOOL c_bVocalCut;  

		BOOL c_echo;
		real c_rIgOg;
		real c_rEgOg;
		unsigned int c_delay;
		int c_old_echo_buffer_size;
		int c_echo_alloc_buffer_size;
		SFX_MODE c_sfx_mode_l;
		SFX_MODE c_sfx_mode_r;
		int c_echo_mode;
		int c_sfx_offset;
		short* sfx_buffer[ECHO_BUFFER_NUMBER];
		short* sfx_buffer1[ECHO_BUFFER_NUMBER];  		

		REAL c_lbands[18];
		REAL c_rbands[18];
		paramlist c_paramroot;
		int c_eq[18];
		mad_fixed_t	c_EqFilter[32];
		BOOL c_bUseInternalEQ;
		BOOL c_bUseExternalEQ;
		BOOL c_bEQIn;	
		BOOL c_bChangeEQ;
		char* c_gapless_buffer;
		unsigned int c_byte_to_skip;

		BOOL c_internalVolume;
		short c_lvolume;
		short c_rvolume;  		

		mad_fixed_t gain[2];  

		BOOL c_mix_channels;
		mad_fixed_t c_l_mix;
		mad_fixed_t c_r_mix;

		AUTODITHER_STRUCT left_dither;
		AUTODITHER_STRUCT right_dither;


		HANDLE w_hFile;
		HANDLE w_hFileMap;
		unsigned char* w_mapping;	


		WWaveOut* w_wave;
		HANDLE w_hThread;
		DWORD w_hThreadID;
		HANDLE w_hEvent;
		CRITICAL_SECTION c_wcsThreadDone;
		CRITICAL_SECTION c_wcsSfx;  		


		ID3STRUCT w_myID3;
		ID3v2FRAME w_ID3frame;  

		MP3_STATUS c_status;

		void ClearSfxBuffers();
		static void  DoEcho(WMp3* mp3, short* buf, unsigned int len);  		
		static BOOL _LoadID3(WMp3* mp3,int id3Version);
		static char* _RetrField(WMp3* mp3, char *identifier, DWORD* Size);

		static void _ThreadFunc(void* lpdwParam);
		static void CALLBACK _WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
			DWORD dwParam1, DWORD dwParam2);

		static int reallocate_sfx_buffers(WMp3* mp3);

		static unsigned long _Get32bits(unsigned char *buf);

		static DWORD _RemoveLeadingNULL( char *str , DWORD size);
		static BOOL _DecodeFrameHeader(WMp3* mp3, char *header, char *identifier);
		static DWORD _DecodeUnsync(unsigned char* header, DWORD size);

		char _lpErrorMessage[ERRORMESSAGE_SIZE];
		void error(char* err_message);

		static void init(WMp3* mp3);	 
		static int _OpenMp3(WMp3* mp3, int WaveBufferLength);

	};
}

#endif

