/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123


#include "../AnyPlay.h"
#include "AudioSystem.h"
#include "wmp3decoder.h"
#include "wmp3.h"
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4244)

namespace AnyPlay
{


#define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
	(((y) + (1L << 15)) >> 16))


#define MAX_ERROR_STR MAX_PATH * 2


# define RGAIN_REFERENCE  83		 

#define SKIP_ID3TAG_AT_BEGINNING 1


# define RGAIN_SET(rgain)	((rgain)->name != RGAIN_NAME_NOT_SET)

# define RGAIN_VALID(rgain)  \
	(((rgain)->name == RGAIN_NAME_RADIO ||  \
	(rgain)->name == RGAIN_NAME_AUDIOPHILE) &&  \
	(rgain)->originator != RGAIN_ORIGINATOR_UNSPECIFIED)

# define RGAIN_DB(rgain)  ((rgain)->adjustment / 10.0)

	void rgain_parse(struct rgain *, struct mad_bitptr *);
	char const *rgain_originator(struct rgain const *);

	unsigned short crc_compute(char const *data, unsigned int length, unsigned short init);

	enum
	{
		TAG_XING = 0x0001,
		TAG_LAME = 0x0002,
		TAG_VBRI = 0x0004,
		TAG_VBR  = 0x0100
	};

	enum
	{
		TAG_XING_FRAMES = 0x00000001L,
		TAG_XING_BYTES  = 0x00000002L,
		TAG_XING_TOC    = 0x00000004L,
		TAG_XING_SCALE  = 0x00000008L
	};


	enum
	{
		TAG_LAME_NSPSYTUNE    = 0x01,
		TAG_LAME_NSSAFEJOINT  = 0x02,
		TAG_LAME_NOGAP_NEXT   = 0x04,
		TAG_LAME_NOGAP_PREV   = 0x08,

		TAG_LAME_UNWISE       = 0x10
	};



	void tag_init(struct tag_xl *);

# define tag_finish(tag)	 

	int tag_parse(struct tag_xl *, struct mad_stream const *, struct mad_frame const *);


	void mono_filter(struct mad_frame *frame);
	void gain_filter(struct mad_frame *frame, mad_fixed_t gain[2]);
	void experimental_filter(struct mad_frame *frame);

	void mix_filter(struct mad_frame *frame, mad_fixed_t left_p, mad_fixed_t right_p);
	void vocal_cut_filter(struct mad_frame *frame);



	void alert(char* str)
	{


	}
	void alert(unsigned char* str)
	{

	}

	void alert(int num)
	{


	}

	void alert(unsigned int num)
	{


	}

	void alert(float num)
	{
	}


	void alert(double num)
	{
	}


	void alerti(int num)
	{


	}

	void alertn(unsigned int num)
	{


	}

	void alertf(float num)
	{


	}

	void beep()
	{
	}


#define AVG_SINE_16 23169.0  
#define SINGLE_AUDIO_BUFFER_SIZE 100   
#define SEARCH_DEEP_VBR 200  

#define MIN_FRAME_SIZE 24
#define MAX_FRAME_SIZE 5761  


	const char empty_string[2] = "\0";


	void RemoveEndingSpaces(char* buff);	
	unsigned long Get32bits(unsigned char *buf);


#define NUM_SFX_MODE 10
#define MAX_DELAY 1000  

	SFX_MODE sfx_mode[NUM_SFX_MODE * 2] =
	{
		{0.5,0.2 ,0.1 ,0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0, 0, 0}, 
		{0.5,0.2 ,0.1 ,0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0, 0, 0, 0, 0, 0},  
		{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0.2,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
		{0.5,0.3,0.1,0.3,0.2,0.1,0.2,0.1,0.3,0.2,0.1,0.1,0.05,0.03,0.2,0.1,0.05,0.1,0.02,0.01},
		{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
		{0, 0, 0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.02},
		{0.5,0.3,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0,0.2,0.15,0.1,0.07,0.05},
		{0.5,0.1,0,0,0,0,0,0,0,0.5,0.2,0.1,0,0,0,0,0,0.3,0.2,0.04},
		{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
		{0.5,0.2,0.1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1},
		{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
		{0.3,0.1,0.2,0.05,0.1,0.05,0.05,0.025,0.1,0.05,0.1,0.05,0.1,0.01,0.05,0.01,0.1,0.01,0.1,0.01},
		{0.5,0.4,0.2,0.1,0.025,0,0,0,0,0,0,0,0,0,0,0.3,0.2,0.1,0.05,0.01},
		{0,0,0,0.5,0.2,0.1,0.05,0,0,0,0,0,0,0,0,0,0.3,0.2,0.05,0.01},
		{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
		{0.5,0.3,0.2,0.1,0.05,0.025,0.017,0.005,0,0,0,0,0,0,0,0,0,0.1,0.05,0.025},
		{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01},
		{0.5,0.2,0.1,0.3,0.2,0.1,0.2,0.1,0.05,0.1,0.05,0.025,0.1,0.05,0.05,0.025,0.1,0.05,0.025,0.01}

	};

	const char *sGenres [] =
	{
		"Blues",
		"Classic Rock",
		"Country",
		"Dance",
		"Disco",
		"Funk",
		"Grunge",
		"Hip-Hop",
		"Jazz",
		"Metal",
		"New Age",
		"Oldies",
		"Other",
		"Pop",
		"R&B",
		"Rap",
		"Reggae",
		"Rock",
		"Techno",
		"Industrial",
		"Alternative",
		"Ska",
		"Death Metal",
		"Pranks",
		"Soundtrack",
		"Euro-Techno",
		"Ambient",
		"Trip Hop",
		"Vocal",
		"Jazz+Funk",
		"Fusion",
		"Trance",
		"Classical",
		"Instrumental",
		"Acid",
		"House",
		"Game",
		"Sound Clip",
		"Gospel",
		"Noise",
		"Alternative Rock",
		"Bass",
		"Soul",
		"Punk",
		"Space",
		"Meditative",
		"Instrumental Pop",
		"Instrumental Rock",
		"Ethnic",
		"Gothic",
		"Darkwave",
		"Techno-Industrial",
		"Electronic",
		"Pop-Folk",
		"Eurodance",
		"Dream",
		"Southern Rock",
		"Comedy",
		"Cult",
		"Gangsta Rap",
		"Top 40",
		"Christian Rap",
		"Pop/Punk",
		"Jungle",
		"Native American",
		"Cabaret",
		"New Wave",
		"Phychedelic",
		"Rave",
		"Showtunes",
		"Trailer",
		"Lo-Fi",
		"Tribal",
		"Acid Punk",
		"Acid Jazz",
		"Polka",
		"Retro",
		"Musical",
		"Rock & Roll",
		"Hard Rock",
		"Folk",
		"Folk/Rock",
		"National Folk",
		"Swing",
		"Fast-Fusion",
		"Bebob",
		"Latin",
		"Revival",
		"Celtic",
		"Blue Grass",
		"Avantegarde",
		"Gothic Rock",
		"Progressive Rock",
		"Psychedelic Rock",
		"Symphonic Rock",
		"Slow Rock",
		"Big Band",
		"Chorus",
		"Easy Listening",
		"Acoustic",
		"Humour",
		"Speech",
		"Chanson",
		"Opera",
		"Chamber Music",
		"Sonata",
		"Symphony",
		"Booty Bass",
		"Primus",
		"Porn Groove",
		"Satire",
		"Slow Jam",
		"Club",
		"Tango",
		"Samba",
		"Folklore",
		"Ballad",
		"power Ballad",
		"Rhythmic Soul",
		"Freestyle",
		"Duet",
		"Punk Rock",
		"Drum Solo",
		"A Capella",
		"Euro-House",
		"Dance Hall",
		"Goa",
		"Drum & Bass",
		"Club-House",
		"Hardcore",
		"Terror",
		"indie",
		"Brit Pop",
		"Negerpunk",
		"Polsk Punk",
		"Beat",
		"Christian Gangsta Rap",
		"Heavy Metal",
		"Black Metal",
		"Crossover",
		"Comteporary Christian",
		"Christian Rock",
		"Merengue",
		"Salsa",
		"Trash Metal",
		"Anime",
		"JPop",
		"Synth Pop"
	};



#define GetFourByteSyncSafe(value1, value2, value3, value4) (((value1 & 255) << 21) | ((value2 & 255) << 14) | ((value3 & 255) << 7) | (value4 & 255))
#define CreateFourByteField(value1, value2, value3, value4) (((value1 & 255) << 24) | ((value2 & 255) << 16) | ((value3 & 255) << 8) | (value4 & 255))
#define ExtractBits(value, start, length) (( value >> start ) & (( 1 << length ) -1 ))


	WMp3::WMp3()
	{

		c_mix_channels = FALSE;


		gain[0] = MAD_F_ONE;
		gain[1] = MAD_F_ONE;


		w_hFile = INVALID_HANDLE_VALUE;	 	w_hFileMap = NULL;

		c_bUseInternalEQ = FALSE;
		c_internalVolume = TRUE;


		c_bEQIn = FALSE;

		c_bChangeEQ = FALSE;

		int n;

		c_bUseExternalEQ = FALSE;  
		for (n = 0; n < 18; n++)
		{
			c_lbands[n] = 1.0;
			c_rbands[n]= 1.0;
		};


		for(n = 0; n < 32; n++)
		{
			c_EqFilter[n] = MAD_F_ONE;

		}


		c_bVocalCut = FALSE;  

		c_echo = 0;  	c_echo_mode = 0;  
		c_delay = 0;	 
		c_rIgOg = 1.0;
		c_rEgOg = 1.0;


		for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
			sfx_buffer[i] = 0;


		c_echo_alloc_buffer_size = 0;
		c_old_echo_buffer_size = 0;  				



		c_dwPos = 0;
		c_dwIndex = 0;
		c_vudata_ready = FALSE;



		c_lvolume = 100;
		c_rvolume = 100;



		InitializeCriticalSection(&c_wcsThreadDone);
		InitializeCriticalSection(&c_wcsSfx);

		w_wave = new WWaveOut();  

		w_hThread = 0;
		w_hEvent = 0;  

		w_myID3.album = (char*) malloc(2);
		*w_myID3.album = 0;
		w_myID3.artist = (char*) malloc(2);
		*w_myID3.artist = 0;
		w_myID3.comment = (char*) malloc(2);
		*w_myID3.comment = 0;
		w_myID3.genre = (char*) malloc(2);
		*w_myID3.genre = 0;
		w_myID3.title = (char*) malloc(2);
		*w_myID3.title = 0;
		w_myID3.year = (char*) malloc(2);
		*w_myID3.year = 0;
		w_myID3.tracknum = (char*) malloc(2);
		*w_myID3.tracknum = 0;

		c_old_sampling_rate = 0;

		c_gapless_buffer = 0;


		equ_init(14);

		init(this);

	}


	WMp3::~WMp3()
	{

		Close();

		if(w_myID3.album)
			free(w_myID3.album);

		if(w_myID3.artist)
			free(w_myID3.artist);

		if(w_myID3.comment)
			free(w_myID3.comment);

		if(w_myID3.title)
			free(w_myID3.title);

		if(w_myID3.year)
			free(w_myID3.year);

		if(w_myID3.tracknum)
			free(w_myID3.tracknum);

		if(w_myID3.genre)
			free(w_myID3.genre);

		if(w_hEvent)
			CloseHandle(w_hEvent);

		for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
			if(sfx_buffer[i])
				free(sfx_buffer[i]);

		DeleteCriticalSection(&c_wcsThreadDone);
		DeleteCriticalSection(&c_wcsSfx);

		if(w_wave) delete w_wave;

		equ_quit();

		if(c_gapless_buffer)
			free(c_gapless_buffer);

	}


	BOOL WMp3::OpenMp3File(LPCTSTR filename, int WaveBufferLengthMs)
	{
		return OpenMp3File(filename,WaveBufferLengthMs,0,0);
	}


	int WMp3::OpenMp3File(LPCTSTR filename, int WaveBufferLengthMs, DWORD seek, DWORD size)
	{
		if(c_stream_start)
		{
			error("WMp3::OpenMp3File->You need to close mp3 file before open new one");
			return 0;
		}

		w_hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);	
		if(w_hFile ==  INVALID_HANDLE_VALUE)
		{
			sprintf(_lpErrorMessage,"WMp3::OpenMp3File->Can't open file: \" %s \"",
				filename);
			return 0;	
		}

		c_stream_size = GetFileSize(w_hFile,NULL);
		if(c_stream_size == 0xFFFFFFFF)
		{
			sprintf(_lpErrorMessage,"WMp3::OpenMp3File->Can't open file: \" %s \"",
				filename);
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;
			c_stream_size = 0;
			return 0;

		}


		if(seek >= c_stream_size)
		{
			error("WMp3::OpenMp3File->Seek value out of range");
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;
			c_stream_size = 0;
			return 0;


		}

		if(size > (c_stream_size - seek))
		{
			error("WMp3::OpenMp3File->Size value out of range");
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;
			c_stream_size = 0;
			return 0;
		}

		w_hFileMap = CreateFileMapping(w_hFile,NULL,PAGE_READONLY,0,0,NULL);
		if(w_hFileMap == NULL)
		{
			error("WMp3::OpenMp3File->Can't create file mapping");
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;
			c_stream_size = 0;
			return 0;

		}

		w_mapping = (unsigned char*) MapViewOfFile(w_hFileMap,FILE_MAP_READ,0,0,0);
		if(w_mapping == NULL)
		{
			error("WMp3::OpenMp3File->Can't create file view");
			CloseHandle(w_hFileMap);
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;
			c_stream_size = 0;
			return 0;

		}

		c_stream_start = w_mapping + seek;
		if(size) c_stream_size = size;

		c_stream_end = c_stream_start + c_stream_size - 1;

		return _OpenMp3(this,WaveBufferLengthMs);
	}



	BOOL WMp3::Stop()
	{
		if(!c_start)
		{
			error("WMp3::Stop->Stream isn't open!");
			return 0;
		}

		if(!c_play)
			return FALSE;

		c_vudata_ready = FALSE;



		c_bLoop  = FALSE;

		w_wave->Pause();

		c_play = FALSE;
		c_pause = FALSE;

		w_wave->Stop();  	
		SetEvent(w_hEvent);


		EnterCriticalSection(&c_wcsThreadDone);
		LeaveCriticalSection(&c_wcsThreadDone);
		return TRUE;
	}


	BOOL WMp3::Play()
	{


		if(!c_start)
		{
			error("WMp3::Play->Strean isn't open!");
			return 0;
		}

		if(c_pause)
			return Resume();

		if(c_play)
			return FALSE;


		EnterCriticalSection(&c_wcsThreadDone);
		LeaveCriticalSection(&c_wcsThreadDone);


		c_bLoop = FALSE;  
		c_pause = FALSE;  

		c_vudata_ready = FALSE;
		c_dwPos = 0;
		c_dwIndex = 0;

		c_play = TRUE;  

		if(!w_hThread)
			w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFunc, (void*) this,0,&w_hThreadID);



		if( w_hThread == NULL)
		{
			error("WMp3::Play->Can't create decoding thread!");
			return FALSE;
		}


		return TRUE;
	}



	void WMp3::_ThreadFunc(void* lpdwParam)
	{

		WMp3* mp3 = (WMp3*) lpdwParam;


		EnterCriticalSection(&mp3->c_wcsThreadDone);


		mp3->c_current_bitrate = 0;	 	mp3->c_frame_counter = 0;	 	mp3->c_bitrate_sum = 0;		 

		mp3->c_bEQIn = FALSE;


		unsigned int size;


		if(mp3->c_size < (unsigned int) ( (mp3->c_position - mp3->c_start + MAD_BUFFER_GUARD)))
		{
			LeaveCriticalSection(&mp3->c_wcsThreadDone);
			mp3->error("WMp3::_ThreadFunc->Stream too small");
			return;

		}

		if(mp3->c_position + MAD_BUFFER_GUARD > mp3->c_end)
		{
			LeaveCriticalSection(&mp3->c_wcsThreadDone);
			return;
		}

		size = mp3->c_size - (mp3->c_position - mp3->c_start + MAD_BUFFER_GUARD);

		unsigned int i;




		if(mp3->c_tmp)
			LocalFree(mp3->c_tmp);

		mp3->c_tmp = 0;


		struct mad_stream *stream = &mp3->c_stream;
		struct mad_frame *frame = &mp3->c_frame; 
		struct mad_synth *synth = &mp3->c_synth; 


		int skip_byte = mp3->c_byte_to_skip;

		unsigned int frame_count = 0;
		int output_size = 0;
		unsigned int output_index = 0;
		unsigned int frame_count_sum = 0;

		char* output_buf = mp3->w_wave->GetBufferData(output_index);

		AUTODITHER_STRUCT *left_dither = &mp3->left_dither;
		AUTODITHER_STRUCT *right_dither = &mp3->right_dither;
		ZeroMemory(left_dither, sizeof(AUTODITHER_STRUCT));
		ZeroMemory(right_dither, sizeof(AUTODITHER_STRUCT));

		register mad_fixed_t *left_ch;
		register mad_fixed_t *right_ch;
		register mad_fixed_t sample;
		register mad_fixed_t output;

		mad_fixed_t random;

		unsigned int nchannels, nsamples1;


		if ( !mp3->w_wave->OpenAudioOutput(WAVE_MAPPER, (DWORD)  mp3->_WaveOutProc, (DWORD)  mp3, CALLBACK_FUNCTION))
		{
			LeaveCriticalSection(&mp3->c_wcsThreadDone);
			mp3->error("WMp3::_ThreadFunc->Can't open wave output");
			return;
		}

		for( i = 0; i < mp3->w_wave->GetNumOfBuffers(); i++)
		{
			mp3->w_wave->SetBufferDone(i);
			mp3->w_wave->UnprepareHeader(i);
		}

		mad_stream_init(stream);
		mad_frame_init(frame);
		mad_synth_init(synth);


		EnterCriticalSection(&mp3->c_wcsSfx);

		if(mp3->c_echo)
			mp3->ClearSfxBuffers();

		LeaveCriticalSection(&mp3->c_wcsSfx);

		if(mp3->c_bUseExternalEQ)
			equ_clearbuf(mp3->c_sampling_rate);


		mad_stream_buffer(stream, mp3->c_position, size);

		while(mp3->c_play)
		{
			if(mad_frame_decode(frame, stream))
			{
				if(MAD_RECOVERABLE(stream->error))  					continue;

				if(stream->error == MAD_ERROR_BUFLEN)
				{
					if(mp3->c_tmp)
					{
						LocalFree(mp3->c_tmp);
						mp3->c_tmp = 0;

						if(mp3->c_bLoop)
						{
							if(mp3->c_nLoop < mp3->c_nLoopCount)
							{
								mp3->c_nLoop++;
								mp3->c_nLoopFramePlayed = 0;
								mad_stream_buffer(stream, mp3->c_position, size);
								continue;
							}
							else
								break;

						}
						else
							break;  	
					}
					else
					{
						unsigned int len = mp3->c_end + 1 - stream->this_frame;	
						mp3->c_tmp = (unsigned char*) LocalAlloc(0, len + MAD_BUFFER_GUARD + 1);	


						if(!mp3->c_tmp)
							break;

						CopyMemory (mp3->c_tmp, stream->this_frame, len);
						FillMemory (mp3->c_tmp + len,MAD_BUFFER_GUARD,0);
						mad_stream_buffer(stream, mp3->c_tmp, len + MAD_BUFFER_GUARD);
						continue;
					}
				}

			}


			if(mp3->c_bLoop)
			{ 
				if(mp3->c_nLoop < mp3->c_nLoopCount)
				{
					if(mp3->c_nLoopFramePlayed >= mp3->c_nLoopFrameNum)
					{
						mp3->c_nLoop++;

						mp3->c_nLoopFramePlayed = 0;
						mad_stream_buffer(stream, mp3->c_position, size);

					}		
				}
				else
					break;


				mp3->c_nLoopFramePlayed++;	
			}


			if(mp3->c_mix_channels)
				mix_filter(frame, mp3->c_l_mix, mp3->c_r_mix);



			if(mp3->c_bUseInternalEQ)
			{

				int	Channel;
				int	Sample;
				int	Samples;
				int	SubBand;



				Samples = MAD_NSBSAMPLES(&frame->header);



				if(frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
				{
					for(Channel = 0; Channel < 2; Channel++)
						for(Sample = 0; Sample < Samples; Sample++)
							for(SubBand = 0;SubBand < 32; SubBand++) 
								frame->sbsample[Channel][Sample][SubBand]=
								mad_f_mul(frame->sbsample[Channel][Sample][SubBand],
								mp3->c_EqFilter[SubBand]);

				}
				else
				{


					for(Sample = 0;Sample < Samples; Sample++)
						for(SubBand = 0;SubBand < 32; SubBand++)
							frame->sbsample[0][Sample][SubBand]=
							mad_f_mul(frame->sbsample[0][Sample][SubBand],
							mp3->c_EqFilter[SubBand]);
				}
			}



			if(mp3->c_bVocalCut)
				vocal_cut_filter(frame);



			if(mp3->c_internalVolume)
			{
				gain_filter(frame,mp3->gain);

			}

			if(mp3->c_bFadeOut)
			{
				if(mp3->c_fade_frame_count < mp3->c_nFadeOutFrameNum)
				{ 
					mp3->fadeout_gain[0] = MAD_F_ONE - mad_f_tofixed(((double) mp3->c_fade_frame_count * mp3->c_fadeOutStep) / 100.0);
					mp3->fadeout_gain[1] = mp3->fadeout_gain[0];
					gain_filter(frame,mp3->fadeout_gain);
					mp3->c_fade_frame_count++;
				}
				else
					mad_frame_mute(frame);
			}


			if(mp3->c_bFadeIn)
			{
				if(mp3->c_fade_frame_count < mp3->c_nFadeInFrameNum)
				{ 
					mp3->fadeout_gain[0] = mad_f_tofixed(((double) mp3->c_fade_frame_count * mp3->c_fadeInStep) / 100.0);
					mp3->fadeout_gain[1] = mp3->fadeout_gain[0];
					gain_filter(frame,mp3->fadeout_gain);
					mp3->c_fade_frame_count++;

				}
				else
					mp3->c_bFadeIn = FALSE;	
			}



			mp3->c_current_bitrate = frame->header.bitrate;
			mp3->c_frame_counter++;
			mp3->c_bitrate_sum += mp3->c_current_bitrate / 1000;


			mad_synth_frame(synth, frame);

			if(!mp3->c_play)
				break;


			nchannels = synth->pcm.channels;
			nsamples1  = synth->pcm.length;
			left_ch   = synth->pcm.samples[0];
			right_ch  = synth->pcm.samples[1];

			if(nchannels == 2)
			{
				while (nsamples1--)
				{

					sample = *left_ch++;


					sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];

					left_dither->error[2] = left_dither->error[1];
					left_dither->error[1] = left_dither->error[0] / 2;

					output = sample + 4096;

					random  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

					output += (random & 8191) - (left_dither->random & 8191);

					if (output >= MAD_F_ONE)
						output = MAD_F_ONE - 1;
					else if (output < -MAD_F_ONE)
						output = -MAD_F_ONE;

					output &= ~8191;

					left_dither->error[0] = sample - output;


					output_buf[0] = (char) (output >> 13);
					output_buf[1] = (char) (output >> 21);

					sample = *right_ch++;


					sample += right_dither->error[0] - right_dither->error[1] + right_dither->error[2];

					right_dither->error[2] = right_dither->error[1];
					right_dither->error[1] = right_dither->error[0] / 2;

					output = sample + 4096;


					random  = ((unsigned long) right_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

					output += (random & 8191) - (right_dither->random & 8191);

					if (output >= MAD_F_ONE)
						output = MAD_F_ONE - 1;
					else if (output < -MAD_F_ONE)
						output = -MAD_F_ONE;

					output &= ~8191;

					right_dither->error[0] = sample - output;

					output_buf[2] = (char) (output >> 13);
					output_buf[3] = (char) (output >> 21);

					output_buf += 4;


				}

				output_size += synth->pcm.length * 4;

			}

			else
			{
				while (nsamples1--)
				{


					sample = *left_ch++;


					sample += left_dither->error[0] - left_dither->error[1] + left_dither->error[2];

					left_dither->error[2] = left_dither->error[1];
					left_dither->error[1] = left_dither->error[0] / 2;

					output = sample + 4096;


					random  = ((unsigned long) left_dither->random * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;

					output += (random & 8191) - (left_dither->random & 8191);

					if (output >= MAD_F_ONE)
						output = MAD_F_ONE - 1;
					else if (output < -MAD_F_ONE)
						output = -MAD_F_ONE;




					output &= ~8191;

					left_dither->error[0] = sample - output;

					output_buf[0] = (char) (output >> 13);
					output_buf[1] = (char) (output >> 21);
					output_buf += 2;

				}

				output_size += synth->pcm.length * 2;

			}


			frame_count++;
			frame_count_sum++;

			if(frame_count == mp3->c_input_buffer_size)
			{

				if(output_size)
				{



					if(mp3->c_echo)
					{
						if(!mp3->c_play)
							break;


						EnterCriticalSection(&mp3->c_wcsSfx);

						int size = output_size;
						int buf_size = size / 2;
						char* buf = (char *) mp3->w_wave->GetBufferData(output_index);


						DoEcho(mp3, (short*) buf, buf_size);
						DoEcho(mp3, (short*) ( buf + buf_size), buf_size);


						LeaveCriticalSection(&mp3->c_wcsSfx);						
					}




					if(mp3->c_bUseExternalEQ)
					{
						if(!mp3->c_play)
							break;

						mp3->c_bEQIn = TRUE;

						int samples_num;
						if(mp3->c_channel == 2) 
							samples_num = output_size / 4;
						else
							samples_num = output_size / 2;

						if(mp3->c_bChangeEQ)
						{
							equ_makeTable(mp3->c_lbands,mp3->c_rbands,&mp3->c_paramroot, (REAL) mp3->c_sampling_rate);
							mp3->c_old_sampling_rate = mp3->c_sampling_rate;
							mp3->c_bChangeEQ = FALSE;	
						}




						if(skip_byte)
						{
							if(skip_byte > output_size)
							{  						

								equ_modifySamples((char *)mp3->w_wave->GetBufferData(output_index),
									samples_num, mp3->c_channel);


								skip_byte -= output_size;
								output_buf = mp3->w_wave->GetBufferData(output_index);
								output_size = 0;
								frame_count = 0;

								continue;

							}
							else
							{  						
								equ_modifySamples((char *)mp3->w_wave->GetBufferData(output_index),
									samples_num, mp3->c_channel);

								char *data = mp3->w_wave->GetBufferData(output_index);
								output_size -= skip_byte;
								MoveMemory(data, data + skip_byte, output_size);
								skip_byte = 0;  


							}
						}
						else
						{  					

							equ_modifySamples((char *)mp3->w_wave->GetBufferData(output_index),
								samples_num, mp3->c_channel);



						}
					}
					else
					{  			
						if(mp3->c_bEQIn)
						{
							mp3->c_bEQIn = FALSE;

							int samples_num;
							if(mp3->c_channel == 2) 
								samples_num = output_size / 4;
							else
								samples_num = output_size / 2;




							CopyMemory (mp3->c_gapless_buffer,
								mp3->w_wave->GetBufferData(output_index),
								output_size);


							int o_size = output_size;
							skip_byte = mp3->c_byte_to_skip;

							while(skip_byte)
							{


								equ_modifySamples(mp3->w_wave->GetBufferData(output_index),
									samples_num, mp3->c_channel);


								if(skip_byte > o_size)
								{
									skip_byte -= o_size;

								}
								else
								{
									o_size = skip_byte;
									skip_byte = 0;
								}

								mp3->w_wave->SetBufferSize(output_index, o_size);
								mp3->w_wave->PlayBuffer(output_index);



								output_index++;
								if(output_index == mp3->w_wave->GetNumOfBuffers())
									output_index = 0;


								output_buf = mp3->w_wave->GetBufferData(output_index);



								while(!mp3->w_wave->IsBufferDone(output_index))
								{
									if(!mp3->c_play)
										break;
									WaitForSingleObject(mp3->w_hEvent, INFINITE);

								}


							}

							CopyMemory(output_buf, mp3->c_gapless_buffer, output_size);
							skip_byte = mp3->c_byte_to_skip;	
						}

					}





					if(!mp3->c_play)
						break;

					mp3->w_wave->SetBufferSize(output_index, output_size);

					mp3->w_wave->PlayBuffer(output_index);

					output_index++;
					if(output_index == mp3->w_wave->GetNumOfBuffers())
						output_index = 0;


					output_buf = mp3->w_wave->GetBufferData(output_index);
					output_size = 0;
					frame_count = 0;


					while(!mp3->w_wave->IsBufferDone(output_index))
					{
						if(!mp3->c_play)
							break;
						WaitForSingleObject(mp3->w_hEvent, INFINITE);

					}




					mp3->w_wave->UnprepareHeader(output_index);


				}
				else
				{
					output_buf = mp3->w_wave->GetBufferData(output_index);
					frame_count = 0;

				}	
			}


		}  

		if(mp3->c_bEQIn && mp3->c_play)
		{

			int out_size = mp3->c_input_buffer_size * mp3->c_sample_per_frame *
				2 * mp3->c_channel;

			unsigned int sum_samples = frame_count_sum * mp3->c_sample_per_frame;

			int samples_num = mp3->c_input_buffer_size * mp3->c_sample_per_frame;

			int have_byte = sum_samples * 2 * mp3->c_channel;


			skip_byte = mp3->c_byte_to_skip;

			if(have_byte < skip_byte)
				skip_byte = have_byte;


			while(!mp3->w_wave->IsBufferDone(output_index))
			{
				if(!mp3->c_play)
					break;		
				WaitForSingleObject(mp3->w_hEvent, INFINITE);

			}


			while(skip_byte)
			{  		
				if(skip_byte > out_size)
				{
					skip_byte -= out_size;

				}
				else
				{
					out_size = skip_byte;
					skip_byte = 0;
				}


				equ_modifySamples(mp3->w_wave->GetBufferData(output_index),
					samples_num, mp3->c_channel);

				mp3->w_wave->SetBufferSize(output_index, out_size);
				mp3->w_wave->PlayBuffer(output_index);

				output_index++;
				if(output_index == mp3->w_wave->GetNumOfBuffers())
					output_index = 0;


				while(!mp3->w_wave->IsBufferDone(output_index))
				{
					if(!mp3->c_play)
						break;
					WaitForSingleObject(mp3->w_hEvent, INFINITE);

				}
			}

		}



		if(mp3->c_tmp)
			LocalFree(mp3->c_tmp);

		mp3->c_tmp = 0;



		for(int j = 0; j < (int) mp3->w_wave->GetNumOfBuffers(); j++ )
		{
			if(mp3->w_wave->IsBufferDone(j))
			{
				mp3->w_wave->UnprepareHeader(j);
			}
			else
			{
				if(!mp3->c_play)
					break;
				j--;
				WaitForSingleObject(mp3->w_hEvent,SINGLE_AUDIO_BUFFER_SIZE);
			}
		}

		mp3->w_wave->Stop();

		mp3->w_wave->Close();

		mp3->c_bFadeOut = FALSE;
		mp3->c_bFadeIn = FALSE;


		mp3->c_play = FALSE;
		mp3->c_pause = FALSE;


		mp3->c_vudata_ready = FALSE;
		mp3->c_dwPos = 0;  	mp3->c_dwIndex = 0;  	


		mad_stream_finish(stream);
		mad_frame_finish(frame);
		mad_synth_finish(synth);


		mp3->c_current_bitrate = 0;
		mp3->c_frame_counter = 0;
		mp3->c_bitrate_sum = 0;


		mp3->c_position = mp3->c_start;


		mp3->c_nPosition = 0;


		mp3->c_bLoop = FALSE;

		mp3->w_hThread = 0;
		LeaveCriticalSection(&mp3->c_wcsThreadDone);
// 		if(mp3->GetMgrPtr())
// 		{
// 			mp3->GetMgrPtr()->Remove(mp3);
// 		}
		return;	
	}



	unsigned int WMp3::GetPosition()
	{	
		return ( GetPositionM() / 1000 );
	}

	unsigned int WMp3::GetPositionM()
	{
		if(!c_start)
		{
			error("WMp3::GetPosition->Strean isn't open!");
			return 0;
		}




		unsigned int pos;

		if(c_bLoop)
		{
			if(c_nLoop)
			{
				pos = (unsigned int) w_wave->GetPosition(SAMPLES);
				if(pos >= ( c_dLoopPositionCorrector * c_nLoop))
				{
					pos -= ( c_dLoopPositionCorrector * c_nLoop);

				}
				else
				{
					pos -= ( c_dLoopPositionCorrector * ( c_nLoop - 1));

				}

				return ( c_nPosition + (unsigned int) ( (double) pos * 1000.0 / (double) c_sampling_rate) );

			}
		}

		pos = w_wave->GetPosition(MILLISECONDS);
		return ( c_nPosition + pos );




	}




	BOOL WMp3::LoadID3(int id3Version)
	{

		if(!c_start)
		{
			error("WMp3::LoadID3->Strean isn't open!");
			return 0;
		}

		return _LoadID3(this, id3Version);


	}


	BOOL WMp3::_LoadID3(WMp3* mp3,int id3Version)
	{

		switch(id3Version)
		{

		case ID3_VERSION1:
			{
				if(mp3->w_myID3.album)
				{
					free(mp3->w_myID3.album);
					mp3->w_myID3.album = 0;
				}

				mp3->w_myID3.album = (char*) malloc(31);
				*mp3->w_myID3.album = 0;

				if(mp3->w_myID3.artist)
				{
					free(mp3->w_myID3.artist);
					mp3->w_myID3.artist = 0;
				}

				mp3->w_myID3.artist = (char*) malloc(31);
				*mp3->w_myID3.artist = 0;

				if(mp3->w_myID3.comment)
				{
					free(mp3->w_myID3.comment);
					mp3->w_myID3.comment = 0;
				}

				mp3->w_myID3.comment = (char*) malloc(31);
				*mp3->w_myID3.comment = 0;

				if(mp3->w_myID3.title)
				{
					free(mp3->w_myID3.title);
					mp3->w_myID3.title = 0;
				}

				mp3->w_myID3.title = (char*) malloc(31);
				*mp3->w_myID3.title = 0;

				if(mp3->w_myID3.year)
				{
					free(mp3->w_myID3.year);
					mp3->w_myID3.year = 0;
				}

				mp3->w_myID3.year = (char*) malloc(5);
				*mp3->w_myID3.year = 0;

				if(mp3->w_myID3.tracknum)
				{
					free(mp3->w_myID3.tracknum);
					mp3->w_myID3.tracknum = 0;
				}

				mp3->w_myID3.tracknum = (char*) malloc(4);
				*mp3->w_myID3.tracknum = 0;

				if(mp3->w_myID3.genre)
				{
					free(mp3->w_myID3.genre);
					mp3->w_myID3.genre = 0;
				}

				mp3->w_myID3.genre = (char*) malloc(1);
				*mp3->w_myID3.genre = 0;


				if(!mp3->c_ID3v1)
				{  	
					mp3->error("WMp3::LoadID3->There is no ID3v1 tag"); 
					return FALSE;
				}


				memcpy(mp3->w_myID3.title,mp3->c_stream_end - 124 , 30);
				mp3->w_myID3.title[30] = 0;
				RemoveEndingSpaces(mp3->w_myID3.title);

				memcpy(mp3->w_myID3.artist,mp3->c_stream_end - 94, 30);
				mp3->w_myID3.artist[30] = 0;
				RemoveEndingSpaces(mp3->w_myID3.artist);

				memcpy(mp3->w_myID3.album,mp3->c_stream_end - 64, 30);
				mp3->w_myID3.album[30] = 0;
				RemoveEndingSpaces(mp3->w_myID3.album);

				memcpy(mp3->w_myID3.year,mp3->c_stream_end - 34, 4);
				mp3->w_myID3.year[4] = 0;
				RemoveEndingSpaces(mp3->w_myID3.year);

				memcpy(mp3->w_myID3.comment,mp3->c_stream_end - 30, 30);
				mp3->w_myID3.comment[30] = 0;

				memcpy(mp3->w_myID3.genre, mp3->c_stream_end,1);

				if( (unsigned char) mp3->w_myID3.genre[0] > 146)
				{
					*mp3->w_myID3.genre = 0;
				}
				else
				{
					unsigned char tmp = mp3->w_myID3.genre[0]; 
					free(mp3->w_myID3.genre);
					if(tmp > 147) tmp = 0;
					mp3->w_myID3.genre = (char*) malloc(strlen(sGenres[(BYTE) tmp]) + 1);
					strcpy(mp3->w_myID3.genre,sGenres[(BYTE) tmp]); 
				}



				BYTE tracknum = 0;
				if( mp3->w_myID3.comment[28] == 0 )
					tracknum = mp3->w_myID3.comment[29];
				sprintf( mp3->w_myID3.tracknum, "%u", tracknum);



			}
			return TRUE;


		case ID3_VERSION2:
			{

				if(!mp3->c_ID3v2)
				{
					mp3->error("WMp3::LoadID3->There is no ID3v2 tag");
					return FALSE;
				}

				char	tmpHeader[10];
				tmpHeader[0] = 0;
				DWORD dwRead;


				memcpy( mp3->w_myID3.Id3v2.Header,mp3->c_stream_start, 10);


				if(memcmp( mp3->w_myID3.Id3v2.Header, "ID3", 3) != 0)
				{
					mp3->error("WMp3::LoadID3->There is no ID3v2 tag");

					return FALSE;
				}


				mp3->w_myID3.Id3v2._flags			= ( mp3->w_myID3.Id3v2.Header[5] & 255);
				mp3->w_myID3.Id3v2.Flags.Unsync = ExtractBits ( mp3->w_myID3.Id3v2._flags , 7, 1);
				mp3->w_myID3.Id3v2.Flags.Extended = ExtractBits ( mp3->w_myID3.Id3v2._flags , 6, 1);
				mp3->w_myID3.Id3v2.Flags.Experimental = ExtractBits ( mp3->w_myID3.Id3v2._flags , 5, 1);
				mp3->w_myID3.Id3v2.Flags.Footer = ExtractBits ( mp3->w_myID3.Id3v2._flags , 4, 1);

				mp3->w_myID3.Id3v2.Size			= GetFourByteSyncSafe( mp3->w_myID3.Id3v2.Header[6], mp3->w_myID3.Id3v2.Header[7], mp3->w_myID3.Id3v2.Header[8], mp3->w_myID3.Id3v2.Header[9]);


				if(mp3->w_myID3.album) free(mp3->w_myID3.album);
				mp3->w_myID3.album = 0;
				mp3->w_myID3.album		= _RetrField(mp3, "TALB", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.album, dwRead);


				if(mp3->w_myID3.artist) free( mp3->w_myID3.artist);
				mp3->w_myID3.artist = 0;
				mp3->w_myID3.artist		= _RetrField(mp3, "TPE1", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.artist, dwRead);


				if(mp3->w_myID3.comment) free(mp3->w_myID3.comment);
				mp3->w_myID3.comment = 0;
				mp3->w_myID3.comment	= _RetrField(mp3, "COMM", &dwRead);
				DWORD newsize = _RemoveLeadingNULL( mp3->w_myID3.comment, dwRead);

				if(newsize < 2)
				{
					free(mp3->w_myID3.comment);
					mp3->w_myID3.comment = 0;
					mp3->w_myID3.comment = (char*) malloc(2);
					strcpy(mp3->w_myID3.comment,"");
				}
				else
				{
					char* tmp = ( char* ) memchr(mp3->w_myID3.comment , '\0', newsize - 1);
					if(tmp)
					{
						strcpy(mp3->w_myID3.comment, tmp +1);
					}
				}


				if(mp3->w_myID3.genre) free(mp3->w_myID3.genre);
				mp3->w_myID3.genre = 0;
				mp3->w_myID3.genre		= _RetrField(mp3, "TCON", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.genre, dwRead);


				if(mp3->w_myID3.title) free(mp3->w_myID3.title);
				mp3->w_myID3.title = 0;
				mp3->w_myID3.title		= _RetrField(mp3, "TIT2", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.title, dwRead);

				if(mp3->w_myID3.tracknum) free( mp3->w_myID3.tracknum);
				mp3->w_myID3.tracknum = 0;
				mp3->w_myID3.tracknum	= _RetrField(mp3, "TRCK", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.tracknum, dwRead);

				if(mp3->w_myID3.year) free(mp3->w_myID3.year);
				mp3->w_myID3.year = 0;
				mp3->w_myID3.year		= _RetrField(mp3, "TYER", &dwRead);
				_RemoveLeadingNULL( mp3->w_myID3.year, dwRead);


			}
			return TRUE;


		}

		return FALSE;

	}




	char* WMp3::_RetrField(WMp3* mp3, char *identifier, DWORD* Size)
	{



		char tmpHeader[10];

		DWORD framesize;
		unsigned char *field;



		DWORD dwSize = mp3->w_myID3.Id3v2.Size;
		unsigned char* pos = mp3->c_stream_start + 10;


		if( mp3->w_myID3.Id3v2.Flags.Extended )
		{
			memcpy( tmpHeader, pos, 4);
			pos += 4;
			DWORD extHdrSize = GetFourByteSyncSafe( tmpHeader[0], tmpHeader[1], tmpHeader[2], tmpHeader[3]);
			pos += extHdrSize;
			dwSize -= extHdrSize;

		}


		*Size = 0;
		BOOL ret;

		while(1)
		{

			memcpy(tmpHeader,pos, 10);
			pos += 10;

			if(tmpHeader[0] == 0 || dwSize <= 0)
			{
				char* tmp = (char*) malloc(2);
				tmp[0] = 0;
				return tmp;
			}
			ret = _DecodeFrameHeader(mp3, tmpHeader, identifier);
			if(  ret )
			{

				field = (unsigned char*) malloc(mp3->w_ID3frame.Size + 1);

				field[mp3->w_ID3frame.Size] = 0;



				memcpy((char*) field, pos, mp3->w_ID3frame.Size);
				pos += mp3->w_ID3frame.Size;

				if( mp3->w_ID3frame.Flags.Unsynchronisation )
					*Size = _DecodeUnsync(field, mp3->w_ID3frame.Size);
				else
					*Size = mp3->w_ID3frame.Size;


				return  (char *) field;
			}
			else
			{
				if( mp3->w_ID3frame.Flags.Unsynchronisation )
					framesize = GetFourByteSyncSafe(tmpHeader[4], tmpHeader[5], tmpHeader[6], tmpHeader[7]);

				else
					framesize = CreateFourByteField(tmpHeader[4], tmpHeader[5], tmpHeader[6], tmpHeader[7]);


				pos += framesize;
				dwSize -= ( framesize + 10 );
			}
		}

		char* tmp = (char*) malloc(2);
		tmp[0] = 0;

		return tmp;

	}


	DWORD WMp3::_RemoveLeadingNULL( char *str , DWORD size)
	{
		if(!str)
			return 0;
		for ( DWORD i = 0; i < size; i++ )
		{
			if( str[i] > 0 )
			{
				for( DWORD j = 0; j < size - i; j++ )
				{
					str[j] = str[i+j];
				}
				for( DWORD k = size - i; k < size; k++)
					str[k] = 0;

				return( size - i );
			}
		}

		return 0;
	}




	BOOL WMp3::_DecodeFrameHeader(WMp3* mp3, char *header, char *identifier)
	{
		mp3->w_ID3frame.Flags.Tag_alter_preservation = ExtractBits(header[8], 6,1);
		mp3->w_ID3frame.Flags.File_alter_preservation = ExtractBits(header[8], 5,1);
		mp3->w_ID3frame.Flags.Read_only = ExtractBits(header[8], 4,1);
		mp3->w_ID3frame.Flags.Grouping_identity = ExtractBits(header[9], 6,1);
		mp3->w_ID3frame.Flags.Compression = ExtractBits(header[9], 3,1);
		mp3->w_ID3frame.Flags.Encryption = ExtractBits(header[9], 2,1);
		mp3->w_ID3frame.Flags.Unsynchronisation = ExtractBits(header[9], 1,1);
		mp3->w_ID3frame.Flags.Data_length_indicator = ExtractBits(header[9], 0,1);


		mp3->w_ID3frame.Size = GetFourByteSyncSafe(header[4], header[5], header[6], header[7]);

		CreateFourByteField(header[4], header[5], header[6], header[7]);  
		if( strncmp(header, identifier, 4) != 0 )
			return FALSE;
		if(ExtractBits(header[9], 7,1) || ExtractBits(header[9], 5,1) || ExtractBits(header[9], 4,1) )
			return FALSE;

		return TRUE;


	}

	DWORD WMp3::_DecodeUnsync(unsigned char* header, DWORD size)
	{
		DWORD count = 0;
		for( UINT i = 0; i < size - 2; i++ )
		{
			if(header[i] ==  0xFF && header[i+1] == 0 )
			{
				header[i+1] = header[i+2];
				count++;
			}
		}
		return count;
	}




	char* WMp3::GetMpegVersion()
	{
		return c_mpeg_version_str;
	}

	char* WMp3::GetMpegLayer()
	{
		return c_layer_str;
	}

	char* WMp3::GetChannelMode()
	{
		return c_channel_str;
	}

	char* WMp3::GetEmphasis()
	{
		return c_emphasis_str;
	}

	unsigned int WMp3::GetSamplingRate()
	{
		return c_sampling_rate;

	}

	unsigned int WMp3::GetHeaderStart()
	{
		return (c_start - c_stream_start);

	}





	unsigned long Get32bits(unsigned char *buf)
	{

		unsigned long ret;
		ret = (((unsigned long) buf[0]) << 24) |
			(((unsigned long) buf[1]) << 16) |
			(((unsigned long) buf[2]) << 8) |
			((unsigned long) buf[3]) ;

		return ret;
	}



	BOOL WMp3::Pause()
	{
		if(!c_start)
		{
			error("WMp3::Pause->Strean isn't open!");
			return 0;
		}

		w_wave->Pause();
		c_pause = TRUE;
		return TRUE;
	}

	BOOL WMp3::Resume()
	{
		if(!c_start)
		{
			error("WMp3::Pause->Strean isn't open!");
			return 0;
		}

		w_wave->Resume();
		c_pause = FALSE;
		return TRUE;
	}


	BOOL WMp3::Seek(unsigned int seconds)
	{
		return SeekM(seconds * 1000 );	
	}


	BOOL WMp3::SeekM(unsigned int milliseconds)
	{


		if(!c_start)
		{
			error("WMp3::SeekM->Strean isn't open!");
			return 0;
		}


		if(c_play)
		{
			Stop();
		}


		if(milliseconds  > c_song_length_ms)
			milliseconds = c_song_length_ms;

		if(milliseconds == 0)
		{


		}



		if(c_valid_xing)
		{
			if ((xing.flags & TAG_XING) && (xing.xing.flags & TAG_XING_TOC))
			{

				double pa, pb, px;
				double percentage = 100.0 * (double) milliseconds / (double) c_song_length_ms;

				int perc = (int) percentage;
				if (perc > 99) perc = 99;
				pa = xing.xing.toc[perc];
				if (perc < 99)
				{
					pb = xing.xing.toc[perc+1];
				}
				else
				{
					pb = 256;
				}

				px = pa + (pb - pa) * (percentage - perc);
				c_position = c_start  + (unsigned int) (((double) ( c_size + c_xing_frame_size )   / 256.0) * px);



			}
			else
			{
				c_position = c_start + (unsigned int) ((double) milliseconds / (double) c_song_length_ms * (double) c_size); 

			}




		}
		else
		{
			c_position = c_start + (unsigned int) ((double) milliseconds / (double) c_song_length_ms * (double) c_size); 


		}


		c_nPosition = milliseconds;


		return TRUE;

	}













	unsigned int WMp3::GetCurrentBitrate()
	{

		if(!c_start)
			return 0;

		return c_current_bitrate / 1000;

	}


	unsigned int WMp3::GetAvgBitrate()
	{

		if(!c_start)
			return 0;

		if(c_frame_counter == 0)
			return 0;

		return c_bitrate_sum / c_frame_counter;

	}

	MP3_STATUS* WMp3::GetStatus()
	{

		memset(&c_status,0, sizeof(MP3_STATUS));

		c_status.stop = TRUE;


		if(c_play)
		{
			c_status.play = TRUE;
			c_status.stop = FALSE;
		}

		if(c_pause)
		{
			c_status.pause = TRUE;
			c_status.play = FALSE;
			c_status.stop = FALSE;
		}


		c_status.echo = c_echo;
		c_status.echo_mode = c_echo_mode;
		c_status.eq_external = c_bUseExternalEQ; 
		c_status.eq_internal = c_bUseInternalEQ;
		c_status.vocal_cut = c_bVocalCut;
		c_status.channel_mix = c_mix_channels;
		c_status.fade_in = c_bFadeIn;
		c_status.fade_out = c_bFadeOut;
		c_status.internal_volume = c_internalVolume;
		c_status.loop = c_bLoop;


		return &c_status;

	}






	unsigned int WMp3::GetSongLength()
	{ 
		return GetSongLengthM() / 1000;
	}

	unsigned int WMp3::GetSongLengthM()
	{ 
		return c_song_length_ms; 
	}


	unsigned int WMp3::GetFileBitrate()
	{ 
		if(c_vbr)
			return (unsigned int) c_avg_bitrate / 1000;

		return c_bitrate / 1000; 

	}

	char* WMp3::GetArtist()		{ return w_myID3.artist;}
	char* WMp3::GetTrackTitle()	{ return w_myID3.title;}
	char* WMp3::GetTrackNum()		{ return w_myID3.tracknum;}
	char* WMp3::GetAlbum()		{ return w_myID3.album;}
	char* WMp3::GetYear()			{ return w_myID3.year;}
	char* WMp3::GetComment()		{ return w_myID3.comment;}



	char* WMp3::GetGenre()
	{ 
		if(w_myID3.genre)
			return w_myID3.genre;
		else
			return (char*) empty_string;
	};


















	BOOL WMp3::OpenMp3Stream(char* memw_stream, DWORD size, int WaveBufferLength)
	{

		if(c_stream_start)
		{
			error("WMp3::OpenMp3Stream->You need to close mp3 stream before open new one");
			return FALSE;
		}

		if(!memw_stream)
		{
			error("WMp3::OpenMp3Stream->Can't open NULL stream!");
			return FALSE;
		}

		c_stream_start = (unsigned char*) memw_stream;
		c_stream_size = size;
		c_stream_end = c_stream_start +  c_stream_size - 1;

		return  _OpenMp3(this, WaveBufferLength);

	}


	void WMp3::Close()
	{
		if(!c_start) return;

		Stop();

		w_wave->Uninitialize();

		CloseHandle(w_hEvent);
		w_hEvent = 0;




		if(w_hFile != INVALID_HANDLE_VALUE)
		{
			UnmapViewOfFile(w_mapping);
			w_mapping = 0;
			CloseHandle(w_hFileMap);
			w_hFileMap = NULL;
			CloseHandle(w_hFile);
			w_hFile = INVALID_HANDLE_VALUE;

		}

		if(c_gapless_buffer)
			free(c_gapless_buffer);

		c_gapless_buffer = 0;

		init(this);



	}



	BOOL WMp3::OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType, int WaveBufferLength)
	{

		return OpenMp3Resource(hModule,lpName,lpType,WaveBufferLength, 0,0);
	}


	BOOL WMp3::OpenMp3Resource(HMODULE hModule,LPCTSTR lpName, LPCTSTR lpType, int WaveBufferLength, DWORD seek, DWORD size)
	{

		if(c_stream_start)
		{
			error("WMp3::OpenMp3Resource->You need to close mp3 stream before open new one");
			return FALSE;
		}

		HRSRC hrSrc = FindResource(hModule,lpName,lpType);
		if(!hrSrc)
		{
			error("WMp3::OpenMp3Resource->Can't find resource!");
			return FALSE;

		}

		HGLOBAL hRes = LoadResource(hModule,hrSrc);

		if(!hRes)
		{
			error("WMp3::OpenMp3Resource->Can't load resource!");
			return FALSE;

		}

		char* res = (char*) LockResource(hRes);

		if(!res)
		{
			error("WMp3::OpenMp3Resource->Can't lock resource");
			return FALSE;

		}

		c_stream_size = SizeofResource(hModule, hrSrc);
		if(c_stream_size == 0)
		{
			error("WMp3::OpenMp3Resource->Can't get resource size!");
			return FALSE;

		}

		if(seek >= c_stream_size)
		{
			error("WMp3::OpenMp3Resource->Seek value out of range");
			c_stream_size = 0;
			return 0;


		}

		if(size > (c_stream_size - seek))
		{
			error("WMp3::OpenMp3Resource->Size value out of range");
			c_stream_size = 0;
			return 0;
		}


		c_stream_start = (unsigned char*)  res;
		if(size)
			c_stream_size = size;

		c_stream_end = c_stream_start + c_stream_size - 1;

		return _OpenMp3(this, WaveBufferLength);


	}




	void WMp3::SetVolume(unsigned int lvolume, unsigned int rvolume)
	{

		if( lvolume > 100)
			lvolume = 100;

		if( rvolume > 100)
			rvolume = 100;

		if(c_internalVolume)
		{
			c_lvolume = (short) lvolume;
			c_rvolume = (short) rvolume;
			gain[0] = lvolume < 100 ? mad_f_tofixed(pow(10, (double) lvolume / 100) / 10.0 - 0.1) : MAD_F_ONE;
			gain[1] = rvolume < 100 ? mad_f_tofixed(pow(10, (double) rvolume / 100) / 10.0 - 0.1) : MAD_F_ONE;

		}
		else
		{

			unsigned int l = (unsigned int) (lvolume < 100 ? ((pow(10, (double) lvolume / 100) / 10.0 - 0.1) * 65535.0) : 65535);
			unsigned int r = (unsigned int) ( rvolume < 100 ? ((pow(10, (double) rvolume / 100) / 10.0 - 0.1) * 65535.0) : 65535);
			w_wave->SetVolume((unsigned short) l, (unsigned short) r);

		}

	}



	void WMp3::GetVolume(unsigned int* lvolume, unsigned int* rvolume)
	{

		if(c_internalVolume)
		{
			*lvolume = c_lvolume;
			*rvolume = c_rvolume;

		}
		else
		{
			w_wave->GetVolume((WORD*) lvolume, (WORD*) rvolume);
			*lvolume = *lvolume * 100 / 65535;
			*rvolume = *rvolume * 100 / 65535;
		}

	}





	void CALLBACK WMp3::_WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
		DWORD dwParam1, DWORD dwParam2)
	{

		WMp3* mp3 = (WMp3*) dwInstance;
		WAVEHDR* header = (WAVEHDR *) dwParam1;
		switch(uMsg)
		{
		case WOM_OPEN:
			break;

		case WOM_CLOSE:
			break;

		case WOM_DONE:
			{
				mp3->c_dwPos += header->dwBufferLength;

				if(header->dwUser == mp3->w_wave->GetNumOfBuffers() - 1)
					mp3->c_dwIndex = 0;
				else
					mp3->c_dwIndex = header->dwUser + 1;



				::SetEvent(mp3->w_hEvent);
				mp3->c_vudata_ready = TRUE;




			}
			break;



		}
	}




	void WMp3::error(char* err_message)
	{
		size_t size = 0;
		if(err_message != 0)
		{
			size = strlen(err_message );
			size = (size > 	ERRORMESSAGE_SIZE) ? ( ERRORMESSAGE_SIZE - 1) : size;
			strncpy(_lpErrorMessage, err_message, size);
			_lpErrorMessage[size] = 0;
		}
		_lpErrorMessage[size]= 0;

	}






	unsigned long WMp3::_Get32bits(unsigned char *buf)
	{

		unsigned long ret;
		ret = (((unsigned long) buf[0]) << 24) |
			(((unsigned long) buf[1]) << 16) |
			(((unsigned long) buf[2]) << 8) |
			((unsigned long) buf[3]) ;

		return ret;
	}



	void WMp3::GetVUData(unsigned int* left, unsigned int* right)
	{
		*left = 0;
		*right = 0;	

		if(!c_start || !c_play || !c_vudata_ready)
			return;



		short* left1;
		short* right1;
		real sum_left = 0;
		real avg_left;
		real sumww_right = 0;
		real avgww_right;
		real num = 0;
		real la;
		real ra;



		unsigned int pos = w_wave->GetPosition(BYTES);
		unsigned int buff_size = w_wave->GetBufferSize(c_dwIndex);

		char* data = w_wave->GetBufferData(c_dwIndex);

		unsigned int num_chunk = 2;

		unsigned int size = buff_size / num_chunk ;




		DWORD offset;


		if(c_channel == 2)
		{
			if(num_chunk >= 2)
			{

				offset = pos - c_dwPos;
				if(offset > buff_size)
				{
					if(c_dwIndex == w_wave->GetNumOfBuffers() - 1)
						c_dwIndex = 0;
					else
						c_dwIndex++;

					offset = 0;
					data = w_wave->GetBufferData(c_dwIndex);
				}

				if(offset + size + 4 > buff_size)
				{
					offset = buff_size - size - 4;


				}

			}
			else
			{
				offset = 0;
			}


			for(DWORD i = 0; i < size; i += 4)
			{
				left1 = (short*) ( data + i + offset);
				sum_left += (real) abs(*left1);
				right1 = (short*) ( data + i + offset + 2);
				sumww_right += (real) abs(*right1);
				num++;

			}

			avg_left = sum_left / num;
			avgww_right = sumww_right / num;



			if(avg_left < 21.61)
				*left = 0;
			else
			{
				la = avg_left / AVG_SINE_16;
				*left =  (unsigned int) (100 + (33.0 * log10(la)));
				if(*left > 100)
					*left = 100;

			}



			if(avgww_right < 21.61)
				*right = 0;
			else
			{
				ra = avgww_right / AVG_SINE_16 ;
				*right = (unsigned int) (100 + (33.0 * log10(ra)));
				if(*right > 100)
					*right = 100;
			}






		}
		else if(c_channel == 1)
		{
			if(num_chunk >= 2)
			{
				offset = pos - c_dwPos;
				if(offset > buff_size)
				{
					if(c_dwIndex == w_wave->GetNumOfBuffers() - 1)
						c_dwIndex = 0;
					else
						c_dwIndex++;

					offset = 0;
					data = w_wave->GetBufferData(c_dwIndex);
				}

				if(offset + size + 2 > buff_size)
				{
					offset = buff_size - size - 2;


				}

			}
			else
			{
				offset = 0;
			}


			for(DWORD i = 0; i < size; i += 2)
			{
				left1 = (short*) ( data + i + offset);
				sum_left += (real) abs(*left1);
				num++;
			}

			avg_left = sum_left / num;


			if(avg_left < 21.61)
			{
				*left = 0;
				*right = 0;
			}
			else
			{
				la = avg_left / AVG_SINE_16 ;
				*left = (unsigned int) (100 + (33 * log10(la)));
				if(*left > 100)
					*left = 100;

				*right = *left;
			}
		}

	}





	void RemoveEndingSpaces(char* buff)
	{
		int size = strlen(buff);
		for(int i = size - 1; i >= 0; i--)
		{
			if(buff[i] == ' ')
				buff[i] = 0;
			else
				return;

		}

	}



	void WMp3::DoEcho(WMp3* mp3, short* buf, unsigned int len)
	{


		unsigned int i;
		unsigned int j;
		unsigned int sample_num = len / 2;



		for( i = ECHO_BUFFER_NUMBER - 1; i > 0; i--)
		{
			mp3->sfx_buffer1[i] = mp3->sfx_buffer1[i - 1];

		} 

		mp3->sfx_buffer1[0] = mp3->sfx_buffer1[ECHO_BUFFER_NUMBER - 1];



		for(i = 0; i < sample_num; i++)
		{
			buf[i] /= 2;
		}



		CopyMemory(mp3->sfx_buffer1[0], buf, len);	




		real e;
		real f;
		int a;


		for(i = 0; i < sample_num; i++)
		{
			e = 0;
			f = 0;

			for(j = 0; j < ECHO_MAX_DELAY_NUMBER ; j++)
			{

				if(mp3->c_sfx_mode_l[j]) 
					e +=  (real) mp3->sfx_buffer1[j + mp3->c_sfx_offset][i] * mp3->c_sfx_mode_l[j];



				if(mp3->c_sfx_mode_r[j]) 
					f +=  (real) mp3->sfx_buffer1[j + mp3->c_sfx_offset][i + 1] * mp3->c_sfx_mode_r[j];

			}


			a = (int) (e * mp3->c_rEgOg + (real) buf[i] * mp3->c_rIgOg);

			if(a > 32767)
				a = 32767;

			if(a < -32768)
				a = -32768;

			buf[i] = (short) a;

			i++;

			a =  (int) (f * mp3->c_rEgOg + (real) buf[i] * mp3->c_rIgOg);

			if(a > 32767)
				a = 32767;

			if(a < -32768)
				a = -32768;

			buf[i] = (short) a;


		}

	}


	void WMp3::ClearSfxBuffers()
	{
		if(sfx_buffer[0] != NULL)
		{
			for(unsigned int i = 0; i < ECHO_BUFFER_NUMBER; i++)
				ZeroMemory(sfx_buffer[i], c_echo_alloc_buffer_size);

		}	

	}	



	BOOL WMp3::SetSfxParam(BOOL fEnable, int iMode, int uDelayMs, int InputGain, int EchoGain, int OutputGain)
	{

		c_echo = FALSE;

		EnterCriticalSection(&c_wcsSfx);


		if(uDelayMs < 0)
			uDelayMs = 0;

		if(uDelayMs > MAX_DELAY)
			uDelayMs = MAX_DELAY;



		if(iMode > NUM_SFX_MODE - 1)
			iMode = NUM_SFX_MODE - 1;	



		int i;

		int mode = iMode * 2;

		for(i = 0; i < ECHO_MAX_DELAY_NUMBER ; i++)
		{
			c_sfx_mode_l[i] =  sfx_mode[mode][i] ;
			if(c_channel > 1)
				c_sfx_mode_r[i] = sfx_mode[mode + 1][i];
			else
				c_sfx_mode_r[i] = c_sfx_mode_l[i];	
		}

		c_echo_mode = iMode;


		if(fEnable)
		{  		 		
			if(reallocate_sfx_buffers(this) == 0)
				return FALSE;



			if(InputGain < -12)
				InputGain = -12;

			if(InputGain > 12)
				InputGain = 12;

			if(EchoGain < -12)
				EchoGain = -12;

			if(EchoGain > 12)
				EchoGain = 12;

			if(OutputGain < -12)
				OutputGain = -12;

			if(OutputGain > 12)
				OutputGain = 12;



			c_rIgOg = pow(10.0, (double) ( (double) ( InputGain + OutputGain )  / 20));
			c_rEgOg = pow(10.0, (double) ( (double) ( EchoGain + OutputGain )  / 20));



			c_delay = uDelayMs;


			c_sfx_offset = ECHO_MAX_DELAY_NUMBER * c_delay / MAX_DELAY;




			ClearSfxBuffers();		

		}
		else
		{
			c_echo = fEnable;
			c_old_echo_buffer_size = 0;

			if(sfx_buffer[0])
			{
				for(int i = 0; i < ECHO_BUFFER_NUMBER; i++)
				{
					free(sfx_buffer[i]);
					sfx_buffer[i] = 0;
					sfx_buffer1[i] = 0;
				}
			}
		}



		LeaveCriticalSection(&c_wcsSfx);
		c_echo = fEnable;

		return TRUE;
	}



	void WMp3::EnableEQ(BOOL fEnable, BOOL fExternal)
	{



		if(fEnable)
		{
			if(fExternal)
			{  		
				c_bUseExternalEQ = TRUE;

			}
			else
			{  	
				c_bUseInternalEQ = TRUE;
			}
		}
		else
		{
			if(fExternal)
			{
				c_bUseExternalEQ = FALSE;
			}
			else
			{
				c_bUseInternalEQ = FALSE;
			}
		}


	}




	void WMp3::SetEQParam(BOOL fExternal, int iPreAmpValue, EQ_PARAM  values)
	{


		int i;
		double AmpFactor;
		double max = mad_f_todouble(MAD_F_MAX);


		if(iPreAmpValue < -12)
			iPreAmpValue = -12;

		if(iPreAmpValue > 12)
			iPreAmpValue = 12;

		for(i = 0; i < 10; i++)
		{
			if(values[i] < -12)
				values[i] = -12;

			if(values[i] > 12)
				values[i] = 12;
		}




		if(!fExternal)
		{  
			c_EqFilter[0] = values[0];
			c_EqFilter[1] = values[1];
			c_EqFilter[2] = values[2];
			c_EqFilter[3] = values[3];
			c_EqFilter[4] = values[4];
			c_EqFilter[5] = values[5];
			c_EqFilter[6] = values[5];
			c_EqFilter[7] = values[5];
			c_EqFilter[8] = values[6];
			c_EqFilter[9] = values[6];
			c_EqFilter[10] = values[6];
			c_EqFilter[11] = values[6];
			c_EqFilter[12] = values[6];
			c_EqFilter[13] = values[6];
			c_EqFilter[14] = values[7];
			c_EqFilter[15] = values[7];
			c_EqFilter[16] = values[7];
			c_EqFilter[17] = values[7];
			c_EqFilter[18] = values[7];
			c_EqFilter[19] = values[8];
			c_EqFilter[20] = values[8];
			c_EqFilter[21] = values[8];
			c_EqFilter[22] = values[8];
			c_EqFilter[23] = values[8];
			c_EqFilter[24] = values[9];
			c_EqFilter[25] = values[9];
			c_EqFilter[26] = values[9];
			c_EqFilter[27] = values[9];
			c_EqFilter[28] = values[9];
			c_EqFilter[29] = values[9];
			c_EqFilter[30] = values[9];
			c_EqFilter[31] = values[9];

			for(i = 0; i < 32; i++)
			{
				AmpFactor = pow(10.0, (double) ( (double) ( c_EqFilter[i] + iPreAmpValue )  / 20));
				if(AmpFactor > max)
					AmpFactor = max;

				c_EqFilter[i] = mad_f_tofixed(AmpFactor);

			}

			return;
		}


		c_eq[0] = values[0];
		c_eq[1] = values[0];
		c_eq[2] = values[0];
		c_eq[3] = values[1];
		c_eq[4] = values[1];
		c_eq[5] = values[2];
		c_eq[6] = values[2];
		c_eq[7] = values[3];
		c_eq[8] = values[3];
		c_eq[9] = values[4];
		c_eq[10] = values[4];
		c_eq[11] = values[5];
		c_eq[12] = values[5];
		c_eq[13] = values[6];
		c_eq[14] = values[7];
		c_eq[15] = values[8];
		c_eq[16] = values[9];
		c_eq[17] = values[9];


		for(i = 0; i < 18; i++)
		{
			AmpFactor = pow(10.0 , (double) ( (double) ( c_eq[i] + iPreAmpValue )  / 20));
			c_lbands[i] = (REAL) AmpFactor;
			c_rbands[i] = (REAL) AmpFactor;
		}

		c_bChangeEQ = TRUE;

	}



	void WMp3::VocalCut(BOOL fEnable)
	{
		c_bVocalCut = fEnable;

	}



	void WMp3::InternalVolume(BOOL fEnable)
	{
		c_internalVolume = fEnable;

	}



	int WMp3::_OpenMp3(WMp3* mp3, int WaveBufferLength)
	{
		unsigned char* tmp = 0;

		struct mad_stream stream;
		struct mad_frame frame;
		struct mad_header header;


		mp3->c_ID3v1 = FALSE;
		mp3->c_ID3v2 = FALSE;

		mp3->c_start = mp3->c_stream_start;
		mp3->c_end = mp3->c_stream_end;
		mp3->c_position = mp3->c_stream_start;
		mp3->c_size = mp3->c_stream_size;


		if(mp3->c_size > 128 && memcmp(mp3->c_end - 127,"TAG",3) == 0)
		{
			mp3->c_end -= 128;
			mp3->c_size -= 128;
			mp3->c_ID3v1 = TRUE;	
		}




		if(( mp3->c_size  > 10 ) && (memcmp(mp3->c_start,"ID3",3) == 0) &&
			mp3->c_start[6] < 0x80 && mp3->c_start[7] < 0x80 &&
			mp3->c_start[8] < 0x80 && mp3->c_start[9] < 0x80)
		{
			unsigned int id3v2_size	= GetFourByteSyncSafe( mp3->c_start[6], 
				mp3->c_start[7], mp3->c_start[8], mp3->c_start[9]);

			id3v2_size += 10;
			if(mp3->c_size >  ( id3v2_size + MIN_FRAME_SIZE) )
			{
				mp3->c_ID3v2 = TRUE;


				if(SKIP_ID3TAG_AT_BEGINNING && ( *(mp3->c_start + id3v2_size) == 0xFF) &&  (( *(mp3->c_start + id3v2_size + 1) & 0xE0) == 0xE0))
				{
					mp3->c_start += id3v2_size;
					mp3->c_size -= id3v2_size;


				}
			}
		}

		if(mp3->c_size < MAD_BUFFER_GUARD)
		{
			mp3->error("WMp3::_OpenMp3->Invalid stream!");
			mp3->init(mp3);
			return FALSE;
		}



		mad_stream_init(&stream);
		mad_frame_init(&frame);
		mad_header_init(&header);




		mad_stream_buffer(&stream, mp3->c_start, mp3->c_size - MAD_BUFFER_GUARD);

		unsigned char* first_frame = mp3->c_start;

		while(1)
		{
			while(mad_frame_decode(&frame, &stream))
			{
				if(MAD_RECOVERABLE(stream.error))
					continue;

				mp3->error("WMp3::_OpenMp3->Can't find first valid mp3 frame");
				mad_stream_finish(&stream);
				mad_frame_finish(&frame);
				mad_header_finish(&header);

				mp3->init(mp3);
				return FALSE;

			}

			first_frame =  (unsigned char*) stream.this_frame;



			mp3->c_sampling_rate = frame.header.samplerate;
			mp3->c_layer = frame.header.layer;
			mp3->c_mode = frame.header.mode;
			mp3->c_channel = ( frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
			mp3->c_emphasis = frame.header.emphasis;
			mp3->c_mode_extension = frame.header.mode_extension;
			mp3->c_bitrate = frame.header.bitrate;
			mp3->c_flags = frame.header.flags;
			mp3->c_avg_bitrate = 0;
			mp3->c_duration = frame.header.duration;
			mp3->c_sample_per_frame = (frame.header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152;



			if( (mp3->c_flags & MAD_FLAG_MPEG_2_5_EXT))
			{
				mp3->c_mpeg_version = MPEG25;
				mp3->c_mpeg_version_str = "MPEG Version 2.5";
			}
			else if((mp3->c_flags & MAD_FLAG_LSF_EXT))
			{
				mp3->c_mpeg_version = MPEG2;
				mp3->c_mpeg_version_str = "MPEG Version 2"; 
			}
			else
			{
				mp3->c_mpeg_version = MPEG1;
				mp3->c_mpeg_version_str = "MPEG Version 1";

			}

			switch(mp3->c_layer)
			{
			case MAD_LAYER_I:
				mp3->c_layer_str = "Layer I";
				break;

			case MAD_LAYER_II:
				mp3->c_layer_str =	"Layer II";
				break;

			case MAD_LAYER_III:
				mp3->c_layer_str =	"Layer III"; 
				break;

			default:
				mp3->c_layer_str =	"reserved"; 
				break;		
			}

			switch(mp3->c_mode)
			{
			case MAD_MODE_SINGLE_CHANNEL:
				mp3->c_channel_str = "Single channel (Mono)";
				break;

			case MAD_MODE_DUAL_CHANNEL:
				mp3->c_channel_str = "Dual channel (Stereo)";
				break;

			case MAD_MODE_JOINT_STEREO:
				mp3->c_channel_str = "Joint stereo (Stereo)";
				break;

			case MAD_MODE_STEREO:
				mp3->c_channel_str = "Stereo";
				break;
			}


			switch(mp3->c_emphasis)
			{
			case MAD_EMPHASIS_NONE:
				mp3->c_emphasis_str =  "none";
				break;

			case MAD_EMPHASIS_50_15_US:
				mp3->c_emphasis_str = "50/15 ms";
				break;

			case MAD_EMPHASIS_CCITT_J_17:
				mp3->c_emphasis_str = "CCIT J.17";
				break;

			case MAD_EMPHASIS_RESERVED:
				mp3->c_emphasis_str = "reserved"; 
				break;
			}


			if(mad_frame_decode(&frame, &stream))
			{
				if(MAD_RECOVERABLE(stream.error))
					continue;

				mp3->error("WMp3::_OpenMp3->Can't find first valid mp3 frame");
				mad_stream_finish(&stream);
				mad_frame_finish(&frame);
				mad_header_finish(&header);
				mp3->init(mp3);
				return FALSE;
			}
			else
			{
				if(mp3->c_sampling_rate != frame.header.samplerate ||
					mp3->c_layer != frame.header.layer)
					continue;

				break;	
			}		
		}




		mp3->c_size -= (first_frame - mp3->c_start);

		mp3->c_start = first_frame;



		if(mp3->c_size < MAD_BUFFER_GUARD)
		{
			mp3->error("WMp3::_OpenMp3->Can't find first valid mp3 frame");
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			mp3->init(mp3);
			return FALSE;
		}






		mp3->c_valid_xing = FALSE;

		mad_stream_buffer(&stream, mp3->c_start, mp3->c_size - MAD_BUFFER_GUARD);

		if(mad_frame_decode(&frame, &stream) == 0)
		{

			if(tag_parse(&mp3->xing, &stream, &frame) == 0)
			{
				if(mp3->xing.flags & TAG_XING)
				{
					if((mp3->xing.xing.flags & TAG_XING_FRAMES) && mp3->xing.xing.flags & TAG_XING_TOC)
					{
						mp3->c_song_length_frames = (unsigned int) mp3->xing.xing.frames;
						mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
						mp3->c_song_length_ms = (unsigned int) ( 1000.0 * (double) mp3->c_song_length_frames * (double) mp3->c_sample_per_frame / (double) mp3->c_sampling_rate);


						mp3->c_size -= ( stream.next_frame - mp3->c_start);
						mp3->c_start = (unsigned char*) stream.next_frame;

						mp3->c_song_length_bytes = mp3->c_size;
						mp3->c_avg_frame_size =(float) ( (double) mp3->c_song_length_bytes / (double) mp3->c_song_length_frames); 
						mp3->c_avg_bitrate = (float) ((double) mp3->c_song_length_bytes * 8 / (double) mp3->c_song_length_frames *  (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);


						mp3->c_xing_frame_size = stream.next_frame - stream.this_frame;		
						mp3->c_valid_xing = TRUE;


					}
				}	
			}
		}


		if(mp3->c_size < MAD_BUFFER_GUARD)
		{
			mp3->error("WMp3::_OpenMp3->Can't find first valid mp3 frame");
			mad_stream_finish(&stream);
			mad_frame_finish(&frame);
			mad_header_finish(&header);
			mp3->init(mp3);
			return FALSE;
		}


		if(1)
		{



			mad_stream_buffer(&stream, mp3->c_start, mp3->c_size);

			unsigned int frame_num = 0;
			mp3->c_vbr = 0;
			unsigned int size = 0;


			tmp = 0;
			while(frame_num < SEARCH_DEEP_VBR)
			{
				if(mad_header_decode(&header,&stream))
				{
					if(MAD_RECOVERABLE(stream.error))   					continue;



					if(stream.error == MAD_ERROR_BUFLEN)
					{
						if(tmp)
						{
							free(tmp);
							tmp = 0;
							break;
						}
						unsigned int len = mp3->c_end + 1 - stream.this_frame;	
						tmp = (unsigned char*) malloc(len + MAD_BUFFER_GUARD);	
						if(!tmp)
							break;

						memcpy(tmp, stream.this_frame, len);
						memset(tmp + len,0,MAD_BUFFER_GUARD);
						mad_stream_buffer(&stream, tmp, len + MAD_BUFFER_GUARD);
						continue;
					}

				}


				if(header.bitrate != mp3->c_bitrate)   				mp3->c_vbr = 1;	

				size += header.size;	
				frame_num++;

			}

			if(tmp )
				free(tmp);

			tmp = 0;


			if(!mp3->c_valid_xing && mp3->c_vbr)
			{
				mp3->c_song_length_frames = (unsigned int) ((double) frame_num * (double) mp3->c_size / (double) size);
				mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
				mp3->c_song_length_ms = (unsigned int) ( 1000.0 * (double) mp3->c_song_length_frames * (double) mp3->c_sample_per_frame / (double) mp3->c_sampling_rate);		
				mp3->c_song_length_bytes = mp3->c_size;
				mp3->c_avg_frame_size = (float) ((double) size / (double) frame_num);
				mp3->c_avg_bitrate = (float) ((double) mp3->c_song_length_bytes * 8 / (double) mp3->c_song_length_frames *  (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);	
			}
			else if(!mp3->c_valid_xing) 
			{
				mp3->c_avg_bitrate = (float) mp3->c_bitrate;
				double length =  (double) mp3->c_size * 8.0 / ( (double) mp3->c_avg_bitrate / 1000.0); 
				mp3->c_song_length_ms = (unsigned int) length;
				mp3->c_song_length_frames = (unsigned int) ceil(length / 1000 * (double) mp3->c_sampling_rate / (double) mp3->c_sample_per_frame);
				mp3->c_song_length_samples = mp3->c_song_length_frames * mp3->c_sample_per_frame;
				mp3->c_song_length_bytes = mp3->c_size;
				mp3->c_avg_frame_size = (float) ( (double) mp3->c_size / (double) mp3->c_song_length_frames);	

			}

		}






		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_header_finish(&header);



		if(WaveBufferLength < 200 )
		{
			mp3->error("WMp3::_Open->Buffer can't be smaller than 200 ms");
			mp3->init(mp3);
			return FALSE;
		}

		if( (mp3->w_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		{
			mp3->error("WMp3::_Open->Can't create event");
			mp3->init(mp3);
			return FALSE;
		}




		mp3->c_input_buffer_size = MulDiv(SINGLE_AUDIO_BUFFER_SIZE , mp3->c_sampling_rate , 1000 * mp3->c_sample_per_frame ); 




		if(mp3->c_input_buffer_size == 0)
		{
			mp3->error("WMp3::_Open->SINGLE_AUDIO_BUFFER_SIZE is too small!");
			CloseHandle(mp3->w_hEvent);
			mp3->init(mp3);
			return FALSE;

		}

		int single_audio_buffer_len = MulDiv(mp3->c_input_buffer_size, mp3->c_sample_per_frame * 1000, mp3->c_sampling_rate) + 1;



		int WaveBufferNum = MulDiv(WaveBufferLength,1 , single_audio_buffer_len - 1);


		if ( ! mp3->w_wave->Initialize(WAVE_FORMAT_PCM, (WORD)mp3->c_channel, mp3->c_sampling_rate,16,
			WaveBufferNum, single_audio_buffer_len))
		{

			mp3->error("WMp3::_Open->Can't initialize wave out");
			CloseHandle(mp3->w_hEvent);
			mp3->init(mp3);
			return FALSE;
		}



		mp3->c_echo_alloc_buffer_size = mp3->w_wave->GetBufferAllocSize() / 2 + 2;

		if(mp3->c_echo)
		{
			if(reallocate_sfx_buffers(mp3) == 0)
			{
				mp3->error("WMp3::_Open->Can't allocate echo buffer");
				CloseHandle(mp3->w_hEvent);
				mp3->init(mp3);
				return FALSE;
			}
		}


		mp3->c_gapless_buffer = (char*) malloc (mp3->w_wave->GetBufferAllocSize());
		if(!mp3->c_gapless_buffer)
		{
			mp3->error("WMp3::_Open->Can't allocate gapless buffer");
			CloseHandle(mp3->w_hEvent);
			mp3->init(mp3);
			return FALSE;

		}


		if(mp3->c_old_sampling_rate != mp3->c_sampling_rate)
			mp3->c_bChangeEQ = TRUE;




		int mode = mp3->c_echo_mode * 2;


		for(int i = 0; i < ECHO_MAX_DELAY_NUMBER ; i++)
		{
			mp3->c_sfx_mode_l[i] = sfx_mode[mode][i];
			if(mp3->c_channel > 1)
				mp3->c_sfx_mode_r[i] = sfx_mode[mode + 1][i];
			else
				mp3->c_sfx_mode_r[i] = mp3->c_sfx_mode_l[i];	
		}







		mp3->c_byte_to_skip = 	equ_latency() * 2 * mp3->c_channel;


		mp3->c_position = mp3->c_start;





		return TRUE;


	}















	BOOL WMp3::PlayLoop(unsigned int start_s, unsigned int length_s, unsigned int n_times)
	{
		return PlayLoopM(start_s * 1000, length_s * 1000,  n_times);

	}

	BOOL WMp3::PlayLoopM(unsigned int start_ms, unsigned int length_ms, unsigned int n_times)
	{


		if(!c_start)
		{
			error("WMp3::PlayLoopM->Strean isn't open!");
			return 0;
		}

		if(c_pause)
			return Resume();

		if(c_play)
			return FALSE;




		EnterCriticalSection(&c_wcsThreadDone);
		LeaveCriticalSection(&c_wcsThreadDone);




		if(!SeekM(start_ms))
		{
			error("WMp3::PlayLoopM->Can't seek to start of loop!");
			return FALSE;
		}


		c_bLoop = TRUE;  
		c_pause = FALSE;  

		c_vudata_ready = FALSE;
		c_dwPos = 0;
		c_dwIndex = 0;

		c_play = TRUE;  

		c_nLoopFrameNum = MulDiv(length_ms, c_sampling_rate, c_sample_per_frame * 1000);   	c_nLoopFramePlayed = 0;    	c_bLoop = TRUE;
		c_nLoopCount = n_times;
		c_nLoop = 0;


		if(length_ms > c_song_length_ms - start_ms)
			c_dLoopPositionCorrector = (unsigned int) (	(double) (c_song_length_ms - start_ms) / 1000.0 * (double) c_sampling_rate);
		else			
			c_dLoopPositionCorrector = (unsigned int) ((double) c_nLoopFrameNum * (double) c_sample_per_frame);




		if(!w_hThread)
			w_hThread = (HANDLE)  CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) _ThreadFunc, (void*) this,0,&w_hThreadID);



		if( w_hThread == NULL)
		{
			error("WMp3::PlayLoopM->Can't create decoding thread!");
			return FALSE;
		}	

		return TRUE;


	}





# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
# define INFO_MAGIC	(('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
# define LAME_MAGIC	(('L' << 24) | ('A' << 16) | ('M' << 8) | 'E')
# define VBRI_MAGIC	(('V' << 24) | ('B' << 16) | ('R' << 8) | 'I')

	static
		unsigned short const crc16_table[256] =
	{
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,

		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,

		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,

		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
	};

	unsigned short crc_compute(char const *data, unsigned int length,
		unsigned short init)
	{
		register unsigned int crc;

		for (crc = init; length >= 8; length -= 8)
		{
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
			crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		}

		switch (length)
		{
		case 7: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 6: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 5: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 4: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 3: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 2: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 1: crc = crc16_table[(crc ^ *data++) & 0xff] ^ (crc >> 8);
		case 0: break;
		}

		return (unsigned short) crc;
	}


	void tag_init(struct tag_xl *tag)
	{
		tag->flags      = 0;
		tag->encoder[0] = 0;
		memset(&tag->xing, 0, sizeof(struct tag_xing));
		memset(&tag->lame, 0, sizeof(struct tag_lame));
	}


	static
		int parse_xing(struct tag_xing *xing,
	struct mad_bitptr *ptr, unsigned int *bitlen)
	{
		if (*bitlen < 32)
			goto fail;

		xing->flags = mad_bit_read(ptr, 32);
		*bitlen -= 32;

		if (xing->flags & TAG_XING_FRAMES)
		{
			if (*bitlen < 32)
				goto fail;

			xing->frames = mad_bit_read(ptr, 32);
			*bitlen -= 32;
		}

		if (xing->flags & TAG_XING_BYTES)
		{
			if (*bitlen < 32)
				goto fail;

			xing->bytes = mad_bit_read(ptr, 32);
			*bitlen -= 32;
		}

		if (xing->flags & TAG_XING_TOC)
		{
			int i;

			if (*bitlen < 800)
				goto fail;

			for (i = 0; i < 100; ++i)
				xing->toc[i] = (unsigned char) mad_bit_read(ptr, 8);

			*bitlen -= 800;
		}

		if (xing->flags & TAG_XING_SCALE)
		{
			if (*bitlen < 32)
				goto fail;

			xing->scale = mad_bit_read(ptr, 32);
			*bitlen -= 32;
		}

		return 0;

fail:
		xing->flags = 0;
		return -1;
	}


	static
		int parse_lame(struct tag_lame *lame,
	struct mad_bitptr *ptr, unsigned int *bitlen,
		unsigned short crc)
	{
		struct mad_bitptr save = *ptr;
		unsigned long magic;
		char const *version;

		if (*bitlen < 36 * 8)
			goto fail;



		magic   = mad_bit_read(ptr, 4 * 8);
		version = (const char *) mad_bit_nextbyte(ptr);

		mad_bit_skip(ptr, 5 * 8);



		lame->revision = (unsigned char) mad_bit_read(ptr, 4);
		if (lame->revision == 15)
			goto fail;

		lame->vbr_method = (enum tag_lame_vbr) mad_bit_read(ptr, 4);



		lame->lowpass_filter = (unsigned short) ( mad_bit_read(ptr, 8) * 100);



		lame->peak = mad_bit_read(ptr, 32) << 5;



		rgain_parse(&lame->replay_gain[0], ptr);



		rgain_parse(&lame->replay_gain[1], ptr);

		if (magic == LAME_MAGIC)
		{
			char str[6];
			unsigned major = 0, minor = 0, patch = 0;
			int i;

			memcpy(str, version, 5);
			str[5] = 0;

			sscanf(str, "%u.%u.%u", &major, &minor, &patch);

			if (major == 3 && minor < 95)
			{
				for (i = 0; i < 2; ++i)
				{
					if (RGAIN_SET(&lame->replay_gain[i]))
						lame->replay_gain[i].adjustment += 6;   			}
			}
		}



		lame->flags    = (unsigned char) mad_bit_read(ptr, 4);
		lame->ath_type = (unsigned char) mad_bit_read(ptr, 4);



		lame->bitrate = (unsigned char) mad_bit_read(ptr, 8);



		lame->start_delay = mad_bit_read(ptr, 12);
		lame->end_padding = mad_bit_read(ptr, 12);



		lame->source_samplerate = (enum tag_lame_source) mad_bit_read(ptr, 2);

		if (mad_bit_read(ptr, 1))
			lame->flags |= TAG_LAME_UNWISE;

		lame->stereo_mode   = (enum tag_lame_mode) mad_bit_read(ptr, 3);
		lame->noise_shaping = (unsigned char) mad_bit_read(ptr, 2);



		lame->gain = (signed char) mad_bit_read(ptr, 8);



		mad_bit_skip(ptr, 2);

		lame->surround = (enum tag_lame_surround) mad_bit_read(ptr,  3);
		lame->preset   = (enum tag_lame_preset) mad_bit_read(ptr, 11);



		lame->music_length = mad_bit_read(ptr, 32);



		lame->music_crc = (unsigned short) mad_bit_read(ptr, 16);



		if (mad_bit_read(ptr, 16) != crc)
			goto fail;

		*bitlen -= 36 * 8;

		return 0;

fail:
		*ptr = save;
		return -1;
	}


	int tag_parse(struct tag_xl *tag, struct mad_stream const *stream, struct mad_frame const *frame)
	{
		struct mad_bitptr ptr = stream->anc_ptr;
		struct mad_bitptr start = ptr;
		unsigned int bitlen = stream->anc_bitlen;
		unsigned long magic;
		unsigned long magic2;
		int i;

		tag_init(tag);  
		if (bitlen < 32)
			return -1;

		magic = mad_bit_read(&ptr, 32);

		bitlen -= 32;

		if (magic != XING_MAGIC &&
			magic != INFO_MAGIC &&
			magic != LAME_MAGIC)
		{


			if (magic != ((XING_MAGIC << 16) & 0xffffffffL) &&
				magic != ((INFO_MAGIC << 16) & 0xffffffffL))
			{
				if (bitlen >= 400)
				{
					mad_bit_skip(&ptr, 256);
					magic2 = mad_bit_read(&ptr, 32);
					if (magic2 == VBRI_MAGIC)
					{
						mad_bit_skip(&ptr, 16);  						tag->lame.start_delay = mad_bit_read(&ptr, 16);  						mad_bit_skip(&ptr, 16);  						tag->xing.bytes = mad_bit_read(&ptr, 32);  						tag->xing.frames = mad_bit_read(&ptr, 32);  						unsigned int table_size = mad_bit_read(&ptr, 16);
						unsigned int table_scale = mad_bit_read(&ptr, 16);
						unsigned int entry_bytes = mad_bit_read(&ptr, 16);
						unsigned int entry_frames = mad_bit_read(&ptr, 16);
						{
							unsigned int Entry = 0, PrevEntry = 0, Percent, SeekPoint = 0, i = 0;
							float AccumulatedTime = 0;
							float TotalDuration = (float) (1000.0 * tag->xing.frames * ((frame->header.flags & MAD_FLAG_LSF_EXT) ? 576 : 1152) / frame->header.samplerate);
							float DurationPerVbriFrames = TotalDuration / ((float)table_size + 1);
							for (Percent=0;Percent<100;Percent++)
							{
								float EntryTimeInMilliSeconds = ((float)Percent/100) * TotalDuration;
								while (AccumulatedTime <= EntryTimeInMilliSeconds)
								{
									PrevEntry = Entry;
									Entry = mad_bit_read(&ptr, entry_bytes * 8) * table_scale;
									i++;
									SeekPoint += Entry;
									AccumulatedTime += DurationPerVbriFrames;
									if (i >= table_size) break;
								}
								unsigned int fraction = ( (int)(((( AccumulatedTime - EntryTimeInMilliSeconds ) / DurationPerVbriFrames ) 
									+ (1.0f/(2.0f*(float)entry_frames))) * (float)entry_frames));
								unsigned int SeekPoint2 = SeekPoint - (int)((float)PrevEntry * (float)(fraction) 
									/ (float)entry_frames);
								unsigned int XingPoint = (256 * SeekPoint2) / tag->xing.bytes;


								if (XingPoint > 255) XingPoint = 255;
								tag->xing.toc[Percent] = (unsigned char)(XingPoint & 0xFF);
							}
						}
						tag->flags |= (TAG_XING | TAG_VBRI);
						tag->xing.flags = (TAG_XING_FRAMES | TAG_XING_BYTES | TAG_XING_TOC);

						return 0;
					}
				}
				return -1;
			}

			magic >>= 16;



			ptr = start;
			mad_bit_skip(&ptr, 16);
			bitlen += 16;
		}

		if ((magic & 0x0000ffffL) == (XING_MAGIC & 0x0000ffffL))
			tag->flags |= TAG_VBR;



		if (magic == LAME_MAGIC)
		{
			ptr = start;
			bitlen += 32;
		}
		else if (parse_xing(&tag->xing, &ptr, &bitlen) == 0)
			tag->flags |= TAG_XING;



		if (bitlen >= 20 * 8)
		{
			start = ptr;

			for (i = 0; i < 20; ++i)
			{
				tag->encoder[i] = (char)  mad_bit_read(&ptr, 8);

				if (tag->encoder[i] == 0)
					break;



				if (tag->encoder[i] < 0x20 || tag->encoder[i] >= 0x7f)
				{
					tag->encoder[i] = 0;
					break;
				}
			}

			tag->encoder[20] = 0;
			ptr = start;
		}



		if (memcmp(tag->encoder, "LAME", 4) == 0 && stream->next_frame - stream->this_frame >= 192)
		{
			unsigned short crc = crc_compute((const char *) stream->this_frame, (frame->header.flags & MAD_FLAG_LSF_EXT) ? 175 : 190, 0x0000);
			if (parse_lame(&tag->lame, &ptr, &bitlen, crc) == 0)
			{
				tag->flags |= TAG_LAME;
				tag->encoder[9] = 0;
			}
		}
		else
		{
			for (i = 0; i < 20; ++i)
			{
				if (tag->encoder[i] == 0)
					break;



				if (tag->encoder[i] == 0x55)
				{
					tag->encoder[i] = 0;
					break;
				}
			}
		}

		return 0;
	}




	void rgain_parse(struct rgain *rgain, struct mad_bitptr *ptr)
	{
		int negative;

		rgain->name       = (enum rgain_name) mad_bit_read(ptr, 3);
		rgain->originator = (enum rgain_originator) mad_bit_read(ptr, 3);

		negative          = mad_bit_read(ptr, 1);
		rgain->adjustment = (short) mad_bit_read(ptr, 9);

		if (negative)
			rgain->adjustment = (short) -rgain->adjustment;
	}



	void WMp3::init(WMp3* mp3)
	{

		mp3->c_stream_start = 0;  	mp3->c_stream_size = 0;  	mp3->c_stream_end = 0;   
		mp3->c_start = 0;  	mp3->c_end = 0;    	mp3->c_position = 0; 	mp3->c_size = 0;     
		mp3->c_ID3v1 = 0;	 	mp3->c_ID3v2 = 0;	 

		mp3->c_sampling_rate = 0;      	mp3->c_layer = 0;				 	mp3->c_mode = 0;				 	mp3->c_channel = 0;			 	mp3->c_emphasis = 0;
		mp3->c_mode_extension = 0;
		mp3->c_bitrate = 0;	 	mp3->c_avg_bitrate = 0;	 
		mp3->c_avg_frame_size = 0;	 
		mp3->c_duration = mad_timer_zero;	 	mp3->c_sample_per_frame = 0;	 	mp3->c_flags = 0;
		mp3->c_mpeg_version = 0;		 	mp3->c_mpeg_version_str = " ";  	mp3->c_channel_str = " ";
		mp3->c_emphasis_str = " ";
		mp3->c_layer_str = " ";


		memset(&mp3->xing,0,sizeof(mp3->xing));
		mp3->c_valid_xing = 0;
		mp3->c_xing_frame_size = 0;

		mp3->c_song_length_samples = 0;  	mp3->c_song_length_ms = 0;		 	mp3->c_song_length_bytes = 0;	 	mp3->c_song_length_frames = 0;	 		

		mp3->c_input_buffer_size = 0;	 	
		mp3->c_tmp = 0;	
		mp3->c_play = 0;
		mp3->c_vbr = 0;		
		mp3->c_pause = 0;	 

		mp3->c_dwPos = 0;  	mp3->c_dwIndex = 0;  	mp3->c_vudata_ready = 0;  		

		mp3->c_frame_counter = 0;	 	mp3->c_bitrate_sum = 0;		 
		mp3->c_bLoop = 0;		 	mp3->c_nLoopCount = 0;	 	mp3->c_nLoop = 0;;			 		
		mp3->c_nPosition = 0;

		mp3->c_bFadeOut = FALSE;
		mp3->c_nFadeOutFrameNum = 0;

		mp3->c_bFadeIn = FALSE;






		if(mp3->c_gapless_buffer)
			free(mp3->c_gapless_buffer);


		mp3->c_gapless_buffer = 0;


		memset(&mp3->left_dither,0 ,sizeof(mp3->left_dither));
		memset(&mp3->right_dither,0, sizeof(mp3->right_dither));
		memset(&mp3->c_hms_pos,0, sizeof(MP3_TIME));

	}



	void mono_filter(struct mad_frame *frame)
	{
		if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
		{
			unsigned int ns, s, sb;
			mad_fixed_t left, right, mono;

			ns = MAD_NSBSAMPLES(&frame->header);

			for (s = 0; s < ns; ++s)
			{
				for (sb = 0; sb < 32; ++sb)
				{
					left  = frame->sbsample[0][s][sb];
					right = frame->sbsample[1][s][sb];
					mono = (left + right) / 2;
					frame->sbsample[0][s][sb] = mono;
					frame->sbsample[1][s][sb] = mono;

				}
			}
		}
	}



	void gain_filter(struct mad_frame *frame, mad_fixed_t gain[2])
	{

		unsigned int nch, ch, ns, s, sb;

		nch = MAD_NCHANNELS(&frame->header);
		ns  = MAD_NSBSAMPLES(&frame->header);

		for (ch = 0; ch < nch; ++ch)
		{
			for (s = 0; s < ns; ++s)
			{
				for (sb = 0; sb < 32; ++sb)
				{
					frame->sbsample[ch][s][sb] =
						mad_f_mul(frame->sbsample[ch][s][sb], gain[ch]);
				}
			}
		}

	}



	void experimental_filter(struct mad_frame *frame)
	{
		if (frame->header.mode == MAD_MODE_STEREO ||
			frame->header.mode == MAD_MODE_JOINT_STEREO)
		{

			unsigned int ns, s, sb;

			ns = MAD_NSBSAMPLES(&frame->header);



			for (s = 0; s < ns; ++s)
			{
				for (sb = 0; sb < 32; ++sb)
				{
					mad_fixed_t left, right;

					left  = frame->sbsample[0][s][sb];
					right = frame->sbsample[1][s][sb];

					frame->sbsample[0][s][sb] -= right / 4;
					frame->sbsample[1][s][sb] -= left  / 4;
				}
			}
		}
	}





	void WMp3::MixChannels(BOOL fEnable, unsigned int left_percent, unsigned int right_percent)
	{
		c_mix_channels = fEnable;

		c_l_mix = mad_f_tofixed( (double)left_percent / 100.0); 
		c_r_mix = mad_f_tofixed( (double)right_percent / 100.0);


	}

	void vocal_cut_filter(struct mad_frame *frame)
	{
		if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
		{
			unsigned int ns, s, sb;
			mad_fixed_t left, right;

			ns = MAD_NSBSAMPLES(&frame->header);

			for (s = 0; s < ns; ++s)
			{
				for (sb = 0; sb < 32; ++sb)
				{
					left  = frame->sbsample[0][s][sb] - frame->sbsample[1][s][sb];
					right = frame->sbsample[1][s][sb] - frame->sbsample[0][s][sb];

					frame->sbsample[0][s][sb] = left;
					frame->sbsample[1][s][sb] = right;

				}
			}
		}
	}


	void mix_filter(struct mad_frame *frame, mad_fixed_t left_p, mad_fixed_t right_p)
	{
		if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL)
		{
			unsigned int ns, s, sb;
			mad_fixed_t left, right, mono;

			ns = MAD_NSBSAMPLES(&frame->header);

			for (s = 0; s < ns; ++s)
			{
				for (sb = 0; sb < 32; ++sb)
				{
					left  = frame->sbsample[0][s][sb];
					right = frame->sbsample[1][s][sb];
					mono = mad_f_mul(left, left_p);
					mono = mad_f_add(mono, mad_f_mul(right, right_p)); 

					frame->sbsample[0][s][sb] = mono;
					frame->sbsample[1][s][sb] = mono;

				}
			}
		}
	}



	void WMp3::FadeOutM(unsigned int length_ms)
	{
		c_bFadeIn = FALSE;
		c_bFadeOut = TRUE;

		c_nFadeOutFrameNum = MulDiv(length_ms, c_sampling_rate, c_sample_per_frame * 1000); 
		c_fadeOutStep = 100.0 / (double) c_nFadeOutFrameNum;
		fadeout_gain[0] = MAD_F_ONE;
		fadeout_gain[1] = MAD_F_ONE;
		c_fade_frame_count = 0;

	}


	void WMp3::FadeOut(unsigned int length_seconds)
	{
		FadeOutM(length_seconds * 1000);

	}

	void WMp3::FadeIn(unsigned int length_seconds)
	{
		FadeInM(length_seconds * 1000);

	}

	void WMp3::FadeInM(unsigned int length_ms)
	{

		c_bFadeOut = FALSE;
		c_bFadeIn = TRUE;

		c_nFadeInFrameNum = MulDiv(length_ms, c_sampling_rate, c_sample_per_frame * 1000); 
		c_fadeInStep = 100.0 / (double) c_nFadeInFrameNum;
		fadeout_gain[0] = 0;
		fadeout_gain[1] = 0;
		c_fade_frame_count = 0;

	}




	int WMp3::reallocate_sfx_buffers(WMp3* mp3)
	{

		int i;
		if(mp3->c_old_echo_buffer_size != mp3->c_echo_alloc_buffer_size)
		{

			for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
			{
				if(mp3->sfx_buffer[i])
				{
					free(mp3->sfx_buffer[i]);
					mp3->sfx_buffer[i] = 0;
				}
			}



			for(i = 0; i < ECHO_BUFFER_NUMBER; i++)
			{
				mp3->sfx_buffer[i] = (short*) malloc(mp3->c_echo_alloc_buffer_size);
				mp3->sfx_buffer1[i] = mp3->sfx_buffer[i];
				if(mp3->sfx_buffer[i] == NULL)
				{
					for(int j = 0; j <= i; j++)
					{
						free(mp3->sfx_buffer[j]);
						mp3->sfx_buffer[j] = 0;
						mp3->sfx_buffer1[j] = 0;
					}
					mp3->sfx_buffer1[0] = 0;
					mp3->error("Can't allocate buffers for SFX processor!");

					return 0;		
				}
			}

			mp3->c_old_echo_buffer_size = mp3->c_echo_alloc_buffer_size;

		}  		
		return 1;
	}


	MP3_TIME* WMp3::GetPositionHMS()
	{	
		unsigned int pos = GetPositionM();

		c_hms_pos.h = pos / 3600000;
		pos = pos % 3600000;
		c_hms_pos.m = pos / 60000;
		pos = pos % 60000;
		c_hms_pos.s = pos / 1000;
		c_hms_pos.ms = pos % 1000;

		return &c_hms_pos;
	}


}